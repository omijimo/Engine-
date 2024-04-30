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
  
//    int createParticle(const particleDef &def, const b2Vec2 &p, b2World *world);
    void createParticlesAroundMouse(const particleDef &def, const b2Vec2 &mousePos, float radius, int numParticles, b2World *world);
    void simulateStep(const b2TimeStep& step);
    void SolveGravity(const b2TimeStep& step);
    void LimitVelocity(const b2TimeStep& step);
    void SolveCollision(const b2TimeStep& step);
  
    float density = 1.0f;
    float gravityScale = 0.2f;
    float radius = 1.0f; // for spawning multiple at once
    float particleRadius = 0.5f; // for applying force
    int maxCount = 500; // for limiting how much to spawn
    
    float dampingStrength = 1.0f;
    float elasticStrength = 0.25f;
    float repulsiveStrength = 1.0f;
    
    b2World *psys_world;
    int p_count = 0;
    bool p_paused = false;
  
    int32 psys_iterationIndex;
    int32 psys_timestamp;
    std::vector<particleDef> particles; // can def get rid of this
    
    std::vector<b2Vec2> velocityBuffer;
    std::vector<b2Vec2> positionBuffer;
  
    particleSystem();
    particleSystem(b2World *world, Settings& settings);
  
  private:
    void applyParticleForces();
    void applyDamping();
  
};

#endif
