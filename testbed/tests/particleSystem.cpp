#include "box2d/box2d.h"
#include "settings.h"
#include "particle.h"
#include "particleSystem.h"

particleSystem::particleSystem() {}

particleSystem::particleSystem(b2World *world, Settings& settings) {
  p_count = 0;
  psys_world = world;
  p_paused = settings.m_pause;
  psys_timestamp = 0;
}

b2PolygonShape particleSystem::SpawnEquilateralTriangle(const b2Vec2 &p, float scale) {
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

//int particleSystem::createParticle(const particleDef &def, const b2Vec2 &p, b2World *world) {
//  particleDef newParticleDef = def;
//  newParticleDef.position = p;
//  newParticleDef.velocity.SetZero();  
//  
//  b2BodyDef bodyDef;
//  bodyDef.type = b2_dynamicBody;
//  bodyDef.position = p;
//  b2Body* body = world->CreateBody(&bodyDef);
//  
//  b2PolygonShape triangleShape = SpawnEquilateralTriangle(p, 0.25f);
//  newParticleDef.shape = triangleShape;
//  
//  b2FixtureDef fixtureDef;
//  fixtureDef.shape = &newParticleDef.shape;
//  fixtureDef.density = 1.0f; // Adjust density as needed
//  fixtureDef.restitution = 0.5f; // Adjust restitution as needed
//  body->CreateFixture(&fixtureDef);
//  
//  newParticleDef.body = body;
//  
//  particles.push_back(newParticleDef);
//  p_count++;
//  
//  return p_count - 1;
//}

void particleSystem::createParticlesAroundMouse(const particleDef &def, const b2Vec2 &mousePos, float radius, int numParticles, b2World *world) {
    // Generate random angles for particles
    for (int i = 0; i < numParticles; ++i) {
      if (p_count < maxCount) {
        // Generate random angle
        float angle = RandomFloat(0, 2 * b2_pi);
        // Calculate random position within the specified radius around the mouse position
        float xOffset = radius * cos(angle);
        float yOffset = radius * sin(angle);
        b2Vec2 position = mousePos + b2Vec2(xOffset, yOffset);
        
        particleDef newParticleDef = def;
        newParticleDef.position = mousePos;
        newParticleDef.velocity.SetZero();
        
        particles.push_back(newParticleDef);
        positionBuffer.push_back(newParticleDef.position);
        velocityBuffer.push_back(newParticleDef.velocity);
      } else {
        printf("****CANNOT ADD ANYMORE PARTICLES (it will lag)****\n");
      }
    }
}

void particleSystem::applyParticleForces() {
  // Apply forces between neighboring particles within the radius
  for (size_t i = 0; i < particles.size(); i++) {
    for (size_t j = i + 1; j < particles.size(); j++) {
      b2Vec2& p1 = particles[i].position;
      b2Vec2& p2 = particles[j].position;
      float distance = b2Distance(p1, p2);
      
      if (distance < particleRadius) {
        // Calculate force based on distance using Hooke's law
        float springConstant = 1.0f; // Adjust as needed
        b2Vec2 direction = (p2 - p1);
        direction.Normalize(); // Normalize direction vector
        float forceMagnitude = springConstant * (particleRadius - distance); // Hooke's law
        float force = forceMagnitude * distance;
        
        particles[i].velocity.x += force;
        particles[i].velocity.y += force;
        
        particles[j].velocity.x -= force;
        particles[j].velocity.y -= force;
      }
    }
  }
}

void particleSystem::simulateStep(const b2TimeStep& step) {
  b2Assert(positionBuffer.size() == velocityBuffer.size());
  if (p_count == 0) {
    return;
  }
  if (p_paused) {
    return;
  }
  
  for (psys_iterationIndex = 0; psys_iterationIndex < step.particleIterations; psys_iterationIndex++) {
    ++psys_timestamp;
    b2TimeStep subStep = step;
    subStep.dt /= step.particleIterations;
    subStep.inv_dt *= step.particleIterations;
    
    applyParticleForces();
    for (int i = 0; i < p_count; i++) {
      positionBuffer[i] += subStep.dt * velocityBuffer[i];
    }
  }
  
//  applyParticleForces();
//  for (int i = 0; i < positionBuffer.size(); i++) {
//    positionBuffer[i].x += velocityBuffer[i].x * dt;
//    positionBuffer[i].y += velocityBuffer[i].y * dt;
//  }
}
