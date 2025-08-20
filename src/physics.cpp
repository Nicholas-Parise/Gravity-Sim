#include "Physics.h"
#include "configuration.h"
#include <cmath>
#include <iostream>

Physics::Physics()
{
    initThreads();
}

Physics::~Physics()
{
    //dtor
}


/**
Allow the program to close nicely by stopping all the threads
*/
void Physics::stopThreads(){

    {
        std::lock_guard<std::mutex> lock(mtx);
        threadRunning.store(false);

        for (int i = 0; i < conf::PHYSICS_THREADS; ++i)
            threadShouldRun[i] = false;
    }

    work.notify_all();

    for (int i = 0; i < conf::PHYSICS_THREADS; i++) {
        if (forceWorkers[i].joinable())
            forceWorkers[i].join();
    }
}

/**
Start threads
*/
void Physics::initThreads(){

    threadRunning.store(true);
    threadShouldRun.resize(conf::PHYSICS_THREADS, false);
    for (int i = 0; i < conf::PHYSICS_THREADS; i++) {
        forceThread(i);
    }
}

/**
notifys all threads that they can now start computing the forces
block until all the threads are done
*/
void Physics::calculateForces() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        threadsWorking = conf::PHYSICS_THREADS;
        for (int i = 0; i < conf::PHYSICS_THREADS; ++i)
            threadShouldRun[i] = true;
    }
    work.notify_all();

    // Block until all worker threads finish
    {
        std::unique_lock<std::mutex> lock(mtx);
        done.wait(lock, [this]() { return threadsWorking == 0; });
    }
}


/**
Actually does the physics calculation
Blocks until it's notifed there is work available
performs calculations
decrements the threads running counter
if there are no more threads running notify that we are all done.
*/
void Physics::forceThread(int index){

    int partPerThread = conf::particles / conf::PHYSICS_THREADS;
    int start = partPerThread * index;
    int end = (index == conf::PHYSICS_THREADS - 1) ? conf::particles : start + partPerThread;

    forceWorkers[index] = std::thread([this, start, end, index](){

        while (true){

            { // block thread and wait until there is work to do.
                std::unique_lock<std::mutex> lock(mtx);
                work.wait(lock, [this, index]() { return threadShouldRun[index] || !threadRunning.load(); });
                if (!threadRunning.load()) break;
                if(threadsWorking <= 0) break;

                threadShouldRun[index] = false;
            }
            auto& particles = *this->particlesPtr;
            Node* root = this->rootPtr;

            for (int i = start; i < end; ++i) {
                Particle& p = particles[i];
                sf::Vector2f totalForce{0.f, 0.f};
                root->computeForce(&p, totalForce);
                p.addAcceleration(totalForce);
            }

            // set thread as done and notify if all threads are done.
            {
                std::lock_guard<std::mutex> lock(mtx);
                --threadsWorking;
                assert(threadsWorking >= 0 && "threadsWorking went below zero!");
                if (threadsWorking == 0) {
                    done.notify_one();
                }
            }

        }
    });
}


/**
Update each particles position, velocity, and acceleration
starts by moving each particle and reseting variables
then it adds each particle to a quad tree based on position
it then calls calculateForce which tells all the worker threads to run
finally it updates the particles veloctiy with the new force
*/
void Physics::updateParticles(std::vector<Particle> &particles, float time){

    float dt = std::min(time * conf::timeScale, conf::maxDt);

    Quad boundary = {0, 0, conf::maxX * 2};
    rootPtr = new Node(boundary);

    // move and reset temp acceleration
    for(int i = 0; i<conf::particles; i++){
        particles[i].move(dt);
        particles[i].resetAcceleration();

        if (boundary.contains(particles[i].position)){
            rootPtr->insert(&particles[i]);
        }
    }

    particlesPtr = &particles;

    // calculate all forces using threads
    if (!threadRunning.load()) return;
    calculateForces();

    // update all velocities
    for(int i = 0; i<conf::particles; i++){
        particles[i].updateVelocity(dt);
    }

    delete rootPtr;
}


