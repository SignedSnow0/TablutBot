#pragma once
#include <cstdint>

#include "state/Tablut.h"

struct Node {
    Tablut State;
    uint64_t Value;
    std::set<Node *> Children;

    bool operator<(const Node &r) const;

    void AddChild(const Tablut &state, uint64_t value);

    [[nodiscard]] inline const Node *Min() const { return *Children.begin(); };
    [[nodiscard]] inline const Node *Max() const { return *Children.end(); };
};
