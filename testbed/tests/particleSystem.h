#ifndef PARTICLE_SYSTEM
#define PARTICLE_SYSTEM

#include "box2d/box2d.h"
#include "test.h"
#include "vector"

//struct particleSystemDef {
//  particleSystemDef() {
//    density = 1.0f;
//    gravityScale = 1.0f;
//    radius = 1.0f; // for spawning multiple at once
//    maxCount = 0; // for limiting how much to spawn
//    
//    dampingStrength = 1.0f;
//    elasticStrength = 0.25f;
//    repulsiveStrength = 1.0f;
//  }
//  
//  float density;
//  float gravityScale;
//  float radius;
//  int maxCount;
//  
//  float dampingStrength;
//  float elasticStrength;
//  float repulsiveStrength;
//};

struct particleDef {
  b2PolygonShape shape;
  b2Body *body; // for collisions
  b2Vec2 position;
  b2Vec2 velocity;
};

class particleSystem : public Test {
  public:
  
    b2PolygonShape SpawnEquilateralTriangle(const b2Vec2 &p, float scale);
  
    int createParticle(const particleDef &def, const b2Vec2 &p, b2World *world);
    void createParticlesAroundMouse(const particleDef &def, const b2Vec2 &mousePos, float radius, int numParticles, b2World *world);
    void simulateFluidStep(std::vector<particleDef> &particles, float deltaTime);
//    void DestroyParticle(const particleDef &def);
  
    float density = 1.0f;
    float gravityScale = 1.0f;
    float radius = 1.0f; // for spawning multiple at once
    int maxCount = 0; // for limiting how much to spawn
    
    float dampingStrength = 1.0f;
    float elasticStrength = 0.25f;
    float repulsiveStrength = 1.0f;
    
    int p_count;
    std::vector<particleDef> particles;
  
    int m_nextParticleIndex = 0;
    std::vector<b2Body*> m_bodies;
  
    particleSystem();
};

#endif
