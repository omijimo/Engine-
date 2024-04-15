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

    Engine2();

    void SpawnBox(const b2Vec2& p);
    void SpawnCircle(const b2Vec2& p);
    void SpawnTriangle();
    void ShiftMouseDown(const b2Vec2& p) override;
    void Push(b2World* world, b2Vec2 mousePosition);
    void UpdateUI() override;

    void SpawnEquilateralTriangle(const b2Vec2 &p);
};

#endif // ENGINE2_H
