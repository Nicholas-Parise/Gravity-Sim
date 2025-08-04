#ifndef PHYSICS_H
#define PHYSICS_H
#include "particle.h"

class physics
{
    public:
        physics();
        virtual ~physics();

        void calculateForces(std::vector<particle> &particles, float time);



    protected:

    private:

        float calculateGravity(particle p, particle p2);


};

#endif // PHYSICS_H
