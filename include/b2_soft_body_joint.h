//
// Created by Wil on 4/28/2024.
// Note: stolen from `b2_distance_joint.h`
//

#ifndef BOX2D_B2_SOFT_BODY_JOINT_H
#define BOX2D_B2_SOFT_BODY_JOINT_H

#endif //BOX2D_B2_SOFT_BODY_JOINT_H

#include "box2d/b2_api.h"
#include "box2d/b2_joint.h"

struct B2_API b2SoftBodyJointDef : public b2JointDef
{
    b2SoftBodyJointDef()
    {
        type = e_distanceJoint;
        localAnchorA.Set(0.0f, 0.0f);
        localAnchorB.Set(0.0f, 0.0f);
        length = 1.0f;
        minLength = 0.0f;
        maxLength = FLT_MAX;
        stiffness = 0.0f;
        damping = 0.0f;
    }

    void Initialize(b2Body* bodyA, b2Body* bodyB,
                    const b2Vec2& anchorA, const b2Vec2& anchorB);

    b2Vec2 localAnchorA;
    b2Vec2 localAnchorB;
    float length;
    float minLength;
    float maxLength;
    float stiffness;
    float damping;

};

/// A distance joint constrains two points on two bodies to remain at a fixed
/// distance from each other. You can view this as a massless, rigid rod.
class B2_API b2DistanceJoint : public b2Joint {
public:

    b2Vec2 GetAnchorA() const override;

    b2Vec2 GetAnchorB() const override;

    /// Get the reaction force given the inverse time step.
    /// Unit is N.
    b2Vec2 GetReactionForce(float inv_dt) const override;

    /// Get the reaction torque given the inverse time step.
    /// Unit is N*m. This is always zero for a distance joint.
    float GetReactionTorque(float inv_dt) const override;

    /// The local anchor point relative to bodyA's origin.
    const b2Vec2 &GetLocalAnchorA() const { return m_localAnchorA; }

    /// The local anchor point relative to bodyB's origin.
    const b2Vec2 &GetLocalAnchorB() const { return m_localAnchorB; }

    /// Get the rest length
    float GetLength() const { return m_length; }

    /// Set the rest length
    /// @returns clamped rest length
    float SetLength(float length);

    /// Get the minimum length
    float GetMinLength() const { return m_minLength; }

    /// Set the minimum length
    /// @returns the clamped minimum length
    float SetMinLength(float minLength);

    /// Get the maximum length
    float GetMaxLength() const { return m_maxLength; }

    /// Set the maximum length
    /// @returns the clamped maximum length
    float SetMaxLength(float maxLength);

    /// Get the current length
    float GetCurrentLength() const;

    /// Set/get the linear stiffness in N/m
    void SetStiffness(float stiffness) { m_stiffness = stiffness; }

    float GetStiffness() const { return m_stiffness; }

    /// Set/get linear damping in N*s/m
    void SetDamping(float damping) { m_damping = damping; }

    float GetDamping() const { return m_damping; }

    /// Dump joint to dmLog
    void Dump() override;

    ///
    void Draw(b2Draw *draw) const override;

protected:

    friend class b2Joint;

    b2DistanceJoint(const b2DistanceJointDef *data);

    void InitVelocityConstraints(const b2SolverData &data) override;

    void SolveVelocityConstraints(const b2SolverData &data) override;

    bool SolvePositionConstraints(const b2SolverData &data) override;

    float m_stiffness;
    float m_damping;
    float m_bias;
    float m_length;
    float m_minLength;
    float m_maxLength;

    // Solver shared
    b2Vec2 m_localAnchorA;
    b2Vec2 m_localAnchorB;
    float m_gamma;
    float m_impulse;
    float m_lowerImpulse;
    float m_upperImpulse;

    // Solver temp
    int32 m_indexA;
    int32 m_indexB;
    b2Vec2 m_u;
    b2Vec2 m_rA;
    b2Vec2 m_rB;
    b2Vec2 m_localCenterA;
    b2Vec2 m_localCenterB;
    float m_currentLength;
    float m_invMassA;
    float m_invMassB;
    float m_invIA;
    float m_invIB;
    float m_softMass;
    float m_mass;
}