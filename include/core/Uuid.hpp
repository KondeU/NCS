#pragma once

#include <cstdint>

namespace au::ncs {

using Uuid = uint32_t;
using Name = const char*;

constexpr Uuid MakeUuid(Name sz, Uuid seed)
{
    // 0x9E3779B9 : golden ratio constant, used for better hash scattering
    // https://lkml.org/lkml/2016/4/29/838
    return (*sz == '\0') ? seed :
        MakeUuid(sz + 1, seed ^ (static_cast<Uuid>(*sz) + 0x9E3779B9 + (seed << 6) + (seed >> 2)));
}

template <Uuid U>
struct EnsureConst
{
    static const Uuid V = U;
};

}

// 0xEE6B27EB : golden seed constant, prime number in four billion
#define AU_CT_UUID(name) (au::ncs::EnsureConst<au::ncs::MakeUuid(#name, 0xEE6B27EB)>::V)
#define AU_RT_UUID(name) (au::ncs::MakeUuid(#name, 0xEE6B27EB))
