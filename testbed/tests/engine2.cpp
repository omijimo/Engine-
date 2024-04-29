#include "test.h"
#include "engine2.h"
#include "imgui/imgui.h"
#include <stdlib.h>
#include "MassPoint.h"
#include <stdio.h>
#include <iostream>

using namespace std;
b2Body* Engine2::UpdateGround() {
    b2Body* ground;
    {
        b2BodyDef bd;
        bd.position.Set(0.0f, 0.0f);
        ground = m_world->CreateBody(&bd);

        b2EdgeShape shape;

        b2FixtureDef sd;
        sd.shape = &shape;
        sd.density = 0.0f;
        sd.restitution = k_restitution;

        // Left vertical
        shape.SetTwoSided(b2Vec2(box_minX, box_minY), b2Vec2(box_minX, box_maxY));
        ground->CreateFixture(&sd);

        // Right vertical
        shape.SetTwoSided(b2Vec2(box_maxX, box_minY), b2Vec2(box_maxX, box_maxY));
        ground->CreateFixture(&sd);

        // Top horizontal
        shape.SetTwoSided(b2Vec2(box_minX, box_maxY), b2Vec2(box_maxX, box_maxY));
        ground->CreateFixture(&sd);

        // Bottom horizontal
        shape.SetTwoSided(b2Vec2(box_minX, box_minY), b2Vec2(box_maxX, box_minY));
        ground->CreateFixture(&sd);
    }
    return ground;
}
Engine2::Engine2() {
    m_world->SetGravity(b2Vec2(0.0f, -10.0f));

    // the 'box' in which our objects exist/should not have anything outside it
    b2Body* ground = UpdateGround();

}



b2PolygonShape Engine2::SpawnBox(const b2Vec2& p) {
    b2PolygonShape box;
    box.SetAsBox(width, length);
    return box;
}

b2CircleShape Engine2::SpawnCircle(const b2Vec2& p) {
    b2CircleShape circle;
    circle.m_radius = radius;
    return circle;
}

b2PolygonShape Engine2::SpawnEquilateralTriangle(const b2Vec2& p) {
    /// Made by Wil
    /**
     * Spawns a triangle, abstracted with barycentric coordinates
     */
    b2PolygonShape triangle;

    // Constant defining the side-length of the triangle.
    const float hs = 1.0f;
    const float angle = 0.0f;

    const float height = (sqrt(3.0f) / 2.0f) * (2.0f * hs);
    const float offsetY = height / 3.0f;

    b2Vec2 vertices[3];
    vertices[0].Set(0.0f, hs);
    vertices[1].Set(-hs, -offsetY);
    vertices[2].Set(hs, -offsetY);
    int count = 3;

    triangle.Set(vertices, count);

    return triangle;
}

void Engine2::ShiftMouseDown(const b2Vec2& p) {
    m_mouseWorld = p;

    if (m_mouseJoint != NULL)
    {
        return;
    }

    m_bombSpawnPoint = m_mouseWorld;
    m_bombSpawning = true;
}

void Engine2::LaunchBomb(const b2Vec2& position, const b2Vec2& velocity)
{
    bool rigid_body = true;
    b2Shape *object;
    switch (shape) {
        case 'b':     // box
        default:
            object = new b2PolygonShape(SpawnBox(position));
            break;
        case 'c':     // circle
            object = new b2CircleShape(SpawnCircle(position));
            break;
        case 't':     // triangle
            object = new b2PolygonShape(SpawnEquilateralTriangle(position));
            break;
        case 'l':     // soft body lattice
            rigid_body = false;
            MakeLattice(position);
            break;
    }



    if (rigid_body) {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position = position;
        bd.bullet = true;
        m_bomb = m_world->CreateBody(&bd);
        m_bomb->SetLinearVelocity(velocity);

        b2FixtureDef fd;
        fd.shape = object;
        fd.density = 20.0f;
        fd.restitution = elasticity; // for elastic collision

//      b2Vec2 minV = position - b2Vec2(0.3f, 0.3f);
//      b2Vec2 maxV = position + b2Vec2(0.3f, 0.3f);
//
//      b2AABB aabb;
//      aabb.lowerBound = minV;
//      aabb.upperBound = maxV;

        m_bomb->CreateFixture(&fd);
    }
}

