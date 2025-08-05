#include "particle.h"
#include "configuration.h"
#include <cmath>

particle::particle(float x, float y)
{
    this->position.x = x;
    this->position.y = y;

    this->mass = 1.0f;
    this->width = 10.0f;
}

particle::particle()
{
    this->position.x = 0;
    this->position.y = 0;
    this->mass = 1.0f;
    this->width = 1.0f;
}

particle::~particle()
{
    //dtor
}

void particle::setPosition(float x, float y){
    this->position.x = x;
    this->position.y = y;
}


void particle::setMass(float mass){

    if(mass < 0.1){
        mass = 0.1;
    }
    this->mass = mass;
    this->width = mass;
}

void particle::setspeed(float x, float y){

    float speed = sqrt(x * x + y * y);

    if(speed < conf::minSpeed){
        return;
    }
    this->velocity.x = x;
    this->velocity.y = y;
}


float particle::calcDistance(particle p){

    float xComp = this->position.x - p.position.x;
    float yComp = this->position.y - p.position.y;

    return sqrt(xComp*xComp + yComp*yComp);
}

float particle::calcDirection(particle p){
    return atan2(p.position.y - this->position.y, p.position.x - this->position.x);
}

void particle::updateVector(float force, float direction, float time){

if (!std::isfinite(force) || !std::isfinite(direction)) return;

    float acc = force / this->mass;
    float accX = acc * cos(direction);
    float accY = acc * sin(direction);

    float dt = time * conf::timeScale;

    if (!std::isfinite(accX) || !std::isfinite(accY) || !std::isfinite(dt)) return;

    setspeed(this->velocity.x + accX * dt, this->velocity.y + accY * dt);
}

void particle::move(float time){
    this->position.x += this->velocity.x * time * conf::timeScale;
    this->position.y += this->velocity.y * time * conf::timeScale;

    if(this->position.x > conf::maxX){
        this->position.x = conf::maxX;
    }
    if(this->position.x < conf::maxX * -1){
        this->position.x = conf::maxX * -1;
    }

    if(this->position.y > conf::maxY){
        this->position.y = conf::maxY;
    }
    if(this->position.y < conf::maxY * -1){
        this->position.y = conf::maxY * -1;
    }
}

float particle::getMass(){
    return this->mass;
}

sf::VertexArray particle::generateQuad(){

    float half = this->width / 2.0f;
    float x = this->position.x;
    float y = this->position.y;

    sf::VertexArray quad(sf::PrimitiveType::Triangles, 6);
    quad[0].position = sf::Vector2f(x - half, y - half);
    quad[1].position = sf::Vector2f(x + half, y - half);
    quad[2].position = sf::Vector2f(x + half, y + half);

    quad[3].position = sf::Vector2f(x + half, y + half);
    quad[4].position = sf::Vector2f(x - half, y + half);
    quad[5].position = sf::Vector2f(x - half, y - half);

    return quad;
}

