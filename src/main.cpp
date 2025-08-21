#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
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

int ScreenWidth = 1920;//1280;
int ScreenHeight = 1080;//720;

std::thread physicsWorker;
std::mutex physicsMutex;
std::atomic<float> tps = 0.016f; // 1/60
std::atomic<bool> redraw = true; // 1/60
bool threadRunning = true;

extern size_t forceCalls;

void textUpdater(sf::Text &text, long double value, string header){

    int precision = 5;
    std::stringstream ss;
    if(value < 10000){
        ss << value;
    }else{
        ss << std::setprecision(precision) << value;
    }
   // std::string varAsString = std::to_string(value);
    text.setString(header+ss.str());
}


void physicsThread(Physics &P, std::vector<Particle> &particles){
    threadRunning = true;

    physicsWorker = std::thread([&]()
    {

        std::chrono::steady_clock::time_point lastUpdate;

        while (threadRunning){
            {
                //float dt = deltaTime.load();
                //std::lock_guard<std::mutex> lock(physicsMutex);
                auto now = std::chrono::steady_clock::now();
                float dt = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
                lastUpdate = now;
                //std::cout<<"dt physics: "<<dt<<std::endl;
                tps.store(dt);
                P.updateParticles(particles,dt);
                redraw.store(true);
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
        particles[i].setMass(rand()%100+100);
        particles[i].setspeed((static_cast<double>(rand()) / RAND_MAX) * 8.0 - 4.0, (static_cast<double>(rand()) / RAND_MAX) * 8.0 - 4.0);
    }

    particles[0].setMass(2e8);
    particles[0].setspeed((static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0, (static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0);
    particles[0].setPosition(50,50);

    particles[1].setMass(2e7);
    particles[1].setspeed((static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0, (static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0);

    particles[2].setMass(2e6);
    particles[2].setspeed((static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0, (static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0);

    physicsThread(P, particles);

    std::chrono::steady_clock::time_point lastUpdate;
    float dt;
    auto now = std::chrono::steady_clock::now();
    dt = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
    lastUpdate = now;


    sf::ContextSettings settings;
    settings.majorVersion = 2;
    settings.minorVersion = 1;
    settings.attributeFlags = sf::ContextSettings::Default;
    sf::RenderWindow window(
                            sf::VideoMode({ScreenWidth, ScreenHeight}),
                            "Gravity",
                            sf::Style::Default,
                            sf::State::Windowed,
                            sf::ContextSettings(settings)
                            );

    //window.setFramerateLimit(60);

    // Create a fullscreen rectangle
    sf::RectangleShape fullscreenQuad(sf::Vector2f(window.getSize().x, window.getSize().y));
    fullscreenQuad.setFillColor(sf::Color::Black);

    sf::Shader crosshairShader;
    if (!crosshairShader.loadFromFile("src/crosshair.frag", sf::Shader::Type::Fragment))
        return -1;

    crosshairShader.setUniform("screenSize", sf::Glsl::Vec2(window.getSize().x, window.getSize().y));


    sf::Shader circleShader;
    if (!circleShader.loadFromFile("src/circle.vert","src/circle.frag"))
        return -1;

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

    sf::Text fps_Text(font,"0", 30);
    fps_Text.setFillColor(sf::Color::White);
    fps_Text.setOrigin(sf::Vector2f(15,15));
    fps_Text.setPosition({15,65});

    sf::Text tps_Text(font,"0", 30);
    tps_Text.setFillColor(sf::Color::White);
    tps_Text.setOrigin(sf::Vector2f(15,15));
    tps_Text.setPosition({15,105});

    window.setActive(false);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {

            if (event->is<sf::Event::Closed>())
            {
                threadRunning = false;
                P.stopThreads();
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


        UI.handleKeyboard(window,dt);

        if(redraw.load()){

            //std::cout<<forceCalls<<endl;
            forceCalls = 0;

            redraw.store(false);
            //renderQuad.clear();
            renderQuad.resize(0);
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
        }

        // update the text 1 / 5 frames
        if(rand()%10 > 8){
            textUpdater(fps_Text, ceil(1.0/dt), "FPS: ");
            textUpdater(tps_Text, ceil(1.0/tps.load()), "TPS: ");
        }

        window.clear(sf::Color::Black);

        sf::View view = window.getView();
        view.setCenter(UI.getPan());
        view.setSize({ScreenWidth * UI.getZoom(), ScreenHeight * UI.getZoom()});
        window.setView(view);

        //window.draw(fullscreenQuad);
        {
            window.draw(renderQuad, &circleShader);
        }

        window.setView(window.getDefaultView());

        crosshairShader.setUniform("background", fullscreenQuad.getTexture());
        //window.draw(crosshair, &crosshairShader);
        window.draw(particle_Text);
        window.draw(fps_Text);
        window.draw(tps_Text);

        window.setView(view);

        window.display();
    }
    threadRunning = false;
    P.stopThreads();
    if (physicsWorker.joinable()) physicsWorker.join();
    return 0;
}
