#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

#pragma once

namespace conf{

constexpr float timeScale = 100.0f;
constexpr float G = 0.0005f;
constexpr double PI = 3.14159265358979323846;
constexpr float minSpeed = 0.00001f;
constexpr float maxSpeed = 1000.0f;

constexpr float maxY = 4000.0f;
constexpr float maxX = 4000.0f;

constexpr int particles = 1000;
constexpr int quadrentSize = 50;
}
#endif // CONFIGURATION_H_INCLUDED
