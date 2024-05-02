#ifndef ENGINE2_H
#define ENGINE2_H

#include "box2d/box2d.h"
#include "test.h"
#include "box2d/particleSystem.h"
#include "imgui/imgui.h"
#include <stdlib.h>
#include <stdio.h>

class Engine2 : public Test {
public:

    const float originX = 0.0f;
    const float originY = 0.0f;

    float arena_width = 60.0f;
    float arena_height = 40.0f;

    float box_minX;
    float box_minY;
    float box_maxX;
    float box_maxY;

    float gravityX = 0.0f;
    float gravityY = -10.0f;


    const float k_restitution = 0.0f;
    bool pushEnabled = false;
    char shape = 'b'; // 'c' means circle, 'b' means box/square, 't' means triangle, 'l' means soft body lattice
  
    // sizes for box/square, circle, triangle
    float width = 1.0f;
    float length = 1.0f;
    float box_mass = 1.0f;
  
    float radius = 25.f;
    float circle_mass = 1.0f;
  
    float triangle_size = 1.0f;
    float triangle_mass = 1.0f;

    // for setting the arena
    b2Fixture* fixtureList[4];
    bool hasGround = false;
    b2Body* currGround = nullptr;


  
    // elasticity -- applies to next spawned object
    float elasticity = 1.0f;

    int lattice_height = 2;
    int lattice_width = 2;

    float joint_length = 0.5f;       // This is the length of non-diagonal joints

    float lattice_stiffness = 20.f;
    float lattice_damping = 1.f;

    float lattice_mass = 1.0f;
  
    // particle
    bool particleFlag = false;
    particleDef pDef;
//    particleSystem psys;

    Engine2();

    b2PolygonShape SpawnBox(const b2Vec2& p);
    b2CircleShape SpawnCircle(const b2Vec2& p);
    b2PolygonShape SpawnEquilateralTriangle(const b2Vec2 &p);
    void MouseDown(const b2Vec2& p) override;
    void ShiftMouseDown(const b2Vec2& p) override;
    void LaunchBomb(const b2Vec2& position, const b2Vec2& velocity) override;
    void CompleteBombSpawn(const b2Vec2& p) override;
    void UpdateUI() override;
    b2Body* UpdateGround();

    void SetGround();
    void ResetGravity();

    void CreateJoint(b2Body* body_a, b2Body* body_b);

    // For soft bodies
    void MakeLattice(b2Vec2 position);
};

#endif // ENGINE2_H
