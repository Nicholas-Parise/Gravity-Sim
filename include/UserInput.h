#ifndef USERINPUT_H
#define USERINPUT_H
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>

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

        float basePanSpeed;
        float panStep;
        float baseZoom;
        int zoomSteps;
};

#endif // USERINPUT_H
