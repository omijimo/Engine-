#include "box2d/box2d.h"
#include "box2d/particleSystem.h"
#include "box2d/spatialHash.h"
#include "vector"

// SPATIAL HASH

int spatialHash::hashPosition(const b2Vec2& position) {
  int x = static_cast<int>(floor(position.x / cellSize));
  int y = static_cast<int>(floor(position.y / cellSize));
  return std::hash<int>{}(x ^ (y << 16));
}

spatialHash::spatialHash(float cellSize) : cellSize(cellSize) {}

void spatialHash::insert(b2Body* body) {
  int key = hashPosition(body->GetPosition());
  table[key].push_back(body);
}

void spatialHash::remove(b2Body* body, const b2Vec2& oldPosition) {
  int key = hashPosition(oldPosition);
  auto& bucket = table[key];
  auto iter = std::find(bucket.begin(), bucket.end(), body);
  if (iter != bucket.end()) {
    bucket.erase(iter);
  }
}

std::vector<b2Body*> spatialHash::query(const b2Vec2& position, float queryRadius) {
  std::vector<b2Body*> neighbors;
  int minX = static_cast<int>(floor((position.x - queryRadius) / cellSize));
  int maxX = static_cast<int>(floor((position.x + queryRadius) / cellSize));
  int minY = static_cast<int>(floor((position.y - queryRadius) / cellSize));
  int maxY = static_cast<int>(floor((position.y + queryRadius) / cellSize));
  
  for (int x = minX; x <= maxX; x++) {
    for (int y = minY; y <= maxY; y++) {
      int key = std::hash<int>{}(x ^ (y << 16));
      if (table.count(key)) {
        for (b2Body* body : table[key]) {
          if ((body->GetPosition() - position).Length() <= queryRadius) {
            neighbors.push_back(body);
          }
        }
      }
    }
  }
  
  return neighbors;
}

// PARTICLE SYSTEM

particleSystem::particleSystem(float width, float height, float cellSize, float worldminX, float worldminY, b2World* w)
: width(width), height(height), cellSize(cellSize) {
  
  world = w;
  hm = new spatialHash(cellSize);
}

particleSystem::particleSystem() {
  p_count = 0;
}

b2PolygonShape particleSystem::SpawnEquilateralTriangle(const b2Vec2 &position, float scale) {
  b2PolygonShape triangle;
  // Constant defining the side-length of the triangle.
  const float hs = 1.0f * scale;
  const float angle = 0.0f;
  
  const float height = (sqrt(3.0f) / 2.0f) * (2.0f * hs);
  const float offsetY = height / 3.0f;
  
  b2Vec2 vertices[3];
  vertices[0].Set(0.0f, hs);
  vertices[1].Set(-hs, -offsetY);
  vertices[2].Set(hs, -offsetY);
  int count = 3;
  
  triangle.Set(vertices, count);
  
  return triangle;
}

void particleSystem::createParticle(b2World* world, const b2Vec2& position, float scale) {
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position = position;
  b2Body* body = world->CreateBody(&bodyDef);
  
  b2CircleShape particleShape;
  particleShape.m_radius = 0.25f;
  particleShape.isParticle = true;
  
  b2FixtureDef fixtureDef;
  fixtureDef.shape = new b2CircleShape(particleShape);
  fixtureDef.density = 1.0f;
  fixtureDef.friction = 0.1f;
  fixtureDef.restitution = 0.1f;  // Low restitution for water-like behavior
  
  body->CreateFixture(&fixtureDef);
  
  particles.push_back(body);
  hm->insert(body);
  
  p_count++;
}

void particleSystem::createParticlesAroundMouse(const b2Vec2 &mousePos, float radius, int numParticles, b2World *world) {
  const float angleIncrement = 2 * b2_pi / numParticles;
  
  // Generate random angles for particles
  for (int i = 0; i < numParticles; ++i) {
    float angle = i * angleIncrement;
    float xOffset = radius * cos(angle);
    float yOffset = radius * sin(angle);
    b2Vec2 position = mousePos + b2Vec2(xOffset, yOffset);
    createParticle(world, position, 0.25f);
  }
}

