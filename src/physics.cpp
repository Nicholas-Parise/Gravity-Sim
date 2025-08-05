#include "physics.h"
#include "configuration.h"
#include <cmath>

physics::physics()
{
    //ctor
}

physics::~physics()
{
    //dtor
}


float physics::calculateGravity(particle p, particle p2){
    return conf::G*(p.getMass()*p2.getMass())/ pow(p.calcDistance(p2),2);
}


void physics::calculateForces(std::vector<particle> &particles, float time){

    float dt = time * conf::timeScale;

    // move and reset temp acceleration
    for(int i = 0; i<conf::particles; i++){
        particles[i].move(dt);
        particles[i].resetAcceleration();
    }

    // compute all forces
    for(int i = 0; i<conf::particles; i++){
        for(int j = i+1; j<conf::particles; j++){

            float force = calculateGravity(particles[i],particles[j]);
            float direction = particles[i].calcDirection(particles[j]);
            particles[i].addAcceleration(force, direction);
            particles[j].addAcceleration(force, direction+conf::PI); // inverse direction
        }
    }

    // update all velocities
    for(int i = 0; i<conf::particles; i++){
        particles[i].updateVelocity(dt);
    }

}


