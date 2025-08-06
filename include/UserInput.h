#ifndef USERINPUT_H
#define USERINPUT_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class UserInput
{
    public:
        UserInput();
        virtual ~UserInput();

            sf::Vector2<float> getPan();
            float getZoom();

            sf::Vector2<float> pan = { 1.0, 1.0 };
            float zoom = 1.0;

            void handleMouse(const std::optional<sf::Event>& event, sf::RenderWindow &window);
            void handleKeyboard(sf::RenderWindow &window, float deltaTime);

    protected:

    private:

        bool isDragging = false;
        sf::Vector2i dragStartMouse;
        sf::Vector2<float> dragStartCenter;
        const int dragThreshold = 1; // min pixels for drag re-render

        float basePanSpeed = 1000.0f;
        float panStep = 0.0;
        float baseZoom = 0.9;
        int zoomSteps = -8;
};

#endif // USERINPUT_H
