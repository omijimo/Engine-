#include "test.h"
#include "engine2.h"
#include "imgui/imgui.h"
#include <ratio>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;
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
  m_world->SetGravity(b2Vec2(0.0f, -10.0f));

  // the 'box' in which our objects exist/should not have anything outside it
  currGround = UpdateGround();

  
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
  b2BodyDef bd;
  bd.type = b2_dynamicBody;
  bd.position = position;
  bd.bullet = true;
  m_bomb = m_world->CreateBody(&bd);
  m_bomb->SetLinearVelocity(velocity);
  
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
    }

  b2FixtureDef fd;
  fd.shape = object;
  fd.density = 20.0f;
  fd.restitution = elasticity; // for elastic collision
  
  b2Vec2 minV = position - b2Vec2(0.3f,0.3f);
  b2Vec2 maxV = position + b2Vec2(0.3f,0.3f);
  
  b2AABB aabb;
  aabb.lowerBound = minV;
  aabb.upperBound = maxV;

  m_bomb->CreateFixture(&fd);
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
