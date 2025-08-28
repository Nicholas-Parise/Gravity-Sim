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

    if(speed < conf::MIN_SPEED){
        speed = conf::MIN_SPEED;
    }
    this->velocity.x = x;
    this->velocity.y = y;
}

void Particle::setAcceleration(float x, float y){
    this->temp_acceleration = {x,y};
    this->acceleration = {x,y};
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

    this->temp_acceleration += totalForce / mass;
}


void Particle::updateVelocity(float dt) {
    this->velocity += 0.5f * (this->acceleration + this->temp_acceleration) * dt;
    this->velocity -= normalize(this->velocity) * (conf::DRAG_COEFF * length(this->velocity) * dt); // add drag
    this->acceleration = this->temp_acceleration;
}


void Particle::move(float dt){

    this->position += this->velocity * dt + 0.5f * this->acceleration * dt * dt;

    position.x = fmod(position.x + conf::MAX_X, conf::MAX_X * 2);
    if (position.x < 0) position.x += conf::MAX_X * 2;
    position.x -= conf::MAX_X;

    position.y = fmod(position.y + conf::MAX_Y, conf::MAX_Y * 2);
    if (position.y < 0) position.y += conf::MAX_Y * 2;
    position.y -= conf::MAX_Y;
}

float Particle::getMass(){
    return this->mass;
}

sf::VertexArray Particle::generateQuad(){

    float half = this->width / 2.0f;
    float x = this->position.x - half;
    float y = this->position.y - half;

    sf::Vector2f topLeft(x,y);
    sf::Vector2f topRight(x + width, y);
    sf::Vector2f bottomLeft(x, y + width);
    sf::Vector2f bottomRight(x + width, y + width);

    sf::VertexArray quad(sf::PrimitiveType::Triangles, 6);

    quad[0].position = topLeft;
    quad[0].texCoords = sf::Vector2f(0.0f, 0.0f);
    quad[1].position = bottomLeft;
    quad[1].texCoords = sf::Vector2f(0.0f, 1.0f);
    quad[2].position = topRight;
    quad[2].texCoords = sf::Vector2f(1.0f, 0.0f);

    quad[3].position = bottomLeft;
    quad[3].texCoords = sf::Vector2f(0.0f, 1.0f);
    quad[4].position = bottomRight;
    quad[4].texCoords = sf::Vector2f(1.0f, 1.0f);
    quad[5].position = topRight;
    quad[5].texCoords = sf::Vector2f(1.0f, 0.0f);


    sf::Color topColor    = (this->velocity.y < 0) ? linearInterpolation(-this->velocity.y, sf::Color::Red) : sf::Color::White;
    sf::Color bottomColor = (this->velocity.y > 0) ? linearInterpolation(this->velocity.y, sf::Color::Red) : sf::Color::White;
    sf::Color rightColor  = (this->velocity.x > 0) ? linearInterpolation(this->velocity.x, sf::Color::Blue) : sf::Color::White;
    sf::Color leftColor   = (this->velocity.x < 0) ? linearInterpolation(-this->velocity.x, sf::Color::Blue) : sf::Color::White;

    quad[0].color = blendColors(leftColor, topColor);
    quad[1].color = blendColors(leftColor, bottomColor);
    quad[2].color = blendColors(rightColor, topColor);

    quad[3].color = blendColors(leftColor, bottomColor);
    quad[4].color = blendColors(rightColor, bottomColor);
    quad[5].color = blendColors(rightColor, topColor);
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

