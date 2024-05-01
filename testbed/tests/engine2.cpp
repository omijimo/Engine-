#include "test.h"
#include "engine2.h"
#include "imgui/imgui.h"
#include <ratio>
#include <stdlib.h>
#include "MassPoint.h"
#include <stdio.h>
#include <iostream>

using namespace std;

// for setting and resetting the ground based on the dimensions
b2Body* Engine2::UpdateGround() {
  float box_minX = originX - arena_width/2.0;
  float box_minY = originY - arena_height/2.0;
  float box_maxX = originX + arena_width/2.0;
  float box_maxY = originY + arena_height/2.0;

  if (hasGround) {
    for (b2Fixture* f: fixtureList) {
      currGround->DestroyFixture(f);
    }
  }
  else {
    hasGround = true;
  }

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
    fixtureList[0] = ground->CreateFixture(&sd);
    
    // Right vertical
    shape.SetTwoSided(b2Vec2(box_maxX, box_minY), b2Vec2(box_maxX, box_maxY));
    fixtureList[1] = ground->CreateFixture(&sd);
    
    // Top horizontal
    shape.SetTwoSided(b2Vec2(box_minX, box_maxY), b2Vec2(box_maxX, box_maxY));
    fixtureList[2] = ground->CreateFixture(&sd);
    
    // Bottom horizontal
    shape.SetTwoSided(b2Vec2(box_minX, box_minY), b2Vec2(box_maxX, box_minY));
    fixtureList[3] = ground->CreateFixture(&sd);
  }
  currGround = ground;
  return ground;
}



Engine2::Engine2() {

  ResetGravity();

  // the 'box' in which our objects exist/should not have anything outside it
  currGround = UpdateGround();

  
}

void Engine2::ResetGravity() {
  m_world->SetGravity(b2Vec2(gravityX, gravityY));

}

void Engine2::SetGround() {

    UpdateGround();
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
    const float hs = triangle_size;
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
    /** Creates an n x m lattice as a soft body given `lattice_height` and `width` */
    if (lattice_height < 1 || lattice_width < 1) {      // Edge case
        cout << "Invalid Dimensions. " << lattice_height << " x " << lattice_width << " is not a valid rectangular soft body lattice.";
        return;
    }

    std::vector<b2Body*> bodies;

    cout << "Computing Lattice Geometry..." << endl;
    for (int n = 0; n < lattice_height; n++) {
        for (int m = 0; m < lattice_width; m++) {
            // Defines the geometry of our joint structure
            b2Vec2 mp_position = position
                    - 1.f/2 * joint_length * b2Vec2((float) lattice_width, (float) lattice_height)     // first point
                    + joint_length * b2Vec2((float) m, (float) n);
            b2Vec2 mp_velocity = b2Vec2(0, 0);

            MassPoint* mp = new MassPoint(mp_position, mp_velocity);

            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position = mp->position;
            bd.bullet = true;
            b2Body *body = m_world->CreateBody(&bd);
            m_bomb = body;
            m_bomb->SetLinearVelocity(mp->velocity);

            b2FixtureDef fd;
            b2CircleShape circle;
            circle.m_radius = circle.m_radius = mp->radius;
            fd.shape = new b2CircleShape(circle);
            fd.density = mp->mass;
            // TODO: change elasticity to our own ??
            fd.restitution = elasticity; // for elastic collision

            m_bomb->CreateFixture(&fd);
            bodies.push_back(body);
        }
    }
//    cout << "Lattice Geometry Computed!" << endl;
//    cout << "Lattice size: " << bodies.size() << endl;
//    cout << "Calculating Lattice Topology..." << endl;

    // Defines the topology of our joint structure
    for (int n = 0; n < lattice_height; n++) {
        for (int m = 0; m < lattice_width; m++) {
//            cout << "n, m = " << n << ", " << m << endl;
            // Each RigidBody needs to check if it needs down-left, down, down-right, or right joints.
            // The other ones should already be there

//            cout << "Going into if clauses ..." << endl;
            // Check down-left
            if (n < lattice_height - 1 && m > 0) {
//                cout << "Down left" << endl;
//                std::cout << "Edge (" << n << ", " << m << ") - (" << n + 1 << ", " << m - 1 << ")" << endl;
                CreateJoint(bodies[n * lattice_width + m], bodies[(n + 1) * lattice_width + (m - 1)]);
            }
            // Check down
            if (n < lattice_height - 1) {
//                cout << "Down" << endl;
//                std::cout << "Edge (" << n << ", " << m << ") - (" << n + 1 << ", " << m << ")" << endl;
                CreateJoint(bodies[n * lattice_width + m], bodies[(n + 1) * lattice_width + m]);
            }
            // Check down-right
            if (n < lattice_height - 1 && m < lattice_width - 1) {
//                cout << "Down right" << endl;
//                std::cout << "Edge (" << n << ", " << m << ") - (" << n + 1 << ", " << m + 1 << ")" << endl;
                CreateJoint(bodies[n * lattice_width + m], bodies[(n + 1) * lattice_width + (m + 1)]);
            }
            // Check right
            if (m < lattice_width - 1) {
//                cout << "Right" << endl;
//                std::cout << "Edge (" << n << ", " << m << ") - (" << n << ", " << m + 1 << ")" << endl;
                CreateJoint(bodies[n * lattice_width + m], bodies[(n * lattice_width + m + 1)]);
            }
        }
    }

//    cout << "Lattice Topology Calculated!" << endl;
}

