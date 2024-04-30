#include "test.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include "imgui/imgui.h"
#include <iostream>
#include <vector>

#include "engine2.h"
#include "MassPoint.h"
#include "particleSystem.h"


using namespace std;

class QueryCallback : public b2QueryCallback
{
public:
  QueryCallback(const b2Vec2& point)
  {
    m_point = point;
    m_fixture = NULL;
  }
  
  bool ReportFixture(b2Fixture* fixture) override
  {
    b2Body* body = fixture->GetBody();
    if (body->GetType() == b2_dynamicBody)
    {
      bool inside = fixture->TestPoint(m_point);
      if (inside)
      {
        m_fixture = fixture;
        
        // We are done, terminate the query.
        return false;
      }
    }
    
    // Continue the query.
    return true;
  }
  
  b2Vec2 m_point;
  b2Fixture* m_fixture;
};


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

void Engine2::MouseDown(const b2Vec2& p)
{
    m_mouseWorld = p;
    
    if (m_mouseJoint != NULL)
    {
      return;
    }
    // Make a small box.
    b2AABB aabb;
    b2Vec2 d;
    d.Set(0.001f, 0.001f);
    aabb.lowerBound = p - d;
    aabb.upperBound = p + d;
    
    // Query the world for overlapping shapes.
    QueryCallback callback(p);
    m_world->QueryAABB(&callback, aabb);
    
    if (callback.m_fixture)
    {
      float frequencyHz = 5.0f;
      float dampingRatio = 0.7f;
      
      b2Body* body = callback.m_fixture->GetBody();
      b2MouseJointDef jd;
      jd.bodyA = m_groundBody;
      jd.bodyB = body;
      jd.target = p;
      jd.maxForce = 1000.0f * body->GetMass();
      b2LinearStiffness(jd.stiffness, jd.damping, frequencyHz, dampingRatio, jd.bodyA, jd.bodyB);
      
      m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&jd);
      body->SetAwake(true);
    }
    
    if (particleFlag) {
      // Create particles at the mouse position
      //    engine_particleSys.createParticle(engine_particleDef, p, m_world); --> im thinking make this function private
      engine_particleSys.createParticlesAroundMouse(engine_particleDef, p, 1.0f, 100, m_world);
    }
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

    if (ImGui::Button("Soft Body Rectangular")) {
        shape = 'l';      // Lattice
    }

    if (ImGui::SliderInt("Height", &lattice_height, 1, 20));
    if (ImGui::SliderInt("Width", &lattice_width, 1, 20));
    if (ImGui::SliderFloat("Damping", &lattice_stiffness, 1.f, 200.f));
    if (ImGui::SliderFloat("Stiffness", &lattice_damping, 0.01f, 20.f));


    if (ImGui::Button("Particle")) {
        particleFlag = !particleFlag;
        shape = 'p';
    }

    ImGui::PopItemWidth(); // Reset the item width after setting all sliders and buttons
    ImGui::End();
}

