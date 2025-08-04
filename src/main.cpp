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

#include "configuration.h"
#include "particle.h"
#include "physics.h"

using namespace std;

const int dragThreshold = 1; // min pixels for drag re-render
const int originalSize = 1;

int ScreenWidth = 720;
int ScreenHeight = 480;

sf::Vector2<long double> prevOrigin;
sf::Vector2<long double> prevDelta;

sf::Image pictures({ScreenWidth, ScreenHeight}, sf::Color::Black);
sf::Texture MapTexture(pictures);
sf::Sprite Background(MapTexture);

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


int main()
{
    bool isDragging = false;
    sf::Vector2i dragStartMouse;
    sf::Vector2<float> dragStartCenter;

    sf::Vector2<float> pan = { 1.0, 1.0 };
    sf::Vector2<float> prevPan = { 1.0, 1.0 };

    float basePanSpeed = 0.1f;
    float panStep = 0.0;

    float prevZoom = 0.9;
    float zoom = 1.0;
    float baseZoom = 0.9;
    int zoomSteps = -8;

    sf::Clock clock;

    std::vector<particle> particles(conf::particles);

    sf::VertexArray renderQuad(sf::PrimitiveType::Triangles, 6*conf::particles);

    for(int i = 0; i<conf::particles; i++){
        particles[i].setPosition(i*10,i*3);
    }


    std::chrono::steady_clock::time_point lastUpdate;
    float deltaTime;
    auto now = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
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
             //   if (renderThread.joinable()) renderThread.join();
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

            if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                switch (mouseWheelScrolled->wheel)
                {
                case sf::Mouse::Wheel::Vertical:
                    // std::cout << "wheel type: vertical" << std::endl;

                    if(mouseWheelScrolled->delta >0)
                    {

                        zoomSteps++;
                        prevZoom = zoom;
                    }
                    else
                    {
                        zoomSteps--;
                        prevZoom = zoom;
                    }
                    break;
                case sf::Mouse::Wheel::Horizontal:
                    //std::cout << "wheel type: horizontal" << std::endl;
                    break;
                }
                //std::cout << "wheel movement: " << mouseWheelScrolled->delta << std::endl;
                //std::cout << "mouse x: " << mouseWheelScrolled->position.x << std::endl;
                //std::cout << "mouse y: " << mouseWheelScrolled->position.y << std::endl;
            }


            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    //std::cout << "the left button was pressed" << std::endl;
                    //std::cout << "mouse x: " << mouseButtonPressed->position.x << std::endl;
                    //std::cout << "mouse y: " << mouseButtonPressed->position.y << std::endl;

                    isDragging = true;
                    dragStartMouse = sf::Mouse::getPosition(window);
                    dragStartCenter = pan; // the center when you first clicked

                }
            }


            if (const auto* MouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (MouseButtonReleased->button == sf::Mouse::Button::Left)
                {
                    //std::cout << "the left button was released" << std::endl;
                    //std::cout << "mouse x: " << MouseButtonReleased->position.x << std::endl;
                    //std::cout << "mouse y: " << MouseButtonReleased->position.y << std::endl;
                    isDragging = false;
                }
            }
        }


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Add))
        {
            zoomSteps++;
            prevZoom = zoom;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Subtract))
        {
            zoomSteps--;
            prevZoom = zoom;
        }


        zoom = pow(baseZoom, zoomSteps);
        panStep = basePanSpeed * zoom;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        {
            pan.x -= panStep;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        {
            pan.x += panStep;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        {
            pan.y -= panStep;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        {
            pan.y += panStep;
        }


        if (isDragging)
        {
            sf::Vector2i currentMouse = sf::Mouse::getPosition(window);
            sf::Vector2i mouseDelta = currentMouse - dragStartMouse;

            if (std::abs(mouseDelta.x) >= dragThreshold || std::abs(mouseDelta.y) >= dragThreshold)
            {
                sf::Vector2f worldStart = window.mapPixelToCoords(dragStartMouse);
                sf::Vector2f worldNow   = window.mapPixelToCoords(currentMouse);

                sf::Vector2f worldDelta = worldStart - worldNow;

                pan = dragStartCenter + worldDelta;
            }
        }


        auto now = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
        lastUpdate = now;

        physics P;

        P.calculateForces(particles,deltaTime);

        renderQuad.clear();
        for(int i = 0; i<conf::particles; i++){
            particles[i].move(deltaTime);
            sf::VertexArray quad = particles[i].generateQuad();
            for(int j = 0; j <quad.getVertexCount(); j++){
                renderQuad.append(quad[j]);
            }
        }


        window.clear();

        sf::View view = window.getView();
        view.setCenter(pan);
        view.setSize({ScreenWidth * zoom, ScreenHeight * zoom});
        window.setView(view);

        window.draw(fullscreenQuad);

        crosshairShader.setUniform("background", fullscreenQuad.getTexture());
        window.draw(crosshair, &crosshairShader);
        window.draw(particle_Text);

        window.draw(renderQuad);

        window.display();
    }
    //if (renderThread.joinable()) renderThread.join();
    return 0;
}
