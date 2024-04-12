#include "test.h"
#include "imgui/imgui.h"

class Engine2 : public Test {
public:
  const float box_minX = -30.0f;
  const float box_minY = -20.0f;
  const float box_maxX = 30.0f;
  const float box_maxY = 20.0f;
  const float k_restitution = 0.0f;
  
  Engine2() {
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
  
  void SpawnBox() {
    b2PolygonShape box;
    box.SetAsBox(1.0f, 1.0f, b2Vec2(RandomFloat(box_minX, box_maxX), RandomFloat(box_minY, box_maxY)), 0.0f);
    
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    b2Body *body = m_world->CreateBody(&bd);
    body->CreateFixture(&box, 0.1f);
  }
  
  void SpawnCircle() {
    b2CircleShape circle;
    circle.m_radius = 1.0f;
    circle.m_p.SetZero();
    
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position = b2Vec2(RandomFloat(box_minX, box_maxX), RandomFloat(box_minY, box_maxY));
    b2Body *body = m_world->CreateBody(&bd);
    body->CreateFixture(&circle, 0.1f);
  }
  
  void SpawnTriangle() {
    // TODO
  }
  
  // Update GUI to add custom controls
  void UpdateUI() override {
    ImGui::SetNextWindowPos(ImVec2(10.0f, 100.0f));
    ImGui::SetNextWindowSize(ImVec2(100.0f, 200.0f));
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    
    // START: Buttons to spawn our basic primitives: Box, Circle, Triangle
    if (ImGui::Button("Spawn Box"))
    {
      SpawnBox();
    }
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("Spawns a box - currently cannot change properties");
      ImGui::EndTooltip();
    }
    
    if (ImGui::Button("Spawn Circle"))
    {
      SpawnCircle();
    }
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("Spawns a circle - currently cannot change properties");
      ImGui::EndTooltip();
    }
    
    if (ImGui::Button("Spawn Triangle"))
    {
      SpawnTriangle();
    }
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("Spawns a triangle - currently cannot change properties");
      ImGui::EndTooltip();
    }
    // END
    
    

    ImGui::End();
  }

  static Test *Create() { return new Engine2; }
};

static int testIndex = RegisterTest("Benchmark", "Engine 2", Engine2::Create);
