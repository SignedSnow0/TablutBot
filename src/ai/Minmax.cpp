#include "Minmax.h"

bool Node::operator<(const Node &r) const { return Value < r.Value; }

void Node::AddChild(const Tablut &state, uint64_t value) {
    Children.insert(new Node{std::move(state), value, {}});
}
