#include "Node.h"
#include "Configuration.h"
#include <cmath>

Node::~Node()
{
    delete NW;
    delete NE;
    delete SW;
    delete SE;
}

/**
* This recursive method will add particles to the quad tree
* if it encounters an empty leaf it adds to it
* if it encounters a full leaf it subdivides that leaf
* it then recursively adds the existing node to the proper quadrent
* finally it will add the provided leaf.
**/
void Node::insert(Particle* p) {

    if (this->region.size < conf::MIN_QUAD_SIZE) {
        // Too small to split further
        return;
    }

    if (!body && isLeaf()) {
        body = p;
        updateMassAndCenter(p);
    }else {

        if (isLeaf()) {
            // we have to Subdivide and reinsert existing body
            // since each node can only have one body.
            Particle* existing = body;
            body = nullptr;
            NW = new Node(region.NW());
            NE = new Node(region.NE());
            SW = new Node(region.SW());
            SE = new Node(region.SE());
            insert(existing);
        }

        // now we can finally add our particle p
        // we add it to the proper quad
         if (region.NW().contains(p->position)) NW->insert(p);
        else if (region.NE().contains(p->position)) NE->insert(p);
        else if (region.SW().contains(p->position)) SW->insert(p);
        else if (region.SE().contains(p->position)) SE->insert(p);

        updateMassAndCenter(p);
    }

}

void Node::updateMassAndCenter(Particle* p) {
    // this does both x & y together, they are exactly the same just different components
    sf::Vector2f newCenter = (centerOfMass * totalMass + p->position * p->mass) / (totalMass + p->mass);
    totalMass += p->mass;
    centerOfMass = newCenter;
}


void Node::computeForce(Particle* p, sf::Vector2f& totalForce) {
    if (!body && isLeaf()) return;

    if(p == body) return; // skip self

    float dx = centerOfMass.x - p->position.x;
    float dy = centerOfMass.y - p->position.y;
    float dist = std::sqrt(dx*dx + dy*dy) + conf::minPhysDistance;

    float s = region.size * 2;
    if (isLeaf() || (s / dist) < conf::theta) {
        float F = (conf::G * totalMass * p->mass) / (dist * dist);
        // normalize vector to be in correct direction
        totalForce += sf::Vector2f(F * dx / dist, F * dy / dist);
    }else {
        if (NW) NW->computeForce(p, totalForce);
        if (NE) NE->computeForce(p, totalForce);
        if (SW) SW->computeForce(p, totalForce);
        if (SE) SE->computeForce(p, totalForce);
    }
}


