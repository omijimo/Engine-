//
// Created by Wil on 4/28/2024.
//

#include "box2d/b2_api.h"
#include "box2d/b2_math.h"
#include "box2d/b2_shape.h"
#include "SoftBody.cpp"
#include "box2d/box2d.h"
#include "imgui/imgui.h"
#include "test.h"

MassPoint::MassPoint() {
    /// Constructor
    m_world->SetGravity(b2Vec2(0.0f, -10.0f));

    b2Body* ground;
    {
        /// Solen from engine2.cpp
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
static float width = 1.0f;
static float length = 1.0f;
static float box_mass = 1.0f;

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

    b2Vec2 minV = position - b2Vec2(0.3f,0.3f);
    b2Vec2 maxV = position + b2Vec2(0.3f,0.3f);

    b2AABB aabb;
    aabb.lowerBound = minV;
    aabb.upperBound = maxV;

    m_bomb->CreateFixture(&fd);
}

void MassPoint::MakeRectangularLattice(unsigned int length, unsigned int width) {

}




/// The following code was stolen from engine2.cpp because I have three brain cells
void MassPoint::Push(b2World* world, b2Vec2 mousePosition) {
    b2CircleShape circle;
    circle.m_radius = 2.0f;
    circle.m_p.SetZero();

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position = mousePosition;
    b2Body *body = m_world->CreateBody(&bd);
    body->CreateFixture(&circle, 0.1f);

//      b2Vec2 mouseDirection = mousePosition ;

    // Iterate through bodies in the Box2D world
//      for (b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
//          body->ApplyForceToCenter(pushDirection, true);
//      }
}

// Update GUI to add custom controls
void MassPoint::UpdateUI() {
    ImGui::SetNextWindowPos(ImVec2(10.0f, 100.0f));
    ImGui::SetNextWindowSize(ImVec2(200.0f, 325.0f));
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Spawns a box");
        ImGui::EndTooltip();
    }

    ImGui::Indent();

    ImGui::Unindent();


    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Spawns a circle");
        ImGui::EndTooltip();
    }
    ImGui::Indent();

    if (ImGui::SliderFloat("Radius", &radius, 0.01f, 10.0f));


    ImGui::Unindent();


    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Spawns a triangle");
        ImGui::EndTooltip();
    }

    ImGui::Indent();




    ImGui::Unindent();

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Enabling this will make it so your mouse exerts a pushing force");
        ImGui::EndTooltip();
    }

    ImGui::End();
}

MassPointLattice::MassPointLattice() {

}

static Test* Create() { return new MassPointLattice; }
static int testIndex = RegisterTest("Benchmark", "Soft Body", Create);
