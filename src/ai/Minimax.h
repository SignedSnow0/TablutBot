#pragma once
#include <cstdint>

#include "state/Piece.h"
#include "state/Tablut.h"

struct Node {
    Tablut State;
    std::vector<Node *> Children;
    std::pair<Position, Position> Move;

    ~Node();

    void AddChild(Node *node);
};

class Minimax {
public:
    Minimax();

    void ChangeRoot(const Tablut &state);

    int64_t Solve(uint32_t maxDepth, bool isMax);

    [[nodiscard]] inline const Tablut &CurrentState() const {
        return mTree->State;
    }

    [[nodiscard]] inline std::pair<Position, Position> MaxMove() const {
        return mTree->Children[mMaxIndex]->Move;
    }

    [[nodiscard]] inline std::pair<Position, Position> MinMove() const {
        return mTree->Children[mMinIndex]->Move;
    }

private:
    int64_t Solve(Node *tree, uint32_t maxDepth, bool isMax, int64_t alpha,
                  int64_t beta);

    int64_t Evaluate(const Tablut &state, bool isMax);

    Node *mTree;
    uint64_t mMaxIndex;
    uint64_t mMinIndex;
};
