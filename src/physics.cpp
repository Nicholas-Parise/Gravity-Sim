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

    float dt = time * conf::timeScale;

    for(int i = 0; i<conf::particles; i++){
        for(int j = i; j<conf::particles; j++){

            if(i != j){
                float force = calculateGravity(particles[i],particles[j]);
                float direction = particles[i].calcDirection(particles[j]);
                particles[i].updateVector(force, direction, dt);
                particles[j].updateVector(force, direction+conf::PI, dt);
            }
        }
    }
}