void Engine2::MakeLattice(b2Vec2 position) {
    if (lattice_height < 1 || lattice_width < 1) {      // Edge case
        cout << "Invalid Dimensions. " << lattice_height << " x " << lattice_width << " is not a valid rectangular soft body lattice.";
        return;
    }


    // DEBUG
    // TODO: 1x2, 2x2, nxm
    lattice_height = 1;
    lattice_width = 2;

    b2Vec2 mp1_position = b2Vec2(position.x - 0.5f, position.y);
    b2Vec2 mp2_position = b2Vec2(position.x + 0.5f, position.y);
    b2Vec2 mp1_velocity = b2Vec2(0, 0);
    b2Vec2 mp2_velocity = b2Vec2(0, 0);

    MassPoint* mp1 = new MassPoint(mp1_position, mp1_velocity);
    MassPoint* mp2 = new MassPoint(mp2_position, mp2_velocity);

    b2BodyDef bd1;
    bd1.type = b2_dynamicBody;
    bd1.position = mp1->position;
    bd1.bullet = true;
    b2Body *body1 = m_world->CreateBody(&bd1);
    m_bomb = body1;
    m_bomb->SetLinearVelocity(mp1->velocity);

    b2FixtureDef fd1;
    b2CircleShape circle1;
    circle1.m_radius = circle1.m_radius = mp1->radius;
    fd1.shape = new b2CircleShape(circle1);
    fd1.density = mp1->mass;
    // TODO: change elasticity to our own ??
    fd1.restitution = elasticity; // for elastic collision

    m_bomb->CreateFixture(&fd1);


    b2BodyDef bd2;
    bd2.type = b2_dynamicBody;
    bd2.position = mp2->position;
    bd2.bullet = true;
    b2Body *body2 = m_world->CreateBody(&bd2);
    m_bomb = body2;
    m_bomb->SetLinearVelocity(mp2->velocity);

    b2FixtureDef fd2;
    b2CircleShape circle2;
    circle2.m_radius = circle2.m_radius = mp2->radius;
    fd2.shape = new b2CircleShape(circle2);
    fd2.density = mp2->mass;
    // TODO: change elasticity to our own ??
    fd2.restitution = elasticity; // for elastic collision

    m_bomb->CreateFixture(&fd2);



    b2DistanceJointDef jd;
    jd.bodyA = body1;
    jd.bodyB = body2;

    jd.localAnchorA = b2Vec2(0, 0);
    jd.localAnchorB = b2Vec2(0, 0);

    jd.minLength = 0;
    jd.maxLength = 2 * jd.length;

    jd.collideConnected = false; // Bodies connected by the joint should not collide

    jd.stiffness = 10.f;
    jd.damping = 0.5f;

    // Optionally set limits and motor features
    // jd.enableLimit = true/false;
    // jd.lowerAngle = ...; // example values in radians
    // jd.upperAngle = ...; // example values in radians
    // jd.enableMotor = true/false;
    // jd.motorSpeed = ...; // speed in radians per second
    // jd.maxMotorTorque = ...; // torque in N*m

    // Create the joint in the world
    b2Joint* joint = m_world->CreateJoint(&jd);
}

void Engine2::Push(b2World* world, b2Vec2 mousePosition) {
    if (pushEnabled) {
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
}

void Engine2::CompleteBombSpawn(const b2Vec2& p)
{
    const float multiplier = 30.0f;
    b2Vec2 vel = m_bombSpawnPoint - p;
    vel *= multiplier;
    LaunchBomb(m_bombSpawnPoint, vel);
    m_bombSpawning = false;
}

// Update GUI to add custom controls
void Engine2::UpdateUI() {
    ImGui::SetNextWindowPos(ImVec2(10.0f, 100.0f));
    ImGui::SetNextWindowSize(ImVec2(290.0f, 400.0f));
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::PushItemWidth(150); // Set the item width right after beginning the window

    if (ImGui::SliderFloat("Elasticity", &elasticity, 0.0f, 1.0f));

    // Buttons to spawn our basic primitives: Box, Circle, Triangle
    // Spawn by Shift + Left Mouse Click
    if (ImGui::Button("Spawn Box")) {
        shape = 'b';
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Spawns a box");
        ImGui::EndTooltip();
    }

    ImGui::Indent();
    if (ImGui::SliderFloat("Height", &length, 0.01f, 10.0f));
    if (ImGui::SliderFloat("Width", &width, 0.01f, 10.0f));
    if (ImGui::SliderFloat("Box Mass", &box_mass, 0.1f, 100.0f));
    ImGui::Unindent();

    if (ImGui::Button("Spawn Circle")) {
        shape = 'c';
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Spawns a circle");
        ImGui::EndTooltip();
    }

    ImGui::Indent();
    if (ImGui::SliderFloat("Radius", &radius, 0.01f, 10.0f));
    if (ImGui::SliderFloat("Circle Mass", &circle_mass, 0.1f, 100.0f));
    ImGui::Unindent();

    if (ImGui::Button("Spawn Triangle")) {
        shape = 't';
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Spawns a triangle");
        ImGui::EndTooltip();
    }

    ImGui::Indent();
    if (ImGui::SliderFloat("Side Length", &triangle_size, 0.01f, 10.0f));
    if (ImGui::SliderFloat("Triangle Mass", &triangle_mass, 0.1f, 100.0f));
    ImGui::Unindent();

    if (ImGui::Button(pushEnabled ? "Disable Push" : "Enable Push")) {
        pushEnabled = !pushEnabled;
        ImVec2 mousePos = ImGui::GetMousePos();
        b2Vec2 b2_mousePos = b2Vec2(mousePos.x, mousePos.y);
        Push(m_world, b2_mousePos);
    }

    if (ImGui::Button("Soft Body Rectangular")) {
        shape = 'l';      // Lattice
    }

    if (ImGui::SliderInt("Height", &lattice_height, 1, 20));
    if (ImGui::SliderInt("Width", &lattice_width, 1, 20));


    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Enabling this will make it so your mouse exerts a pushing force");
        ImGui::EndTooltip();
    }

    ImGui::PopItemWidth(); // Reset the item width after setting all sliders and buttons
    ImGui::End();
}

//  static Test *Create() { return new Engine2; }
//static int testIndex = RegisterTest("Benchmark", "Engine 2", Engine2::Create);

// Register the test with the test framework
static Test* Create() { return new Engine2; }
static int testIndex = RegisterTest("Benchmark", "Engine 2", Create);