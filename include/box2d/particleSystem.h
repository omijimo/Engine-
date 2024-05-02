#ifndef PARTICLE_SYSTEM
#define PARTICLE_SYSTEM

#include "box2d/box2d.h"
#include "box2d/spatialHash.h"
#include <stdlib.h>
#include "vector"

struct particleDef {
  particleDef() {
    position = b2Vec2();
    position.SetZero();
    velocity = b2Vec2();
    velocity.SetZero();
  }
  b2PolygonShape shape;
  b2Body *body; // for collisions
  b2Vec2 position;
  b2Vec2 velocity;
};

class particleSystem {
  public:
    b2PolygonShape SpawnEquilateralTriangle(const b2Vec2 &p, float scale);
    void createParticle(b2World* world, const b2Vec2& position, float scale);
    void createParticlesAroundMouse(const b2Vec2 &mousePos, float radius, int numParticles, b2World *world);
    void limitVelocity(float timeStep);
    void update(float timeStep);
    
    int maxCount = 0; // for limiting how much to spawn
    
    int p_count;
  
    particleSystem();
    particleSystem(float width, float height, float cellSize, float worldminX, float worldminY, b2World* w);
//    ~particleSystem();
  
  private:
    b2World* world;
//    particleGrid* grid;
    spatialHash* hm;
    std::vector<b2Body*> particles;
    float width, height, cellSize;
};

#endif
