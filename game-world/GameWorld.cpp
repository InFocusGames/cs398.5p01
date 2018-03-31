#ifdef _WIN32
#include <GL\glut.h>
#elif __APPLE__

#include <GLUT/glut.h>

#endif

#include <iostream>
#include "GameWorld.h"
#include "../map/AStar.h"
#include "../character/derived/Sneak.h"
#include "../character/derived/Thug.h"
#include "../character/derived/Runner.h"

GameWorld::GameWorld(int m_width, int m_height) : m_width(m_width),
                                                  m_height(m_height),
                                                  m_player(nullptr) {

    m_map = new Map(m_width, m_height);

    auto obstacles = m_map->getObstacles();

    for (auto it = obstacles.begin(); it != obstacles.end(); ++it) {
        m_obstacles.push_back(new Obstacle((*it)->getPos(), (*it)->getBoundingRadius()));
    }
}

void GameWorld::update(double timeElapsed) {
    // check if paused

    for (unsigned int i = 0; i < m_vehicles.size(); i++) {
        m_vehicles.at(i)->update(timeElapsed);
    }

    for (unsigned int i = 0; i < m_obstacles.size(); i++) {
        m_obstacles.at(i)->update(timeElapsed);
    }

    auto it = m_projectiles.begin();

    while (it != m_projectiles.end()) {
        if ((*it)->isDead()) {
            delete *it;
            it = m_projectiles.erase(it);
        } else {
            (*it)->update(timeElapsed);
            ++it;
        }
    }
}

/**
 * Is assumed to be in GLUT display function context
 */
void GameWorld::render() {

    m_map->render();

    for (unsigned int i = 0; i < m_vehicles.size(); i++) {
        m_vehicles.at(i)->render();
    }

    for (unsigned int i = 0; i < m_obstacles.size(); i++) {
//        m_obstacles.at(i)->render();
    }

    for (unsigned int i = 0; i < m_projectiles.size(); i++) {
        m_projectiles.at(i)->render();
    }
}

void GameWorld::setDimensions(int width, int height) {
    m_width  = width;
    m_height = height;
}

void GameWorld::clickHandler(int button, int state, int x, int y) {
    const Vector2D<double> &pos = Vector2D<double>(m_Boundaries.left + x, m_Boundaries.top - y);

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        auto goal  = m_map->getNodeByPosition(pos);
        auto start = m_map->getNodeByPosition(m_player->getPos());

        if (!goal->isTraversable()) return;

        Path* p = AStar::shortestPath(m_map->getGraph(), start, goal, m_player->getCostFunction());

        m_player->setPath(p);
        m_player->turnOnBehavior(SteeringBehaviors::fFollow_path);
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        auto goal = m_map->getNodeByPosition(pos);
        if (!goal->isTraversable()) return;

        m_player->setDestination(pos);
        m_player->turnOnBehavior(SteeringBehaviors::fSeek);
    }
}

void GameWorld::keyboardHandler(unsigned char key, int x, int y) {}

void GameWorld::setClippingBoundaries(int left, int right, int bottom, int top) {
    m_Boundaries = Boundaries(left, right, bottom, top);
}

Vector2D<double> GameWorld::randomTraversableLocation() const {
    MapNode* node;
    Vector2D<double> vec;

    do {
        int x = iRandomRange(0, m_width);
        int y = iRandomRange(0, m_height);
        vec  = Vector2D<double>(x, y);
        node = getNodeByPosition(vec);

    } while (!node->isTraversable());

    return vec;
}

void GameWorld::selectCharacter(GameWorld::characterClass aClass) {
    Character* sneak = new Sneak(this,
                                 randomTraversableLocation(),
                                 Vector2D<double>(1, 1),
                                 Vector2D<double>(0, 0),
                                 Vector2D<double>(0, 1),
                                 Vector2D<double>(1, 0));

    Character* thug = new Thug(this,
                               randomTraversableLocation(),
                               Vector2D<double>(1, 1),
                               Vector2D<double>(0, 0),
                               Vector2D<double>(0, 1),
                               Vector2D<double>(1, 0));

    Character* runner = new Runner(this,
                                   randomTraversableLocation(),
                                   Vector2D<double>(1, 1),
                                   Vector2D<double>(0, 0),
                                   Vector2D<double>(0, 1),
                                   Vector2D<double>(1, 0));
    sneak->addAntagonist(thug);
    sneak->addAntagonist(runner);

    thug->addAntagonist(sneak);
    thug->addAntagonist(runner);

    runner->addAntagonist(sneak);
    runner->addAntagonist(thug);

    m_vehicles.push_back(sneak);
    m_vehicles.push_back(thug);
    m_vehicles.push_back(runner);

    thug->turnOnDefaultBehavior();
    sneak->turnOnDefaultBehavior();
    runner->turnOnDefaultBehavior();

    // @todo turn on default behavior for classes not selected
    switch (aClass) {
        case runnerClass:
            m_player = runner;
            break;
        case thugClass:
            m_player = thug;
            break;
        case sneakClass:
            m_player = sneak;
            break;
    }
}