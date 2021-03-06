
#ifndef DZ04_VEHICLE_H
#define DZ04_VEHICLE_H

#include "../game-world/MovingEntity.h"
#include "SteeringBehaviors.h"
#include "../graphics/opengl_helpers.h"
#include "../map/GraphEdge.h"
#include "costFunctions.h"

class GameWorld;

class Attack;

class State;

class Character : public MovingEntity {
private:
    /**
     * Steering Behaviors
     */
    Character* m_leader;
    Character* m_target;
    Vector2D<double>        m_offset;
    std::vector<Character*> m_antagonists;
    Vector2D<double>        m_steeringForce;

protected:
    GameWorld* m_world;
    Vector2D<double> m_destination;

    double m_antagonistDetectionDistance;
    double m_distanceToAttackRanged;
    double m_distanceToAttackMelee;
    double m_detectionBoxLength;
    double m_trackingAdvantage;
    double m_timeLastAttacked;
    double m_attackTimeout;
    double m_attackSpeed;

    int m_health;

    bool m_dead;
    bool m_isPlayerControlled;

    /**
    * Rendering
    */
    void renderAids() const;

    mutable BITMAPINFO* m_info, * m_maskInfo;
    mutable unsigned char* m_pixels, * m_maskPixels;

    SteeringBehaviors* m_steeringBehavior;

public:
    virtual ~Character();

    Character(GameWorld* m_world,
              const Vector2D<double> &pos,
              const Vector2D<double> &scale,
              const Vector2D<double> &m_velocity,
              const Vector2D<double> &m_heading,
              const Vector2D<double> &m_side,
              double m_mass,
              double m_maxSpeed,
              double meleeAttackDistance,
              double rangedAttackDistance,
              double attackTimeout);


    /**
     * Game play
     */

    int getHealth() const { return m_health; }

    double getHealthRatio() const { return m_health / 100.; }

    bool isPlayerControlled() const { return m_isPlayerControlled; }

    Character* seekEnemies() const;

    virtual void attackRanged(Vector2D<double> target);

    virtual void attackMelee(Vector2D<double> target);

    virtual void takeDamage(double damage);

    bool closeEnoughToAttackMelee(Character* enemy) {
        return closeEnoughToAttackMelee(enemy->getPos());
    }

    bool closeEnoughToAttackMelee(Vector2D<double> pos) {
        return m_distanceToAttackMelee > 0 &&
               pos.squareDistanceTo(m_pos) <= m_distanceToAttackMelee * m_distanceToAttackMelee;
    }

    bool closeEnoughToAttackRanged(Character* enemy) {
        return closeEnoughToAttackRanged(enemy->getPos());
    }

    bool closeEnoughToAttackRanged(Vector2D<double> pos) {
        return m_distanceToAttackRanged > 0 &&
               pos.squareDistanceTo(m_pos) <= m_distanceToAttackRanged * m_distanceToAttackRanged;
    }

    bool closeEnoughToAttack(Character* enemy) {
        return closeEnoughToAttackRanged(enemy) || closeEnoughToAttackMelee(enemy);
    }

    bool isDead() const { return m_dead; }

    bool canDetect(Character* enemy);

    void setAutonomousTurning(bool turn) { m_isPlayerControlled = turn; }

    /**
     * Rendering
     */
    Color m_color;

    virtual void drawSprite(int x, int y) const = 0;

    /**
     * Steering Behaviors
     */
    Vector2D<double> wanderTarget;

    const Character* interposeTargetA;

    const Character* interposeTargetB;

    Vector2D<double> getHeading() const { return m_heading; }

    Vector2D<double> getSide() const { return m_side; }

    GameWorld* getWorld() const { return m_world; }

    double getDetectionBoxLength() const { return m_detectionBoxLength; }

    virtual const double calculateMaxSpeed() const { return m_effectiveSpeed; }

    void turnToFace(Vector2D<double> target);

    double getTrackingAdvantage() const { return m_trackingAdvantage; }

    std::vector<Character*> getAntagonists() const { return m_antagonists; }

    void addAntagonist(Character* a) { m_antagonists.push_back(a); }

    void setLeaderAndOffset(Character* l, Vector2D<double> v) {
        m_leader = l;
        m_offset = v;
    }

    Character* getLeader() const { return m_leader; }

    Vector2D<double> getOffset() const { return m_offset; }

    void setDestination(Vector2D<double> dest) { m_destination = dest; }

    Vector2D<double> getDestination() const { return m_destination; }

    void interposeVehicles(const Character* a, const Character* b) {
        interposeTargetA = a;
        interposeTargetB = b;
    }

    void setTarget(Character* t) { m_target = t; }

    Character* getTarget() const { return m_target; }

    void setPath(Path* path) { m_steeringBehavior->setPath(path); }

    virtual costFn getCostFunction() {
        return basicCost;
    }

    void turnOnBehavior(SteeringBehaviors::behaviorType behavior) {
        behavior == SteeringBehaviors::none ? m_steeringBehavior->turnAllOff() : m_steeringBehavior->turnOn(behavior);
    }

    void turnOffBehavior(SteeringBehaviors::behaviorType behavior) {
        m_steeringBehavior->turnOff(behavior);
    }

    virtual void turnOnDefaultBehavior() {
        m_steeringBehavior->turnOn(SteeringBehaviors::fAvoid_obs);
    }

    /**
     * State
     */
    State* currentState;

    void changeState(State* newState);

    /**
     * Events
     */

    void notify(Event e) override;

    /**
     * Game Loop
     */

    void update(double timeElapsed) override;

    void render() const override;
};


#endif //DZ04_VEHICLE_H
