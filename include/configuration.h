#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#pragma once

namespace conf{

constexpr float timeScale = 1.0f;
constexpr float G = 6.67e-2f;
constexpr double PI = 3.14159265358979323846;
constexpr float minSpeed = 0.00001f;
constexpr float maxSpeed = 10000.0f;

constexpr float maxDt = 0.0166f;

constexpr float minPhysDistance = 0.01f;


constexpr float MIN_QUAD_SIZE = 0.0001;

constexpr float theta = 0.70f; // simulation accuracy 0 most 1 least

constexpr float dragCoeff = 0.5f;

constexpr float maxY = 5000.0f;
constexpr float maxX = 5000.0f;

constexpr int ScreenWidth = 1920;
constexpr int ScreenHeight = 1080;

constexpr int PHYSICS_THREADS = 4;

constexpr int particles = 50000;


}
#endif // CONFIGURATION_H_INCLUDED
