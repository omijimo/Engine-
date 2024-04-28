//
// Created by Wil on 4/28/2024.
//

#ifndef BOX2D_SOFT_BODY_H
#define BOX2D_SOFT_BODY_H

#endif //BOX2D_SOFT_BODY_H
#include "test.h"
#include "imgui/imgui.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

class SoftBody : public Test {
public:
    SoftBody();

    float const k_restitution = 1.f;

    void ShiftMouseDown(const b2Vec2& p);
    void CreateRect(const b2Vec2& position, const b2Vec2& velocity);

};
