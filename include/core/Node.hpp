#pragma once

#include <cstdint>

namespace au::framework {

class Entity {
public:
    using EntityId = uint32_t;

    explicit constexpr Entity(EntityId id = 0) noexcept : id(id)
    {
    }

    bool IsInvalid()
    {
        return (id == 0);
    }

    operator EntityId() const
    {
        return id;
    }

private:
    EntityId id;
};

}

namespace std {

template <>
struct hash<au::framework::Entity> {
    au::framework::Entity::EntityId operator()(const au::framework::Entity& entity) const
    {
        return entity;
    }
};

}
