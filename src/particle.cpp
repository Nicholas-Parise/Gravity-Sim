#include "particle.h"
#include "configuration.h"
#include <cmath>

particle::particle()
{
    this->width = 10.0f;
    this->velocity.y = 0.01f;
}

particle::~particle()
{
    //dtor
}

void particle::setPosition(float x, float y){
    this->position.x = x;
    this->position.y = y;
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

    float acc = force / this->mass;

    float accX = acc * cos(direction);
    float accY = acc * sin(direction);

    this->velocity.x = this->velocity.x + accX * time * conf::timeScale;
    this->velocity.y = this->velocity.y + accY * time * conf::timeScale;
}

void particle::move(float time){
    this->position.x += this->velocity.x * time * conf::timeScale;
    this->position.y += this->velocity.y * time * conf::timeScale;
}


sf::VertexArray particle::generateQuad(){

    float half = width/2;
    float halfY = this->position.y/2;

    sf::VertexArray quad(sf::PrimitiveType::LineStrip, 4);
    quad[0].position = sf::Vector2f(this->position.x-half, this->position.y-half);
    quad[1].position = sf::Vector2f(this->position.x+half, this->position.y-half);
    quad[2].position = sf::Vector2f(this->position.x+half, this->position.y+half);
    quad[3].position = sf::Vector2f(this->position.x-half, this->position.y+half);

    return quad;
}

