#include "Node.h"
#include "Configuration.h"
#include <cmath>
#include <iostream>

size_t forceCalls = 0;

Node::~Node()
{
 /*
    delete NW;
    delete NE;
    delete SW;
    delete SE;
    */
}

 void Node::reset(const Quad& r) {
    this->region = r;
    leaf = true;
    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;
    body = nullptr;
    totalMass = 0.f;
    centerOfMass = {0.f,0.f};
 }


/**
* This recursive method will add particles to the quad tree
* if it encounters an empty leaf it adds to it
* if it encounters a full leaf it subdivides that leaf
* it then recursively adds the existing node to the proper quadrent
* finally it will add the provided leaf.
**/
void Node::insert(Particle* p, NodePool& pool) {

    if (this->region.size < conf::MIN_QUAD_SIZE) {
        // Too small to split further
        //std::cout<<"stopped split"<<std::endl;
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
            NW = pool.allocate(region.NW());
            NE = pool.allocate(region.NE());
            SW = pool.allocate(region.SW());
            SE = pool.allocate(region.SE());
            insert(existing, pool);
        }

        // now we can finally add our particle p
        // we add it to the proper quad
         if (region.NW().contains(p->position)) NW->insert(p, pool);
        else if (region.NE().contains(p->position)) NE->insert(p, pool);
        else if (region.SW().contains(p->position)) SW->insert(p, pool);
        else if (region.SE().contains(p->position)) SE->insert(p, pool);

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
    float dist2 = dx*dx + dy*dy + conf::minPhysDistance*conf::minPhysDistance;
    //float dist = std::sqrt(dx*dx + dy*dy + conf::minPhysDistance*conf::minPhysDistance);
    //float invDist = 1.0f / dist;

    float s2 = region.size * region.size * 4.0f;
    if (isLeaf() || s2 < dist2 * conf::theta * conf::theta) {
        float invDist = 1.0f / std::sqrt(dist2); // save 3 divisions by pre computing this value
        float F = (conf::G * totalMass * p->mass) * (invDist * invDist * invDist);

        forceCalls++;

        // normalize vector to be in correct direction
        // we save one multiply by multiplying our force by invDist^3 instead of invDist^2
        totalForce.x += F * dx;
        totalForce.y += F * dy;
    }else {
        if (NW) NW->computeForce(p, totalForce);
        if (NE) NE->computeForce(p, totalForce);
        if (SW) SW->computeForce(p, totalForce);
        if (SE) SE->computeForce(p, totalForce);
    }
}
