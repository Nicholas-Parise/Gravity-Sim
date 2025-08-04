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


using namespace std;

const int dragThreshold = 1; // min pixels for drag re-render

int ScreenWidth = 720;
int ScreenHeight = 480;

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
    sf::Vector2<long double> dragStartCenter;

    sf::Vector2<long double> pan = { 0.0, 0.0 };
    sf::Vector2<long double> prevPan = { 0.0, 0.0 };

    long double basePanSpeed = 0.01f;
    long double panStep = 0.0;

    long double prevZoom = 0.9;
    long double zoom = 1.0;
    long double baseZoom = 0.9;
    int zoomSteps = -8;

    std::vector<particle> particles(conf::particles);

    sf::VertexArray renderQuad(sf::PrimitiveType::LineStrip, 4*conf::particles);

    for(int i = 0; i<conf::particles; i++){
        particles[i].setPosition(i*10,i*3);
    }



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

    sf::Clock clock;

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
            pan.y += panStep;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        {
            pan.y -= panStep;
        }


        if (isDragging)
        {
            sf::Vector2i currentMouse = sf::Mouse::getPosition(window);
            sf::Vector2i mouseDelta = dragStartMouse - currentMouse; // reversed direction (drag moves view)

            if (std::abs(mouseDelta.x) >= dragThreshold || std::abs(mouseDelta.y) >= dragThreshold)
            {
                long double aspectRatio = (long double)(window.getSize().x) / (long double)(window.getSize().y);
                long double scaledZoomX = zoom * aspectRatio;
                long double scaledZoomY = zoom;

                sf::Vector2<long double> pixelDelta =
                {
                    scaledZoomX / (long double)(window.getSize().x),
                    scaledZoomY / (long double)(window.getSize().y)
                };

                long double temp = -1;

                // Convert pixel delta to complex space delta
                sf::Vector2<long double> complexDelta =
                {
                    mouseDelta.x * pixelDelta.x,
                    mouseDelta.y * temp * pixelDelta.y
                };

                pan = dragStartCenter + complexDelta;
                dragStartMouse = currentMouse;
                dragStartCenter = pan;
            }
        }

        long double aspectRatio = (long double)(window.getSize().x) / (long double)(window.getSize().y);

        sf::Vector2<long double> delta =
        {
            zoom * aspectRatio / (long double)(window.getSize().x),
            //zoom / double(window.getSize().x),
            zoom / double(window.getSize().y)
        };

        sf::Vector2<long double> origin =
        {
            pan.x - delta.x * (long double)(window.getSize().x) * 0.5,
            pan.y - delta.y * (long double)(window.getSize().y) * 0.5
        };

        ScreenWidth = window.getSize().x;
        ScreenHeight = window.getSize().y;

        fullscreenQuad.setOrigin({ScreenWidth / 2.f, ScreenHeight / 2.f});
        fullscreenQuad.setScale({1.0f, -1.0f});
        fullscreenQuad.setPosition({ScreenWidth / 2.f, ScreenHeight / 2.f});
/*
        float scaleX = static_cast<float>(prevDelta.x / delta.x);
        float scaleY = static_cast<float>(prevDelta.y / delta.y);

        // Pixel offset caused by panning and zoom
        sf::Vector2f offset =
        {
            static_cast<float>((prevOrigin.x - origin.x) / delta.x
                               - 0.5f * ScreenWidth  * (1.f - scaleX)),
            static_cast<float>((prevOrigin.y - origin.y) / delta.y
                               - 0.5f * ScreenHeight * (1.f - scaleY))
        };

        fullscreenQuad.setScale({scaleX, -scaleY});
        fullscreenQuad.setPosition(
        {
            ScreenWidth / 2.f + offset.x,
            ScreenHeight / 2.f - offset.y
        });

*/



    renderQuad.clear();
    for(int i = 0; i<conf::particles; i++){
        particles[i].move(1.0f);
        sf::VertexArray temp = particles[i].generateQuad();
        for(int j = 0; j <4; j++){
            renderQuad.append(temp[j]);
        }
    }


        window.clear();

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
