#include "physics.h"
#include "configuration.h"

physics::physics()
{
    //ctor
}

physics::~physics()
{
    //dtor
}


float physics::calculateGravity(particle p, particle p2){
    return conf::G*(p.getMass()*p2.getMass())/p.calcDistance(p2);
}


void physics::calculateForces(std::vector<particle> &particles, float time){

    for(int i = 0; i<particles.size(); i++){
        for(int j = i; j<particles.size(); j++){

            if(i != j){
                float force = calculateGravity(particles[i],particles[j]);
                float direction = particles[i].calcDirection(particles[j]);
                particles[i].updateVector(force, direction, time);
                particles[j].updateVector(force, direction+conf::PI, time);
            }
        }
    }
}

