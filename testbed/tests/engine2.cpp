#include "test.h"
#include "engine2.h"
#include "imgui/imgui.h"
#include <stdlib.h>
#include <stdio.h>

Engine2::Engine2() {
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
  
void Engine2::SpawnBox(const b2Vec2& p) {
  b2PolygonShape box;
  box.SetAsBox(1.0f, 1.0f, p, 0.0f);
  
  b2BodyDef bd;
  bd.type = b2_dynamicBody;
  b2Body *body = m_world->CreateBody(&bd);
  body->CreateFixture(&box, 0.1f);
}

void Engine2::SpawnCircle(const b2Vec2& p) {
  b2CircleShape circle;
  circle.m_radius = 1.0f;
  circle.m_p.SetZero();
  
  b2BodyDef bd;
  bd.type = b2_dynamicBody;
  bd.position = p;
  b2Body *body = m_world->CreateBody(&bd);
  body->CreateFixture(&circle, 0.1f);
}
  
void Engine2::SpawnTriangle() {
  // TODO
}

void Engine2::ShiftMouseDown(const b2Vec2& p) {
  m_mouseWorld = p;
  
  if (m_mouseJoint != NULL)
  {
    return;
  }
    
//  SpawnBomb(p);
  if (!box_or_circle) {
    SpawnBox(p);
  } else {
    SpawnCircle(p);
  }
  
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
  
  // Update GUI to add custom controls
void Engine2::UpdateUI() {
  ImGui::SetNextWindowPos(ImVec2(10.0f, 100.0f));
  ImGui::SetNextWindowSize(ImVec2(100.0f, 200.0f));
  ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
  
  // Buttons to spawn our basic primitives: Box, Circle, Triangle
  // Spawn by Shift + Left Mouse Click
  if (ImGui::Button("Spawn Box"))
  {
    if (box_or_circle) {
      box_or_circle = !box_or_circle;
    }
//    SpawnBox();
  }
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::Text("Spawns a box - currently cannot change properties");
    ImGui::EndTooltip();
  }
  
  if (ImGui::Button("Spawn Circle"))
  {
    if (!box_or_circle) {
      box_or_circle = !box_or_circle;
    }
//    SpawnCircle();
  }
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::Text("Spawns a circle - currently cannot change properties");
    ImGui::EndTooltip();
  }
  
  if (ImGui::Button("Spawn Triangle"))
  {
//    SpawnTriangle();
  }
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::Text("Spawns a triangle - currently cannot change properties");
    ImGui::EndTooltip();
  }
  
  // Push
  if (ImGui::Button(pushEnabled ? "Disable Push" : "Enable Push")) {
    pushEnabled = !pushEnabled;
    ImVec2 mousePos = ImGui::GetMousePos();
    b2Vec2 b2_mousePos = b2Vec2(mousePos.x, mousePos.y);
    Push(m_world, b2_mousePos);
  }
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::Text("Enabling this will make it so your mouse exerts a pushing force");
    ImGui::EndTooltip();
  }
  
  // Launch bomb
  if (ImGui::Button("Launch bomb")) {
    
  }
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::Text("To launch a bomb, press space on your keyboard. This will launch a 'bomb' (circle) to where your cursor is and this circle will persist in the world");
    ImGui::EndTooltip();
  }
  
  ImGui::End();
}

//  static Test *Create() { return new Engine2; }
//static int testIndex = RegisterTest("Benchmark", "Engine 2", Engine2::Create);

// Register the test with the test framework
static Test* Create() { return new Engine2; }
static int testIndex = RegisterTest("Benchmark", "Engine 2", Create);