void Engine2::CreateJoint(b2Body* body_a, b2Body* body_b) {
    b2DistanceJointDef jd;
    jd.bodyA = body_a;
    jd.bodyB = body_b;
    jd.localAnchorA = b2Vec2(0, 0);
    jd.localAnchorB = b2Vec2(0, 0);

    jd.minLength = 0;
   // jd.maxLength = 2 * jd.length;

    jd.collideConnected = false; // Bodies connected by the joint should not collide

    jd.stiffness = lattice_stiffness;
    jd.damping = lattice_damping;
    jd.length = joint_length;

    // Create the joint in the world
    m_world->CreateJoint(&jd);


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
    ImGui::SetNextWindowSize(ImVec2(295.0f, 580.0f));
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


  if (ImGui::Button("Set Gravity")) {
  ResetGravity();
  }


  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::Text("Sets the gravity to the given values");
    ImGui::EndTooltip();
  }
  
  ImGui::Indent();
  if (ImGui::SliderFloat("Gravity X", &gravityX, -100.0f, 100.0f));
  if (ImGui::SliderFloat("Gravity Y", &gravityY, -100.0f, 100.0f));
  ImGui::Unindent();

  if (ImGui::Button("Soft Body Rectangular")) {
        shape = 'l';      // Lattice
    }


    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Creates a soft body object");
        ImGui::EndTooltip();
    }

    if (ImGui::SliderInt("Lattice Height", &lattice_height, 1, 20));
    if (ImGui::SliderInt("Lattice Width", &lattice_width, 1, 20));
    if (ImGui::SliderFloat("Damping", &lattice_stiffness, 1.f, 200.f));
    if (ImGui::SliderFloat("Stiffness", &lattice_damping, 0.01f, 20.f));




  if (ImGui::Button("Reset Arena")) {
    SetGround();
  }
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::Text("Resets the arena with the new given dimensions");
    ImGui::EndTooltip();
  }
  
  ImGui::Indent();
  if (ImGui::SliderFloat("Arena Width", &arena_width, 1.0f, 250.0f));
  if (ImGui::SliderFloat("Arena Height", &arena_height, 1.0f, 250.0f));
  ImGui::Unindent();




    ImGui::PopItemWidth(); // Reset the item width after setting all sliders and buttons
    ImGui::End();
}

//  static Test *Create() { return new Engine2; }
//static int testIndex = RegisterTest("Benchmark", "Engine 2", Engine2::Create);

// Register the test with the test framework
static Test* Create() { return new Engine2; }
static int testIndex = RegisterTest("Benchmark", "Engine 2", Create);