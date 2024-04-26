#ifndef ENGINE2_H
#define ENGINE2_H

#include "box2d/box2d.h"
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
    char shape = 'b'; // 'c' means circle, 'b' means box/square, 't' means triangle
  
    // sizes for box/square, circle, triangle
    float width = 1.0f;
    float length = 1.0f;
    float box_mass = 1.0f;
  
    float radius = 1.0f;
    float circle_mass = 1.0f;
  
    float triangle_size = 1.0f;
    float triangle_mass = 1.0f;
  
    // elasticity -- applies to next spawned object
    float elasticity = 1.0f;

    Engine2();

    b2PolygonShape SpawnBox(const b2Vec2& p);
    b2CircleShape SpawnCircle(const b2Vec2& p);
    b2PolygonShape SpawnEquilateralTriangle(const b2Vec2 &p);
    void ShiftMouseDown(const b2Vec2& p) override;
    void LaunchBomb(const b2Vec2& position, const b2Vec2& velocity) override;
    void CompleteBombSpawn(const b2Vec2& p) override;
    void Push(b2World* world, b2Vec2 mousePosition);
    void UpdateUI() override;
};

#endif // ENGINE2_H
