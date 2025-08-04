#ifndef PARTICLE_H
#define PARTICLE_H


class particle
{
    public:
        particle();
        virtual ~particle();
        float distance(particle p);

        float mass;
        float density;
        float direction;
        float velocity;



    protected:

    private:
};

#endif // PARTICLE_H
