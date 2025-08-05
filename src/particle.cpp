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



void particle::resetAcceleration(){
    this->temp_acceleration = {0,0};
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

void particle::addAcceleration(float force, float direction) {
    if (!std::isfinite(force) || !std::isfinite(direction)) return;

    float acc = force / this->mass;
    this->temp_acceleration += {
        acc * cos(direction),
        acc * sin(direction)
    };
}

void particle::updateVelocity(float dt) {
    this->velocity += 0.5f * (this->acceleration + this->temp_acceleration) * dt;
    this->velocity -= normalize(this->velocity) * (conf::dragCoeff * length(this->velocity) * dt);
    this->acceleration = this->temp_acceleration;
}


void particle::move(float dt){

    this->position += this->velocity * dt + 0.5f * this->acceleration * dt * dt;

    if(this->position.x > conf::maxX){
        this->position.x = conf::maxX * -1;
    }
    if(this->position.x < conf::maxX * -1){
        this->position.x = conf::maxX;
    }

    if(this->position.y > conf::maxY){
        this->position.y = conf::maxY * -1;
    }
    if(this->position.y < conf::maxY * -1){
        this->position.y = conf::maxY;
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


float particle::length(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2f particle::normalize(const sf::Vector2f& v) {
    float len = length(v);
    if (len == 0) return {0.f, 0.f};
    return v / len;
}
