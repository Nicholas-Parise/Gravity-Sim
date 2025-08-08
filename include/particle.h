#ifndef PARTICLE_H
#define PARTICLE_H
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>


class Particle
{
    public:
        Particle();
        Particle(float x, float y);
        virtual ~Particle();

        sf::Vector2f normalize(const sf::Vector2f& v);
        float length(const sf::Vector2f& v);

        float calcDistance(Particle p);
        float calcDirection(Particle p);
        void updateVelocity(float dt);
        void addAcceleration(float force, float direction);

        void move(float time);
        sf::VertexArray generateQuad();

        void setPosition(float x, float y);
        float getMass();
        void setMass(float mass);
        void setspeed(float x, float y);

        void addAcceleration(sf::Vector2<float> totalForce);

        sf::Color linearInterpolation(float speed, sf::Color color2);

        void resetAcceleration();

        float mass;
        float density;
        float width;

        float direction; // in radians
        sf::Vector2<float> velocity;
        sf::Vector2<float> acceleration;

        sf::Vector2<float> position;
        sf::Vector2<float> temp_position;

    protected:

    private:
        // used to sum up all the acceleration so we don't over write anything
        sf::Vector2<float> temp_acceleration;
};

#endif // PARTICLE_H
