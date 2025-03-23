#pragma once

#include <stdexcept>

#if __cplusplus >= 202002L
#define if_likely(condition) if (condition) [[likely]]
#define if_unlikely(condition) if (condition) [[unlikely]]
#elif defined(__GNUC__) || defined(__clang__)
#define if_likely(condition) if (__builtin_expect(!!(condition), 1))
#define if_unlikely(condition) if (__builtin_expect(!!(condition), 0))
#else
#define if_likely(condition) if (condition)
#define if_unlikely(condition) if (condition)
#endif

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define __FILE_LINE__ __FILE__ ":" TO_STRING(__LINE__)
#define ct_assert(condition) static_assert(condition, __FILE_LINE__)
#if defined(DEBUG) || defined(_DEBUG)
#define rt_assert(condition) ((condition) ? (false) : \
    (throw std::logic_error("rt_assert failed at " __FILE_LINE__), true))
#else // rt_assert will return true if assert failed in release mode.
#define rt_assert(condition) ((condition) ? (false) : (true))
#endif
#define rt_assert_if(condition) if_unlikely (rt_assert(condition))
