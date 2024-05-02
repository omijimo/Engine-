#ifndef SPATIAL_HASH
#define SPATIAL_HASH

#include "box2d/box2d.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <stdlib.h>
#include <list>
#include <vector>

// class particleGrid {
// public:
//     particleGrid(float width, float height, float cellSize, float worldminX, float worldminY);
//     void addParticle(b2Body* particle);
//     void updateParticle(b2Body* particle, const b2Vec2& oldPosition);
//     std::vector<b2Body*> getNeighbors(b2Body* particle);
// private:
//     float worldminX;
//     float worldminY;
  
//     std::vector<std::list<b2Body*>> grid;
//     int gridWidth, gridHeight;
//     float cellSize;
//     int getGridIndex(const b2Vec2& position);
// };

// #endif

class spatialHash {
public:
    spatialHash(float cellSize);

    void insert(b2Body* body);

    void remove(b2Body* body, const b2Vec2& oldPosition);

    std::vector<b2Body*> query(const b2Vec2& position, float queryRadius);

private:
    std::unordered_map<int, std::vector<b2Body*>> table;
    float cellSize;

    int hashPosition(const b2Vec2& position);
};

#endif
