#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#pragma once

namespace conf{

constexpr float timeScale = 1.0f;
constexpr float G = 6.67e-2f;
constexpr double PI = 3.14159265358979323846;
constexpr float MIN_SPEED = 0.00001f;
constexpr float MAX_SPEED = 10000.0f;

constexpr float MAX_DT = 0.0166f;

constexpr float MIN_PHYSICS_DISTANCE = 0.01f;

constexpr bool SAVE_PICTURES = false;

constexpr float MIN_QUAD_SIZE = 0.0001;

constexpr float THETA = 0.70f; // simulation accuracy 0 most 1 least

constexpr float DRAG_COEFF = 0.5f;

constexpr float MAX_Y = 5000.0f;
constexpr float MAX_X = 5000.0f;

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;

constexpr int PHYSICS_THREADS = 4;

constexpr int particles = 50000;


}
#endif // CONFIGURATION_H_INCLUDED
