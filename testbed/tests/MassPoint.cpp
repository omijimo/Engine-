//
// Created by Wil on 4/28/2024.
//

#include "box2d/b2_api.h"
#include "box2d/b2_math.h"
#include "box2d/b2_shape.h"
#include "MassPoint.h"
#include "box2d/box2d.h"
#include "imgui/imgui.h"
#include "test.h"
#include <stdio.h>
#include <iostream>

using namespace std;

MassPoint::MassPoint() {
    /// Constants

    /// Constructor
    m_world->SetGravity(b2Vec2(0.0f, -10.0f));

    b2Body* ground;
    {
        /// Stolen from engine2.cpp
        b2BodyDef bd;
        bd.position.Set(0.0f, 20.0f);
        ground = m_world->CreateBody(&bd);

        b2EdgeShape shape;

        b2FixtureDef sd;
        sd.shape = &shape;
        sd.density = 0.0f;
        sd.restitution = k_restitution;

        // Left vertical
        shape.SetTwoSided(b2Vec2(-30.0f, -20.0f), b2Vec2(-30.0f, 20.0f));
        ground->CreateFixture(&sd);

        // Right vertical
        shape.SetTwoSided(b2Vec2(30.0f, -20.0f), b2Vec2(30.0f, 20.0f));
        ground->CreateFixture(&sd);

        // Top horizontal
        shape.SetTwoSided(b2Vec2(-30.0f, 20.0f), b2Vec2(30.0f, 20.0f));
        ground->CreateFixture(&sd);

        // Bottom horizontal
        shape.SetTwoSided(b2Vec2(-30.0f, -20.0f), b2Vec2(30.0f, -20.0f));
        ground->CreateFixture(&sd);
    }
}

// TODO: Change from engine2.cpp??

void MassPoint::SpawnPoint(const b2Vec2& posn) {

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position = position;


    b2FixtureDef fd;

    b2CircleShape circle;
    circle.m_radius = radius;

    fd.shape = new b2CircleShape(circle);
    fd.density = 20.0f;
    fd.restitution = 0.0f;

    b2Vec2 minV = position - b2Vec2(0.3f, 0.3f);
    b2Vec2 maxV = position + b2Vec2(0.3f, 0.3f);

    b2AABB aabb;
    aabb.lowerBound = minV;
    aabb.upperBound = maxV;

    m_bomb->CreateFixture(&fd);
}
