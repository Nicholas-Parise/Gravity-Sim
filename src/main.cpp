#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <cstdlib>

#include "Configuration.h"
#include "Particle.h"
#include "Physics.h"
#include "UserInput.h"

using namespace std;

int ScreenWidth = 720;
int ScreenHeight = 480;

std::thread physicsWorker;
std::mutex physicsMutex;
std::atomic<float> deltaTime = 0.016f; // 1/60
bool threadRunning = true;

void textUpdater(sf::Text &text, long double value, string header){

    int precision = 5;
    std::stringstream ss;
    if(value < 10000){
        ss << value;
    }else{
        ss << std::scientific << std::setprecision(precision) << value;
    }
   // std::string varAsString = std::to_string(value);
    text.setString(header+ss.str());
}


void physicsThread(Physics &P, std::vector<Particle> &particles){
    threadRunning = true;
    physicsWorker = std::thread([&]()
    {
        while (threadRunning){
            {
                float dt = deltaTime.load();
                //std::lock_guard<std::mutex> lock(physicsMutex);
                P.calculateForces(particles,dt);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
}


int main()
{
    UserInput UI;

    Physics P;

    srand(time(0));

    std::vector<Particle> particles(conf::particles);

    sf::VertexArray renderQuad(sf::PrimitiveType::Triangles, 6*conf::particles);

    for(int i = 0; i<conf::particles; i++){
        particles[i].setPosition(rand()%(int)(conf::maxX)-(int)conf::maxX/2,rand()%(int)(conf::maxY)-(int)conf::maxY/2);
        particles[i].setMass(rand()%100+50);
        particles[i].setspeed((static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0, (static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0);
    }

    particles[0].setMass(2e7);

    particles[1].setMass(2e7);

    particles[2].setMass(2e6);

    physicsThread(P, particles);

    std::chrono::steady_clock::time_point lastUpdate;
    float dt;
    auto now = std::chrono::steady_clock::now();
    dt = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
    lastUpdate = now;


    sf::RenderWindow window(sf::VideoMode({ScreenWidth, ScreenHeight}), "Gravity");
    //window.setFramerateLimit(60);

    // Create a fullscreen rectangle
    sf::RectangleShape fullscreenQuad(sf::Vector2f(window.getSize().x, window.getSize().y));
    fullscreenQuad.setFillColor(sf::Color::Black);

    sf::Shader crosshairShader;
    if (!crosshairShader.loadFromFile("src/crosshair.frag", sf::Shader::Type::Fragment))
        return -1;

    crosshairShader.setUniform("screenSize", sf::Glsl::Vec2(window.getSize().x, window.getSize().y));

    // create crosshair
    sf::ConvexShape crosshair;
    crosshair.setPointCount(12);
    crosshair.setPoint(0, sf::Vector2f(0, 10));
    crosshair.setPoint(1, sf::Vector2f(10, 10));
    crosshair.setPoint(2, sf::Vector2f(10, 0));
    crosshair.setPoint(3, sf::Vector2f(13, 0));
    crosshair.setPoint(4, sf::Vector2f(13, 10));
    crosshair.setPoint(5, sf::Vector2f(23, 10));
    crosshair.setPoint(6, sf::Vector2f(23, 13));
    crosshair.setPoint(7, sf::Vector2f(13, 13));
    crosshair.setPoint(8, sf::Vector2f(13, 23));
    crosshair.setPoint(9, sf::Vector2f(10, 23));
    crosshair.setPoint(10, sf::Vector2f(10, 13));
    crosshair.setPoint(11, sf::Vector2f(0, 13));
    crosshair.setOrigin({12.5f, 12.5f});
    crosshair.setPosition({ScreenWidth / 2.f, ScreenHeight / 2.f});

    const sf::Font font("assets/Arial.ttf");

    sf::Text particle_Text(font,"0", 30);
    particle_Text.setFillColor(sf::Color::White);
    particle_Text.setOrigin(sf::Vector2f(15,15));
    particle_Text.setPosition({15,15});

    textUpdater(particle_Text,conf::particles,"Particles: ");

    window.setActive(false);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {

            if (event->is<sf::Event::Closed>())
            {
                threadRunning = false;
                if (physicsWorker.joinable()) physicsWorker.join();
                return 0;
            }
            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {

                sf::View view = window.getDefaultView();
                view.setSize({resized->size.x, resized->size.y});
                view.setCenter({resized->size.x / 2.f, resized->size.y / 2.f});
                window.setView(view);

                fullscreenQuad.setSize(sf::Vector2f(resized->size.x, resized->size.y));

                crosshairShader.setUniform("screenSize", sf::Glsl::Vec2(resized->size.x, resized->size.y));

                ScreenWidth = resized->size.x;
                ScreenHeight = resized->size.y;

                crosshair.setPosition({ScreenWidth / 2.f, ScreenHeight / 2.f});
            }

            UI.handleMouse(event,window);
        }

        //------

        auto now = std::chrono::steady_clock::now();
        dt = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
        lastUpdate = now;

        deltaTime.store(dt);

        UI.handleKeyboard(window,dt);

        // moved physics to thread
        //P.calculateForces(particles,dt);

        renderQuad.clear();
        for(int i = 0; i<conf::particles; i++){
            sf::VertexArray quad = particles[i].generateQuad();
            for(int j = 0; j <quad.getVertexCount(); j++){
                renderQuad.append(quad[j]);
            }
            // DEBUG
            if (std::isnan(particles[i].position.x) || std::isnan(particles[i].position.y)) {
                std::cout << "NaN at index " << i << ": " << particles[i].position.x << ", " << particles[i].position.y << std::endl;
            }
        }


//        std::cout<<"node 0 x:"<<particles[0].acceleration.x<<" y:"<<particles[0].acceleration.y<<std::endl;

        window.clear();

        sf::View view = window.getView();
        view.setCenter(UI.getPan());
        view.setSize({ScreenWidth * UI.getZoom(), ScreenHeight * UI.getZoom()});
        window.setView(view);

        window.draw(fullscreenQuad);

        crosshairShader.setUniform("background", fullscreenQuad.getTexture());
        window.draw(crosshair, &crosshairShader);
        window.draw(particle_Text);

        window.draw(renderQuad);

        window.display();
    }
    threadRunning = false;
    if (physicsWorker.joinable()) physicsWorker.join();
    return 0;
}
