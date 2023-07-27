#pragma once

#include <memory>
#include <sstream>
#include <iostream>

#define TEST(expression)        \
if (!(expression)) {            \
    std::stringstream ss;       \
    ss << "[!! TEST FAILED !!]" \
    << "> file:" << __FILE__    \
    << ", line:" << __LINE__    \
    << std::endl;               \
    std::cout << ss.str();      \
    exit(1);                    \
}
