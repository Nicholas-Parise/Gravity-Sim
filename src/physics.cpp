#include "Physics.h"
#include "configuration.h"
#include <cmath>
#include "Node.h"
#include <iostream>

Physics::Physics()
{
    //ctor
}

Physics::~Physics()
{
    //dtor
}

void Physics::addtoGrid(Particle &p){
    int xIdx = static_cast<int>(p.position.x / conf::quadrentSize);
    int yIdx = static_cast<int>(p.position.y / conf::quadrentSize);
    spatialGrid[{xIdx, yIdx}].push_back(&p);
}

void Physics::resetGrid(){
    spatialGrid.clear();
}


float Physics::calculateGravity(Particle p, Particle p2){
    float r = std::max(p.calcDistance(p2), conf::minPhysDistance); // add min distance
    return conf::G*(p.getMass()*p2.getMass())/ pow(r,2);
}
/*
// a big_quad is made of 9 quads in a square shape
// computer large scale gravity calcualtions
for each big_quad bq
    for each big_quad bq2
            temp_particle_bq.mass = sum_Mass(bq)
            temp_particle_bq.position = average_position(bq)
            temp_particle_bq2.mass = sum_Mass(bq2)
            temp_particle_bq2.position = average_position(bq2)
            float force = calculateGravity(bq,bq2);
            float direction = bq.calcDirection(bq2);
            bq.addAcceleration(force, direction);
            bq2.addAcceleration(force, direction+conf::PI);

for each big_quad bq
    particles = bigGetSmall(bq)
    for each paricle in particles
        calculate_gravity(particle)
        add_acceleration(particle)
        add_acceleration(bq) // add computed acceleration from the large scope gravity
        add_direction(bq) // add computed direction from the large scope gravity
*/

void Physics::calculateForces(std::vector<Particle> &particles, float time){

    float dt = std::min(time * conf::timeScale, conf::maxDt);

    //resetGrid();

    Quad boundary = {0, 0, conf::maxX * 2};
    Node* root = new Node(boundary);

    // move and reset temp acceleration
    for(int i = 0; i<conf::particles; i++){
        particles[i].move(dt); // move particle
        //addtoGrid(particles[i]); // then add to grid at new position
        particles[i].resetAcceleration();

        if (boundary.contains(particles[i].position)){
            root->insert(&particles[i]);
        }else{
            std::cout<<"didn't make the cut: x:"<<particles[i].position.x<<" y:"<<particles[i].position.y<<std::endl;
        }

    }
/*

    for (auto& p : particles) {
        if (boundary.contains(p.position)){
            root->insert(&p);
        }else{
         std::cout<<"didn't make the cut: x:"<<p.position.x<<" y:"<<p.position.y<<std::endl;
        }
    }

*/


    // Compute forces
    for (auto& p : particles) {
        sf::Vector2f totalForce = {0.f, 0.f};
        root->computeForce(&p, totalForce);
        p.addAcceleration(totalForce);
        if(totalForce.x == 0.0 || totalForce.y == 0.0){
         std::cout<<"didn't move: x:"<<p.position.x<<" y:"<<p.position.y<<std::endl;
        }
    }




/*
    // compute all forces
    for(int i = 0; i<conf::particles; i++){
        for(int j = i+1; j<conf::particles; j++){

            float force = calculateGravity(particles[i],particles[j]);
            float direction = particles[i].calcDirection(particles[j]);
            particles[i].addAcceleration(force, direction);
            particles[j].addAcceleration(force, direction+conf::PI); // inverse direction
        }
    }
*/

    // update all velocities
    for(int i = 0; i<conf::particles; i++){
        particles[i].updateVelocity(dt);
    }

    delete root;

}


