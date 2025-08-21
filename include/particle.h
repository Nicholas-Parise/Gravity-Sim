#ifndef PARTICLE_H
#define PARTICLE_H
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class Particle
{
    public:
        Particle() : Particle(0,0){};
        Particle(float x, float y);
        ~Particle();

        inline sf::Vector2f normalize(const sf::Vector2f& v);
        inline float length(const sf::Vector2f& v);

        float calcDistance(const Particle &p);
        float calcDirection(const Particle &p);
        void updateVelocity(float dt);
        void addAcceleration(float force, float direction);
        void addAcceleration(sf::Vector2<float> totalForce);
        void resetAcceleration();

        void move(float time);
        sf::VertexArray generateQuad();

        void setPosition(float x, float y);
        float getMass();
        void setMass(float mass);
        void setspeed(float x, float y);

        float mass;
        float density;
        float width;

        float direction; // in radians
        sf::Vector2<float> velocity;
        sf::Vector2<float> acceleration;
        sf::Vector2<float> position;

    protected:

    private:
        // used to sum up all the acceleration so we don't over write anything
        sf::Vector2<float> temp_acceleration;

        sf::Color linearInterpolation(float speed, sf::Color color2);
        sf::Color blendColors(const sf::Color& a, const sf::Color& b);
};

#endif // PARTICLE_H
