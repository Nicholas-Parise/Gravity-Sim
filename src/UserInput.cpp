#include "UserInput.h"
#include "configuration.h"
#include <cmath>

UserInput::UserInput(){
    pan = { 1.0, 1.0 };
    zoom = 1.0;
    isDragging = false;
    basePanSpeed = 200.0f;
    panStep = 0.0;
    baseZoom = 0.9;
    zoomSteps = -8;
}

UserInput::~UserInput(){
    //dtor
}


sf::Vector2<float> UserInput::getPan(){
    return pan;
}

float UserInput::getZoom(){
    zoom = pow(baseZoom, zoomSteps);
    panStep = basePanSpeed * zoom;
    return zoom;
}


void UserInput::handleMouse(const std::optional<sf::Event>& event, sf::RenderWindow &window){

    if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()){
        switch (mouseWheelScrolled->wheel){
        case sf::Mouse::Wheel::Vertical:

            if(mouseWheelScrolled->delta >0){

                zoomSteps++;
            }
            else{
                if(window.getSize().y * zoom < conf::maxY*2.5f){
                    zoomSteps--;
                }
            }
            break;
        case sf::Mouse::Wheel::Horizontal:
            break;
        }
    }

    if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()){
        if (mouseButtonPressed->button == sf::Mouse::Button::Left){
            isDragging = true;
            dragStartMouse = sf::Mouse::getPosition(window);
            dragStartCenter = pan; // the center when you first clicked

        }
    }


    if (const auto* MouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()){
        if (MouseButtonReleased->button == sf::Mouse::Button::Left){
            isDragging = false;
        }
    }


    if (isDragging){
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
}


void UserInput::handleKeyboard(sf::RenderWindow &window, float deltaTime){

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Add)){
        zoomSteps++;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Subtract)){
        if(window.getSize().y * zoom < conf::maxY*2.5f){
            zoomSteps--;
        }
    }

    zoom = pow(baseZoom, zoomSteps);
    panStep = this->basePanSpeed * zoom;

    float adjustedStep = panStep * deltaTime;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
        pan.x -= adjustedStep;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
        pan.x += adjustedStep;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)){
        pan.y -= adjustedStep;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)){
        pan.y += adjustedStep;
    }

}


