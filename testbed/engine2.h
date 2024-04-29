#ifndef ENGINE2_H
#define ENGINE2_H

#include "box2d/box2d.h"
#include "tests/particleSystem.h"
#include "test.h"
#include "imgui/imgui.h"
#include <stdlib.h>
#include <stdio.h>

class Engine2 : public Test {
public:
    const float box_minX = -30.0f;
    const float box_minY = -20.0f;
    const float box_maxX = 30.0f;
    const float box_maxY = 20.0f;
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
  
    // elasticity -- applies to next spawned object
    float elasticity = 1.0f;

    int lattice_height = 2;
    int lattice_width = 2;

    float joint_length = 0.5f;       // This is the length of non-diagonal joints

    float lattice_stiffness = 20.f;
    float lattice_damping = 1.f;
    
    // particle
    bool particleFlag = false; // set true/false when clicked on corresponding button
    particleDef engine_particleDef;
    particleSystem engine_particleSys;
  
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
    void CreateJoint(b2Body* body_a, b2Body* body_b);

    // For soft bodies
    void MakeLattice(b2Vec2 position);
  
    void Step(Settings& settings) override;
};

#endif // ENGINE2_H