void Engine2::Step(Settings& settings)
{
  float timeStep = settings.m_hertz > 0.0f ? 1.0f / settings.m_hertz : float(0.0f);

  if (settings.m_pause)
  {
    if (settings.m_singleStep)
    {
      settings.m_singleStep = 0;
    }
    else
    {
      timeStep = 0.0f;
    }

    g_debugDraw.DrawString(5, m_textLine, "****PAUSED****");
    m_textLine += m_textIncrement;
  }

  uint32 flags = 0;
  flags += settings.m_drawShapes * b2Draw::e_shapeBit;
  flags += settings.m_drawJoints * b2Draw::e_jointBit;
  flags += settings.m_drawAABBs * b2Draw::e_aabbBit;
  flags += settings.m_drawCOMs * b2Draw::e_centerOfMassBit;
  g_debugDraw.SetFlags(flags);

  m_world->SetAllowSleeping(settings.m_enableSleep);
  m_world->SetWarmStarting(settings.m_enableWarmStarting);
  m_world->SetContinuousPhysics(settings.m_enableContinuous);
  m_world->SetSubStepping(settings.m_enableSubStepping);

  m_pointCount = 0;

  m_world->Step(timeStep, settings.m_velocityIterations, settings.m_positionIterations);

  m_world->DebugDraw();
    g_debugDraw.Flush();

  if (timeStep > 0.0f)
  {
    ++m_stepCount;
  }

  if (settings.m_drawStats)
  {
    int32 bodyCount = m_world->GetBodyCount();
    int32 contactCount = m_world->GetContactCount();
    int32 jointCount = m_world->GetJointCount();
    g_debugDraw.DrawString(5, m_textLine, "bodies/contacts/joints = %d/%d/%d", bodyCount, contactCount, jointCount);
    m_textLine += m_textIncrement;

    int32 proxyCount = m_world->GetProxyCount();
    int32 height = m_world->GetTreeHeight();
    int32 balance = m_world->GetTreeBalance();
    float quality = m_world->GetTreeQuality();
    g_debugDraw.DrawString(5, m_textLine, "proxies/height/balance/quality = %d/%d/%d/%g", proxyCount, height, balance, quality);
    m_textLine += m_textIncrement;
  }

  // Track maximum profile times
  {
    const b2Profile& p = m_world->GetProfile();
    m_maxProfile.step = b2Max(m_maxProfile.step, p.step);
    m_maxProfile.collide = b2Max(m_maxProfile.collide, p.collide);
    m_maxProfile.solve = b2Max(m_maxProfile.solve, p.solve);
    m_maxProfile.solveInit = b2Max(m_maxProfile.solveInit, p.solveInit);
    m_maxProfile.solveVelocity = b2Max(m_maxProfile.solveVelocity, p.solveVelocity);
    m_maxProfile.solvePosition = b2Max(m_maxProfile.solvePosition, p.solvePosition);
    m_maxProfile.solveTOI = b2Max(m_maxProfile.solveTOI, p.solveTOI);
    m_maxProfile.broadphase = b2Max(m_maxProfile.broadphase, p.broadphase);

    m_totalProfile.step += p.step;
    m_totalProfile.collide += p.collide;
    m_totalProfile.solve += p.solve;
    m_totalProfile.solveInit += p.solveInit;
    m_totalProfile.solveVelocity += p.solveVelocity;
    m_totalProfile.solvePosition += p.solvePosition;
    m_totalProfile.solveTOI += p.solveTOI;
    m_totalProfile.broadphase += p.broadphase;
  }

  if (settings.m_drawProfile)
  {
    const b2Profile& p = m_world->GetProfile();

    b2Profile aveProfile;
    memset(&aveProfile, 0, sizeof(b2Profile));
    if (m_stepCount > 0)
    {
      float scale = 1.0f / m_stepCount;
      aveProfile.step = scale * m_totalProfile.step;
      aveProfile.collide = scale * m_totalProfile.collide;
      aveProfile.solve = scale * m_totalProfile.solve;
      aveProfile.solveInit = scale * m_totalProfile.solveInit;
      aveProfile.solveVelocity = scale * m_totalProfile.solveVelocity;
      aveProfile.solvePosition = scale * m_totalProfile.solvePosition;
      aveProfile.solveTOI = scale * m_totalProfile.solveTOI;
      aveProfile.broadphase = scale * m_totalProfile.broadphase;
    }

    g_debugDraw.DrawString(5, m_textLine, "step [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.step, aveProfile.step, m_maxProfile.step);
    m_textLine += m_textIncrement;
    g_debugDraw.DrawString(5, m_textLine, "collide [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.collide, aveProfile.collide, m_maxProfile.collide);
    m_textLine += m_textIncrement;
    g_debugDraw.DrawString(5, m_textLine, "solve [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solve, aveProfile.solve, m_maxProfile.solve);
    m_textLine += m_textIncrement;
    g_debugDraw.DrawString(5, m_textLine, "solve init [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveInit, aveProfile.solveInit, m_maxProfile.solveInit);
    m_textLine += m_textIncrement;
    g_debugDraw.DrawString(5, m_textLine, "solve velocity [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveVelocity, aveProfile.solveVelocity, m_maxProfile.solveVelocity);
    m_textLine += m_textIncrement;
    g_debugDraw.DrawString(5, m_textLine, "solve position [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solvePosition, aveProfile.solvePosition, m_maxProfile.solvePosition);
    m_textLine += m_textIncrement;
    g_debugDraw.DrawString(5, m_textLine, "solveTOI [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveTOI, aveProfile.solveTOI, m_maxProfile.solveTOI);
    m_textLine += m_textIncrement;
    g_debugDraw.DrawString(5, m_textLine, "broad-phase [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.broadphase, aveProfile.broadphase, m_maxProfile.broadphase);
    m_textLine += m_textIncrement;
  }

  if (m_bombSpawning)
  {
    b2Color c;
    c.Set(0.0f, 0.0f, 1.0f);
    g_debugDraw.DrawPoint(m_bombSpawnPoint, 4.0f, c);

    c.Set(0.8f, 0.8f, 0.8f);
    g_debugDraw.DrawSegment(m_mouseWorld, m_bombSpawnPoint, c);
  }

  if (settings.m_drawContactPoints)
  {
    const float k_impulseScale = 0.1f;
    const float k_axisScale = 0.3f;

    for (int32 i = 0; i < m_pointCount; ++i)
    {
      ContactPoint* point = m_points + i;

      if (point->state == b2_addState)
      {
        // Add
        g_debugDraw.DrawPoint(point->position, 10.0f, b2Color(0.3f, 0.95f, 0.3f));
      }
      else if (point->state == b2_persistState)
      {
        // Persist
        g_debugDraw.DrawPoint(point->position, 5.0f, b2Color(0.3f, 0.3f, 0.95f));
      }

      if (settings.m_drawContactNormals == 1)
      {
        b2Vec2 p1 = point->position;
        b2Vec2 p2 = p1 + k_axisScale * point->normal;
        g_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.9f));
      }
      else if (settings.m_drawContactImpulse == 1)
      {
        b2Vec2 p1 = point->position;
        b2Vec2 p2 = p1 + k_impulseScale * point->normalImpulse * point->normal;
        g_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
      }

      if (settings.m_drawFrictionImpulse == 1)
      {
        b2Vec2 tangent = b2Cross(point->normal, 1.0f);
        b2Vec2 p1 = point->position;
        b2Vec2 p2 = p1 + k_impulseScale * point->tangentImpulse * tangent;
        g_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
      }
    }
  }
}

//  static Test *Create() { return new Engine2; }
//static int testIndex = RegisterTest("Benchmark", "Engine 2", Engine2::Create);

// Register the test with the test framework
static Test* Create() { return new Engine2; }
static int testIndex = RegisterTest("Benchmark", "Engine 2", Create);
