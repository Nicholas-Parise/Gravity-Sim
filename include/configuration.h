#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#pragma once

namespace conf{

constexpr float timeScale = 1.0f;
constexpr float G = 6.67e-1f;
constexpr double PI = 3.14159265358979323846;
constexpr float minSpeed = 0.00001f;
constexpr float maxSpeed = 10000.0f;

constexpr float maxDt = 0.0625f;

constexpr float minPhysDistance = 0.00001f;


constexpr float MIN_QUAD_SIZE = 0.0001;

constexpr float theta = 0.50f; // simulation accuracy 0 most 1 least

constexpr float dragCoeff = 0.5f;

constexpr float maxY = 6000.0f;
constexpr float maxX = 6000.0f;


constexpr int PHYSICS_THREADS = 1;

constexpr int particles = 10000;
constexpr int quadrentSize = 50;
}
#endif // CONFIGURATION_H_INCLUDED
