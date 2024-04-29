//
// Created by Wil on 4/28/2024.
//

#ifndef BOX2D_MASSPOINTLATTICE_H
#define BOX2D_MASSPOINTLATTICE_H

#endif //BOX2D_MASSPOINTLATTICE_H

#include "box2d/b2_api.h"
#include "box2d/b2_math.h"
#include "box2d/b2_shape.h"
#include "box2d/box2d.h"
#include "imgui/imgui.h"
#include "test.h"

class MassPoint : public Test {
public:
    b2Vec2 position;
    b2Vec2 velocity;
    b2Vec2 force;
    // Please toggle below
    const float mass = 1.f;  // TODO: is mass the same as density?
    const float radius = 0.2f;
    // idk what this does
    const float k_restitution = 1.f;


    b2Vec2 GetAcceleration() {
        /// F = ma

        if (force == b2Vec2(0, 0) || mass == 0.f) {
            return b2Vec2(0, 0);
        }
        return 1 / mass * force;
    };
    
    MassPoint();

    MassPoint(b2Vec2 posn, b2Vec2 vel) {
        position = posn;
        velocity = vel;
    }
    void SpawnPoint(const b2Vec2& posn);
    void MakeRectLattice(unsigned int length, unsigned int width);
};


