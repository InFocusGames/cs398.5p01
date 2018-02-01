
#include "Vehicle.h"
#include "GameWorld.h"
#include "opengl_helpers.h"
#include "Matrix.h"
#include "geometry.h"

#ifdef _WIN32
#include <GL\glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#endif


Vehicle::~Vehicle() = default;

void Vehicle::update(double timeElapsed) {
    m_timeElapsed = timeElapsed;

    Vector2D<double> steeringForce = m_steeringBehavior->calculate();

    m_steeringForce = steeringForce;

    Vector2D<double> acceleration = steeringForce / m_mass;

    m_velocity += acceleration * timeElapsed;

    m_velocity.truncate(m_maxSpeed);

    m_pos += m_velocity * timeElapsed;

    m_pos.wrapAround(m_world->getWidth(), m_world->getHeight());

    if (m_velocity.squareMagnitude() > 0.00000001) {
        m_heading = m_velocity.getNormalized();

        m_side = m_heading.ortho();
    }
}

void Vehicle::render() {
    // @todo move to local space rather than global

    // wandering target
    glColor3f(0.0, 0.0, 1.0);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex2d(m_wanderTarget.x, m_wanderTarget.y);
    glEnd();

    glBegin(GL_LINES);
    glVertex2d(m_pos.x, m_pos.y);
    glVertex2d(m_steeringForce.x + m_pos.x, m_steeringForce.y + m_pos.y);
    glEnd();

    // heading vector RED
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2d(m_pos.x, m_pos.y);
    Vector2D<double> heading = m_heading * 100;
    heading += m_pos;
    glVertex2d(heading.x, heading.y);
    glEnd();

    // side vector GREEN
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex2d(m_pos.x, m_pos.y);
    Vector2D<double> side = m_side * 100;
    side += m_pos;
    glVertex2d(side.x, side.y);
    glEnd();

    // wandering circle
    glColor3f(1.0, 0.0, 1.0);
    auto dir = m_heading * m_steeringBehavior->m_wanderDistance;
    drawCircle(m_steeringBehavior->m_wanderRadius, dir + m_pos);

    glColor3f(0.4, 0.3, 0.3);
    glBegin(GL_TRIANGLES);
    Matrix2D mat;
    mat.rotate(m_heading, m_side);
    auto top = pointToWorldSpace(Vector2D<double>{-10.0, 15.0}, m_heading, m_side, m_pos);
    auto bottom = pointToWorldSpace(Vector2D<double>{-10.0, -15.0}, m_heading, m_side, m_pos);
    auto right = pointToWorldSpace(Vector2D<double>{40.0, 0}, m_heading, m_side, m_pos);

    glVertex2d(top.x, top.y);
    glVertex2d(bottom.x, bottom.y);
    glVertex2d(right.x, right.y);

    glEnd();
}

Vehicle::Vehicle(GameWorld *m_world,
                 const Vector2D<double> &pos,
                 const Vector2D<double> &scale,
                 const Vector2D<double> &m_velocity,
                 const Vector2D<double> &m_heading,
                 const Vector2D<double> &m_side,
                 double m_mass,
                 double m_maxSpeed,
                 double m_maxForce,
                 double m_maxTurnRate) : MovingEntity(pos, scale,
                                                      m_velocity,
                                                      m_heading,
                                                      m_side,
                                                      m_mass,
                                                      m_maxSpeed,
                                                      m_maxForce,
                                                      m_maxTurnRate),
                                         m_world(m_world) {
    m_steeringBehavior = new SteeringBehaviors(this);
}
