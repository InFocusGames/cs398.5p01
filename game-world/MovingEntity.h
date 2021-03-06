
#ifndef DZ04_MOVINGENTITY_H
#define DZ04_MOVINGENTITY_H


#include "BaseGameEntity.h"

class MovingEntity : public BaseGameEntity {
protected:
    Vector2D<double> m_velocity;
    Vector2D<double> m_heading;
    Vector2D<double> m_side;

    double m_mass;
    double m_maxSpeed;
    double m_maxForce;
    double m_maxTurnRate;
    double m_effectiveSpeed;


public:
    MovingEntity(const Vector2D<double> &pos,
                 double radius,
                 const Vector2D<double> &scale,
                 const Vector2D<double> &m_velocity,
                 const Vector2D<double> &m_heading,
                 const Vector2D<double> &m_side,
                 double m_mass,
                 double m_maxSpeed,
                 double m_maxForce,
                 double m_maxTurnRate) : BaseGameEntity(0, pos, radius),
                                         m_velocity(m_velocity),
                                         m_heading(m_heading),
                                         m_side(m_side),
                                         m_mass(m_mass),
                                         m_maxSpeed(m_maxSpeed),
                                         m_effectiveSpeed(m_maxSpeed),
                                         m_maxForce(m_maxForce),
                                         m_maxTurnRate(m_maxTurnRate) {}

    Vector2D<double> getVelocity() const { return m_velocity; }

    double getSpeed() const { return m_velocity.magnitude(); }
};


#endif //DZ04_MOVINGENTITY_H
