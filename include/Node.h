#ifndef NODE_H
#define NODE_H
#include <SFML/System.hpp>
#include "Particle.h"
#include <iostream>

class NodePool; // forward declaration

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
        Node() = default;
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
        bool leaf = true;

        inline bool isLeaf() const {
            return !NW && !NE && !SW && !SE;
        }

        void reset(const Quad& region);

        void insert(Particle* p, NodePool& pool);
        void updateMassAndCenter(Particle* p);
        void computeForce(Particle* p, sf::Vector2f& totalForce);
};


class NodePool
{
    public:
        NodePool(size_t capacity) {
            nodes.resize(capacity); // preallocate memory
            nextFree = 0;
        }

        Node* allocate(const Quad& region) {
        if (nextFree >= nodes.size()) {
            // Expand pool if needed
            //std::cout<<"size: "<<nodes.size()<<"next free: "<<nextFree<<std::endl;

            size_t oldSize = nodes.size();
            nodes.resize(oldSize * 2); // double the pool
            std::cout << "NodePool expanded from " << oldSize << " to " << nodes.size() << std::endl;
        }

        Node* n = &nodes[nextFree++];
        n->reset(region); // reset node
        return n;
    }

    // reset pool
    void clear() {
        nextFree = 0;
    }

    std::vector<Node> nodes;
        size_t nextFree = 0;

    private:

};

#endif // NODE_H
