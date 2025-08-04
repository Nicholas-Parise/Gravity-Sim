#ifndef PARTICLE_H
#define PARTICLE_H
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class particle
{
    public:
        particle();
        virtual ~particle();
        float calcDistance(particle p);
        float calcDirection(particle p);
        void updateVector(float force, float direction, float time);
        void setPosition(float x, float y);
        void move(float time);
        sf::VertexArray generateQuad();

        float mass;
        float density;
        float width;

        float direction; // in radians
        sf::Vector2<float> velocity;

        sf::Vector2<float> position;
        sf::Vector2<float> temp_position;

    protected:

    private:
};

#endif // PARTICLE_H
