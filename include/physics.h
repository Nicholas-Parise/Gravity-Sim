#ifndef PHYSICS_H
#define PHYSICS_H
#include "Particle.h"
#include "Node.h"
#include "Configuration.h"
#include <thread>
#include <condition_variable>
#include <atomic>

class Physics
{
    public:
        Physics();
        virtual ~Physics();

        void updateParticles(std::vector<Particle> &particles, float time);

        void stopThreads();

    protected:

    private:

        void calculateForces();
        void forceThread(int index);
        void initThreads();

        std::thread forceWorkers[conf::PHYSICS_THREADS];
        std::vector<Particle>* particlesPtr = nullptr;
        Node* rootPtr = nullptr;

        std::condition_variable work, done;
        std::mutex mtx;
        std::atomic<bool> threadRunning{true};
        int threadsWorking = 0;
        std::vector<bool> threadShouldRun;
};

#endif // PHYSICS_H
