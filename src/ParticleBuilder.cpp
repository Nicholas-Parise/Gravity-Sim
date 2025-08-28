#include "ParticleBuilder.h"
#include <cmath>

ParticleBuilder::ParticleBuilder() : particles(conf::particles)
{
    srand(time(0));
    // set the default
    setPositionUniform();
    setPositionRing(0.8);
    setMass(100,1000);
    //setVelocity(-300, 300);
    //setAcceleration(-300, 300);
    setOrbitCircle(0, 0, 2e12);
}

ParticleBuilder::~ParticleBuilder(){}

/**
* Set the position of particles to a uniform even random positon across the entire size
*/
ParticleBuilder& ParticleBuilder::setPositionUniform(){
    setPositionUniform(0.0); // no border
    return *this;
}

/**
* Set the position of particles to a uniform even random positon with a border
* 0 -> 1
* 0 no border
* 1 max border
*/
ParticleBuilder& ParticleBuilder::setPositionUniform(float border){

    if(border >1) border = 1.0f;
    if(border <0) border = 0.0f;

    float maxY = conf::maxY - conf::maxY * border;
    float maxX = conf::maxX - conf::maxX * border;

    for(int i = 0; i<conf::particles; i++){
        float x = (rand()%(int)(maxX+maxX+1))-(int)maxX;
        float y = (rand()%(int)(maxY+maxY+1))-(int)maxY;
        particles[i].setPosition(x, y);
    }
    return *this;
}

/**
* Set the position of particles to the circumference of a circle with radius as % of size
*/
ParticleBuilder& ParticleBuilder::setPositionRing(float radius){

    if(radius >1) radius = 1.0f;
    if(radius <0.1) radius = 0.1f;

    radius = radius * std::min(conf::maxX, conf::maxY);

    for(int i = 0; i<conf::particles; i++){
        float angle = (static_cast<double>(rand()) / RAND_MAX) * 2 * conf::PI;
        float r = 50 * sqrt(static_cast<float>(rand()) / RAND_MAX) + radius;
        float x = r * cos(angle);
        float y = r * sin(angle);
        particles[i].setPosition(x,y);
    }
    return *this;
}


/**
* Set the position of particles to a circle with radius as % of size
*/
ParticleBuilder& ParticleBuilder::setPositionCirlce(float radius){

    if(radius >1) radius = 1.0f;
    if(radius <0.1) radius = 0.1f;

    radius = radius * std::min(conf::maxX, conf::maxY);

    for(int i = 0; i<conf::particles; i++){
        float angle = (static_cast<double>(rand()) / RAND_MAX) * 2 * conf::PI;
        float r = radius * sqrt(static_cast<float>(rand()) / RAND_MAX);
        float x = r * cos(angle);
        float y = r * sin(angle);
        particles[i].setPosition(x,y);
    }
    return *this;
}




/**
* set all particles to same mass
*/
ParticleBuilder& ParticleBuilder::setMass(float mass){
    for(int i = 0; i<conf::particles; i++){
        particles[i].setMass(mass);
    }
    return *this;
}

/**
* set particles to random mass between min and max
*/
ParticleBuilder& ParticleBuilder::setMass(float min, float max){
    if (max < min) std::swap(min, max);
    for(int i = 0; i<conf::particles; i++){
        float mass = rand()%(int)(max-min+1) + min;
        particles[i].setMass(mass);
    }
    return *this;
}

/**
* set all particles to same velocity
*/
ParticleBuilder& ParticleBuilder::setVelocity(float velocity){
    for(int i = 0; i<conf::particles; i++){
        particles[i].setspeed(velocity, velocity);
    }
    return *this;
}

/**
* set particles to random velocity between min and max
*/
ParticleBuilder& ParticleBuilder::setVelocity(float min, float max){
    if (max < min) std::swap(min, max);
    for(int i = 0; i<conf::particles; i++){
        float x = rand()%(int)(max-min+1) + min;
        float y = rand()%(int)(max-min+1) + min;
        particles[i].setspeed(x, y);
    }
    return *this;
}

/**
* make particles go in a circle with given speed
*/
ParticleBuilder& ParticleBuilder::setVelocityCircle(float centerX, float centerY, float speed) {
    for (int i = 0; i < conf::particles; i++) {
        float dx = particles[i].position.x - centerX;
        float dy = particles[i].position.y - centerY;

        float len = std::sqrt(dx*dx + dy*dy);
        if (len == 0) continue; // skip center particle
        dx /= len;
        dy /= len;

        float vx = -dy * speed;
        float vy = dx * speed;

        particles[i].setspeed(vx, vy);
        particles[i].addAcceleration({vx, vy});
    }
    return *this;
}


/**
* Given mass of center orbit around
*/
ParticleBuilder& ParticleBuilder::setOrbitCircle(float centerX, float centerY, float mass) {
    for (int i = 0; i < conf::particles; i++) {
        float dx = particles[i].position.x - centerX;
        float dy = particles[i].position.y - centerY;

        float len = std::sqrt(dx*dx + dy*dy);
        if (len == 0) continue; // skip center particle
        dx /= len;
        dy /= len;

        float speed = std::sqrt(conf::G * mass / len);

        float vx = -dy * speed;
        float vy = dx * speed;

        particles[i].setspeed(vx, vy);
        particles[i].setAcceleration(vx, vy);
    }
    return *this;
}


/**
* set all particles to same Acceleration
*/
ParticleBuilder& ParticleBuilder::setAcceleration(float acc){
    for(int i = 0; i<conf::particles; i++){
        particles[i].setAcceleration(acc, acc);
    }
    return *this;
}

/**
* set particles to random Acceleration between min and max
*/
ParticleBuilder& ParticleBuilder::setAcceleration(float min, float max){
    if (max < min) std::swap(min, max);
    for(int i = 0; i<conf::particles; i++){
        float x = rand()%(int)(max-min+1) + min;
        float y = rand()%(int)(max-min+1) + min;
        particles[i].setAcceleration(x,y);
    }
    return *this;
}


std::vector<Particle>& ParticleBuilder::generate(){
    return particles;
}
