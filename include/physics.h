#ifndef PHYSICS_H
#define PHYSICS_H
#include "Particle.h"

class Physics
{
    public:
        Physics();
        virtual ~Physics();

        void calculateForces(std::vector<Particle> &particles, float time);

    protected:

    private:

        float calculateGravity(Particle p, Particle p2);

        void addtoGrid(Particle &particle);

        void resetGrid();


        // XOR X hash and Y hash then bitshift left 1
        struct pair_hash {
            std::size_t operator()(const std::pair<int,int>& p) const {
                return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
            }
        };

        std::unordered_map<std::pair<int, int>, std::vector<Particle*>, pair_hash> spatialGrid;




};

#endif // PHYSICS_H
