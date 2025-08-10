#ifndef NODE_H
#define NODE_H
#include <SFML/System.hpp>
#include "Particle.h"

struct Quad {
    float x, y, size;

    bool contains(const sf::Vector2f& point) const {
        return point.x >= x - size && point.x <= x + size &&
               point.y >= y - size && point.y <= y + size;
    }

    Quad NW() const { return {x - size / 2.0f, y - size / 2.0f, size / 2.0f}; }
    Quad NE() const { return {x + size / 2.0f, y - size / 2.0f, size / 2.0f}; }
    Quad SW() const { return {x - size / 2.0f, y + size / 2.0f, size / 2.0f}; }
    Quad SE() const { return {x + size / 2.0f, y + size / 2.0f, size / 2.0f}; }
};


class Node
{
    public:
        Node(const Quad& r) : region(r) {}
        virtual ~Node();

        Quad region;
        Particle* body = nullptr;

        Node* NW = nullptr;
        Node* NE = nullptr;
        Node* SW = nullptr;
        Node* SE = nullptr;

        float totalMass = 0;
        sf::Vector2f centerOfMass = {0.f, 0.f};

        inline bool isLeaf() const {
            return !NW && !NE && !SW && !SE;
        }

        void insert(Particle* p);
        void updateMassAndCenter(Particle* p);
        void computeForce(Particle* p, sf::Vector2f& totalForce);

    protected:

    private:
};

#endif // NODE_H
