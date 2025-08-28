#ifndef PARTICLEBUILDER_H
#define PARTICLEBUILDER_H
#include "Particle.h"
#include "Configuration.h"

class ParticleBuilder
{
    public:
        ParticleBuilder();
        virtual ~ParticleBuilder();

        ParticleBuilder& setPositionUniform();
        ParticleBuilder& setPositionUniform(float border);
        ParticleBuilder& setPositionRing(float radius);
        ParticleBuilder& setPositionCirlce(float radius);

        ParticleBuilder& setMass(float mass);
        ParticleBuilder& setMass(float min, float max); // random num between

        ParticleBuilder& setVelocity(float velocity);
        ParticleBuilder& setVelocity(float min, float max); // random num between

        ParticleBuilder& setVelocityCircle(float centerX, float centerY, float speed);

        ParticleBuilder& setOrbitCircle(float centerX, float centerY, float mass);


        ParticleBuilder& setAcceleration(float acc);
        ParticleBuilder& setAcceleration(float min, float max); // random num between

        std::vector<Particle>& generate();

    protected:

    private:

        std::vector<Particle> particles;
};

#endif // PARTICLEBUILDER_H
