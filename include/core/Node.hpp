#pragma once

#include "Uuid.hpp"

namespace au::ncs {

class Node final {
public:
    using Id = Uuid;

    explicit constexpr Node(Id id = 0) noexcept : id(id)
    {
    }

    bool IsInvalid() const noexcept
    {
        return (id == 0);
    }

    operator Id() const noexcept
    {
        return id;
    }

private:
    Id id;
};

}

namespace std {

template <>
struct hash<au::ncs::Node> {
    au::ncs::Node::Id operator()(const au::ncs::Node& node) const noexcept
    {
        return node;
    }
};

}
