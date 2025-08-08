#include "particle.h"
#include "configuration.h"
#include <cmath>
#include <iostream>

Particle::Particle(float x, float y)
{
    this->position = {x,y};
    this->density = 1.6; // density of rock 1600 kg/m^3
    setMass(1.0f);
}

Particle::~Particle()
{
    //dtor
}

void Particle::setPosition(float x, float y){
    this->position = {x,y};
}

void Particle::setMass(float mass){

    if(mass < 0.1){
        mass = 0.1;
    }
    this->mass = mass*10.0;
    this->width = std::max(std::cbrt(density*mass),1.0f); // cube root to get width of volume of cube
}



void Particle::resetAcceleration(){
    this->temp_acceleration = {0,0};
}


void Particle::setspeed(float x, float y){

    float speed = sqrt(x * x + y * y);

    if(speed < conf::minSpeed){
        speed = conf::minSpeed;
    }
    this->velocity.x = x;
    this->velocity.y = y;
}


float Particle::calcDistance(const Particle &p){

    float xComp = this->position.x - p.position.x;
    float yComp = this->position.y - p.position.y;

    return sqrt(xComp*xComp + yComp*yComp);
}

float Particle::calcDirection(const Particle &p){
    return atan2(p.position.y - this->position.y, p.position.x - this->position.x);
}

void Particle::addAcceleration(float force, float direction) {
    if (!std::isfinite(force) || !std::isfinite(direction)) return;

    float acc = force / this->mass;
    this->temp_acceleration += {
        acc * cos(direction),
        acc * sin(direction)
    };
}


void Particle::addAcceleration(sf::Vector2<float> totalForce) {
    if (!std::isfinite(totalForce.x) || !std::isfinite(totalForce.y)) return;

    this->temp_acceleration = totalForce / mass;
}



void Particle::updateVelocity(float dt) {
    this->velocity += 0.5f * (this->acceleration + this->temp_acceleration) * dt;
    this->velocity -= normalize(this->velocity) * (conf::dragCoeff * length(this->velocity) * dt); // add drag
    this->acceleration = this->temp_acceleration;
}


void Particle::move(float dt){

    this->position += this->velocity * dt + 0.5f * this->acceleration * dt * dt;
/*
    if(this->position.x > conf::maxX){
        this->position.x = conf::maxX;
        this->velocity.x = conf::minSpeed * -1;
    }
    if(this->position.x < conf::maxX * -1){
        this->position.x = conf::maxX * -1;
        this->velocity.x = conf::minSpeed;
    }

    if(this->position.y > conf::maxY){
        this->position.y = conf::maxY;
        this->velocity.y = conf::minSpeed * -1;
    }
    if(this->position.y < conf::maxY * -1){
        this->position.y = conf::maxY * -1;
        this->velocity.y = conf::minSpeed;
    }
*/


    position.x = fmod(position.x + conf::maxX, conf::maxX * 2);
    if (position.x < 0) position.x += conf::maxX * 2;
    position.x -= conf::maxX;

    position.y = fmod(position.y + conf::maxY, conf::maxY * 2);
    if (position.y < 0) position.y += conf::maxY * 2;
    position.y -= conf::maxY;

}

float Particle::getMass(){
    return this->mass;
}

sf::VertexArray Particle::generateQuad(){

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

    sf::Color topColor    = (this->velocity.y < 0) ? linearInterpolation(-this->velocity.y, sf::Color::Red) : sf::Color::White;
    sf::Color bottomColor = (this->velocity.y > 0) ? linearInterpolation(this->velocity.y, sf::Color::Red) : sf::Color::White;
    sf::Color rightColor  = (this->velocity.x > 0) ? linearInterpolation(this->velocity.x, sf::Color::Blue) : sf::Color::White;
    sf::Color leftColor   = (this->velocity.x < 0) ? linearInterpolation(-this->velocity.x, sf::Color::Blue) : sf::Color::White;

    quad[0].color = blendColors(leftColor, topColor);
    quad[1].color = blendColors(rightColor, topColor);
    quad[2].color = blendColors(rightColor, bottomColor);
    quad[3].color = blendColors(rightColor, bottomColor);
    quad[4].color = blendColors(leftColor, bottomColor);
    quad[5].color = blendColors(leftColor, topColor);
    return quad;
}


float Particle::length(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

sf::Vector2f Particle::normalize(const sf::Vector2f& v) {
    float len = length(v);
    if (len == 0) return {0.f, 0.f};
    return v / len;
}



sf::Color Particle::linearInterpolation(float speed, sf::Color color2) {

    sf::Color color1 = sf::Color::White;

    float normalize = speed / 20.0f;
    if (normalize > 1.0f) normalize = 1.0f;

    return sf::Color(
        color1.r + normalize * (color2.r - color1.r),
        color1.g + normalize * (color2.g - color1.g),
        color1.b + normalize * (color2.b - color1.b)
    );
}

sf::Color Particle::blendColors(const sf::Color& a, const sf::Color& b) {
    return sf::Color(
        std::min(255, (a.r + b.r) / 2),
        std::min(255, (a.g + b.g) / 2),
        std::min(255, (a.b + b.b) / 2)
    );
}

