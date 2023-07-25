#pragma once

#include "NCS.hpp"

struct Component1 {
    DEFINE_COMPONENT(Component1);
    double vd = 1.0;
    float vf = 2.0f;
    int vi = 3;
};

struct Component2 {
    DEFINE_COMPONENT(Component2);
    std::vector<float> vec{ 1.0f, 1.0f, 1.0f, 0.0f };
};

struct Component3 {
    DEFINE_COMPONENT(Component3);
    int arr[4] = { 255, 255, 255, 0 };
};