b2Vec2 calculateCohesionForce(b2Body* particle, const std::vector<b2Body*>& neighbors) {
  if (neighbors.empty()) {
    return b2Vec2(0,0);
  };
  
  b2Vec2 centerOfMass(0,0);
  for (const auto& neighbor : neighbors) {
    centerOfMass += neighbor->GetPosition();
  }
  centerOfMass *= (1.0 / neighbors.size());
  
  b2Vec2 cohesionForce = centerOfMass - particle->GetPosition();
  cohesionForce.x *= 10;
  cohesionForce.y *= 10;
  return cohesionForce;
}

b2Vec2 calculateSeparationForce(b2Body* particle, const std::vector<b2Body*>& neighbors) {
  if (neighbors.empty()) {
    return b2Vec2(0,0);
  };
  
  b2Vec2 separation(0,0);
  for (const auto& neighbor : neighbors) {
    b2Vec2 diff = particle->GetPosition() - neighbor->GetPosition();
    float distSquared = diff.LengthSquared();
    float desiredSeparation = 0.05;
    if (diff.LengthSquared() < 0.01) { // Check for very close proximity
      diff.Normalize();
      float strength = 1.0 - (diff.LengthSquared() / (0.05 * 0.05));
      diff.x *= strength;
      diff.y *= strength;
      separation += diff;
    }
  }
  return separation;
}

b2Vec2 calculateViscousForce(b2Body* particle, const std::vector<b2Body*>& neighbors) {
  if (neighbors.empty()) {
    return b2Vec2(0,0);
  }
  
  b2Vec2 velocityDiff(0,0);
  for (const auto& neighbor : neighbors) {
    velocityDiff += (particle->GetLinearVelocity() - neighbor->GetLinearVelocity());
  }
  
  velocityDiff *= -(0.3 / neighbors.size());
  return velocityDiff;
}

float getCriticalVelocitySquared(float timeStep) {
  float velocity = 0.4 * 1/timeStep;
  return velocity * velocity;
}

void particleSystem::limitVelocity(float timeStep) {
  float criticalVelocitySquared = getCriticalVelocitySquared(timeStep);
  
  for (auto& particle : particles) {
    b2Vec2 velocity = particle->GetLinearVelocity();
    float velocityMagnitudeSquared = velocity.LengthSquared();
    
    if (velocityMagnitudeSquared > criticalVelocitySquared) {
      velocity *= b2Sqrt(criticalVelocitySquared / velocityMagnitudeSquared);
      particle->SetLinearVelocity(velocity);
    }
  }
}

void particleSystem::update(float timeStep) {
  std::vector<b2Vec2> oldPositions(particles.size());
  for (size_t i = 0; i < particles.size(); ++i) {
    oldPositions[i] = particles[i]->GetPosition();
  }
  
  for (auto& particle : particles) {
    b2Vec2 position = particle->GetPosition();
    float queryRadius = 1.0f;
    std::vector<b2Body*> neighbors = hm->query(position, queryRadius);
    
    b2Vec2 cohesion = calculateCohesionForce(particle, neighbors);
    b2Vec2 separation = calculateSeparationForce(particle, neighbors);
    b2Vec2 viscosity = calculateViscousForce(particle, neighbors);
    
    particle->ApplyForceToCenter(cohesion + separation + viscosity, true);
    
  }
  
  world->Step(timeStep, 8, 3);
  limitVelocity(timeStep);
  
  // Update spatial hash for all particles
  for (size_t i = 0; i < particles.size(); ++i) {
    hm->remove(particles[i], oldPositions[i]);
    hm->insert(particles[i]);
  }
}
