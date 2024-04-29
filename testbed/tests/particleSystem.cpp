#include "box2d/box2d.h"
#include "particle.h"
#include "particleSystem.h"

particleSystem::particleSystem() {
  p_count = 0;
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

int particleSystem::createParticle(const particleDef &def, const b2Vec2 &p, b2World *world) {
  particleDef newParticleDef = def;
  newParticleDef.position = p;
  newParticleDef.velocity.SetZero();  
  
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position = p;
  b2Body* body = world->CreateBody(&bodyDef);
  
  b2PolygonShape triangleShape = SpawnEquilateralTriangle(p, 0.25f);
  newParticleDef.shape = triangleShape;
  
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &newParticleDef.shape;
  fixtureDef.density = 1.0f; // Adjust density as needed
  fixtureDef.restitution = 0.5f; // Adjust restitution as needed
  body->CreateFixture(&fixtureDef);
  
  // Set user data of the body to store particle information
//  body->SetUserData(static_cast<void*>(&newParticleDef));
  
  newParticleDef.body = body;
  
  particles.push_back(newParticleDef);
  p_count++;
  
  return p_count - 1;
}

void particleSystem::createParticlesAroundMouse(const particleDef &def, const b2Vec2 &mousePos, float radius, int numParticles, b2World *world) {
    // Generate random angles for particles
    for (int i = 0; i < numParticles; ++i) {
        // Generate random angle
        float angle = RandomFloat(0, 2 * b2_pi);
        // Calculate random position within the specified radius around the mouse position
        float xOffset = radius * cos(angle);
        float yOffset = radius * sin(angle);
        b2Vec2 position = mousePos + b2Vec2(xOffset, yOffset);
        
        createParticle(def, position, world);
    }
}
