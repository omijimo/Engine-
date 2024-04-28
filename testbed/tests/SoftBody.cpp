//
// Created by Wil on 4/27/2024.
//
#include "test.h"
#include "soft_body.h"
#include "imgui/imgui.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

SoftBody::SoftBody() {
    /** Literally stolen from engine2.cpp */
    m_world->SetGravity(b2Vec2(0.0f, -10.0f));

    // the 'box' in which our objects exist/should not have anything outside it
    b2Body* ground;
    {
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

static float width = 1.0f;
static float length = 1.0f;
static float box_mass = 1.0f;


void SoftBody::ShiftMouseDown(const b2Vec2& p) {
    m_mouseWorld = p;

    if (m_mouseJoint != NULL)
    {
        return;
    }

    m_bombSpawnPoint = m_mouseWorld;
    m_bombSpawning = true;
}

void SoftBody::CreateRect(const b2Vec2& position, const b2Vec2& velocity)
{
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position = position;
    bd.bullet = true;
    m_bomb = m_world->CreateBody(&bd);
    m_bomb->SetLinearVelocity(velocity);

    b2Shape *object;
    //object = new b2PolygonShape(b2CircleShape());

    b2FixtureDef fd;
    fd.shape = object;
    fd.density = 20.0f;
    fd.restitution = 0.0f;

    b2Vec2 minV = position - b2Vec2(0.3f,0.3f);
    b2Vec2 maxV = position + b2Vec2(0.3f,0.3f);

    b2AABB aabb;
    aabb.lowerBound = minV;
    aabb.upperBound = maxV;

    m_bomb->CreateFixture(&fd);
}

//void SoftBody::Push(b2World* world, b2Vec2 mousePosition) {
//    if (pushEnabled) {
//        b2CircleShape circle;
//        circle.m_radius = 2.0f;
//        circle.m_p.SetZero();
//
//        b2BodyDef bd;
//        bd.type = b2_dynamicBody;
//        bd.position = mousePosition;
//        b2Body *body = m_world->CreateBody(&bd);
//        body->CreateFixture(&circle, 0.1f);
//
////      b2Vec2 mouseDirection = mousePosition ;
//
//        // Iterate through bodies in the Box2D world
////      for (b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
////          body->ApplyForceToCenter(pushDirection, true);
////      }
//    }
//}

// Update GUI to add custom controls
//void SoftBody::UpdateUI() {
//    ImGui::SetNextWindowPos(ImVec2(10.0f, 100.0f));
//    ImGui::SetNextWindowSize(ImVec2(200.0f, 325.0f));
//    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
//
//    // Buttons to spawn our basic primitives: Box, Circle, Triangle
//    // Spawn by Shift + Left Mouse Click
//    if (ImGui::Button("Spawn Box"))
//    {
//        shape = 'b';
////    SpawnBox();
//    }
//    if (ImGui::IsItemHovered()) {
//        ImGui::BeginTooltip();
//        ImGui::Text("Spawns a box");
//        ImGui::EndTooltip();
//    }
//
//    ImGui::Indent();
//
//    if (ImGui::SliderFloat("Height", &length, 0.01f, 10.0f));
//
//
//    if (ImGui::SliderFloat("Width", &width, 0.01f, 10.0f));
//
//    if (ImGui::SliderFloat("Box Mass", &box_mass, 0.1f, 100.0f));
//
//    ImGui::Unindent();
//
//    if (ImGui::Button("Spawn Circle"))
//    {
//        shape = 'c';
////    SpawnCircle();
//    }
//    if (ImGui::IsItemHovered()) {
//        ImGui::BeginTooltip();
//        ImGui::Text("Spawns a circle");
//        ImGui::EndTooltip();
//    }
//    ImGui::Indent();
//
//    if (ImGui::SliderFloat("Radius", &radius, 0.01f, 10.0f));
//
//    if (ImGui::SliderFloat("Circle Mass", &circle_mass, 0.1f, 100.0f));
//
//
//    ImGui::Unindent();
//
//    if (ImGui::Button("Spawn Triangle"))
//    {
//        shape = 't';
////    SpawnEquilateralTriangle();
//    }
//
//    if (ImGui::IsItemHovered()) {
//        ImGui::BeginTooltip();
//        ImGui::Text("Spawns a triangle");
//        ImGui::EndTooltip();
//    }
//
//    ImGui::Indent();
//
//    if (ImGui::SliderFloat("Side Length", &triangle_size, 0.01f, 10.0f));
//
//    if (ImGui::SliderFloat("Triangle Mass", &triangle_mass, 0.1f, 100.0f));
//
//
//    ImGui::Unindent();
//
//    // Push
//    if (ImGui::Button(pushEnabled ? "Disable Push" : "Enable Push")) {
//        pushEnabled = !pushEnabled;
//        ImVec2 mousePos = ImGui::GetMousePos();
//        b2Vec2 b2_mousePos = b2Vec2(mousePos.x, mousePos.y);
//        Push(m_world, b2_mousePos);
//    }
//    if (ImGui::IsItemHovered()) {
//        ImGui::BeginTooltip();
//        ImGui::Text("Enabling this will make it so your mouse exerts a pushing force");
//        ImGui::EndTooltip();
//    }
//
//    ImGui::End();
//}

// Register the test with the test framework
static Test* Create() { return new SoftBody; }
static int testIndex = RegisterTest("Benchmark", "Soft Body", Create);
