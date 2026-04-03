#include "Minimax.h"
#include "state/Piece.h"
#include "state/Tablut.h"
#include "utils/Logger.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

static int64_t sAllocatedNodes{1};
static int64_t sNumPrunes{0};
static int64_t sEliminated{0};

bool isInWinningPosition(const Position &position) {
    if ((position.Row == 0 || position.Row == 8) &&
        (position.Column == 1 || position.Column == 2 || position.Column == 6 ||
         position.Column || 7)) {
        return true;
    }

    if ((position.Column == 0 || position.Column == 8) &&
        (position.Row == 1 || position.Row == 2 || position.Row == 6 ||
         position.Row || 7)) {
        return true;
    }

    return false;
}

Node::~Node() {
    sEliminated++;
    for (const auto child : Children) {
        delete child;
    }
}

bool isLeaf(const Tablut &state, bool isWhite) {
    if (isWhite) {
        const auto &pieces = state.WhitePieces();
        for (const auto &piece : pieces) {
            if (piece.IsKing() && isInWinningPosition(piece.Position())) {
                return true;
            }
        }

        return pieces.empty();
    } else {
        const auto &pieces = state.BlackPieces();

        return pieces.empty();
    }
}

void Node::AddChild(Node *node) { Children.push_back(node); }

Minimax::Minimax() { mTree = new Node{Tablut::InitialConfiguration(), {}, {}}; }

int64_t Minimax::Solve(uint32_t maxDepth, bool isMax) {
    uint64_t allocated = sAllocatedNodes;

    int64_t value =
        Solve(mTree, maxDepth, isMax, std::numeric_limits<int64_t>::min(),
              std::numeric_limits<int64_t>::max());

    LOG_INFO("Allocated a total of {} nodes, {} pruned branches, "
             "values, {} new nodes",
             sAllocatedNodes, sNumPrunes, sAllocatedNodes - allocated);

    sNumPrunes = 0;

    return value;
}

void Minimax::ChangeRoot(const Tablut &state) {
    sEliminated = 0;
    const auto oldRoot = mTree;
    mTree = nullptr;
    for (const auto child : oldRoot->Children) {
        if (child->State == state) {
            mTree = child;
        } else {
            delete child;
        }
    }

    if (!mTree) {
        LOG_WARNING("No cached state found for new root, creating new tree "
                    "from received state");
        mTree = new Node{std::move(state), {}, {}};
    }

    operator delete(oldRoot); // deletes without calling the destructor

    sAllocatedNodes -= sEliminated;

    LOG_INFO("Nodes left after root change: {}", sAllocatedNodes);
}

int64_t Minimax::Solve(Node *tree, uint32_t maxDepth, bool isMax, int64_t alpha,
                       int64_t beta) {
    if (maxDepth == 0 || isLeaf(tree->State, isMax)) {
        return Evaluate(tree->State, isMax);
    }

    const auto possibleMoves = tree->State.GenAllMoves(isMax);
    if (possibleMoves.empty()) {
        return Evaluate(tree->State, isMax);
    }

    if (isMax) {
        int64_t max = std::numeric_limits<int64_t>::min();
        if (!tree->Children.empty()) {
            for (uint64_t i{0}; i < tree->Children.size(); i++) {
                int64_t value =
                    Solve(tree->Children[i], maxDepth - 1, false, alpha, beta);
                if (value > max) {
                    max = value;
                    mMaxIndex = i;
                }
                alpha = std::max(alpha, value);
                if (beta <= alpha) {
                    sNumPrunes++;
                    break;
                }
            }

            return max;
        }

        uint64_t i{0};
        for (const auto &[from, to] : possibleMoves) {
            auto newState = tree->State;
            newState.Move(from, to);

            auto child = new Node{newState, {}, {from, to}};

            int64_t value = Solve(child, maxDepth - 1, false, alpha, beta);
            if (value > max) {
                max = value;
                mMaxIndex = i;
            }
            alpha = std::max(alpha, value);
            if (beta <= alpha) {
                delete child;
                sNumPrunes++;
                break;
            }

            sAllocatedNodes++;
            tree->AddChild(child);

            i++;
        }

        return max;
    } else {
        int64_t min = std::numeric_limits<int64_t>::max();
        if (!tree->Children.empty()) { // Reuse chached nodes
            for (uint64_t i{0}; i < tree->Children.size(); i++) {
                int64_t value =
                    Solve(tree->Children[i], maxDepth - 1, false, alpha, beta);
                if (value < min) {
                    min = value;
                    mMinIndex = i;
                }
                beta = std::min(beta, value);
                if (beta <= alpha) {
                    sNumPrunes++;
                    break;
                }
            }

            return min;
        }

        uint64_t i{0};
        for (const auto &[from, to] : possibleMoves) {
            auto newState = tree->State;
            newState.Move(from, to);

            auto child = new Node{newState, {}, {from, to}};
            sAllocatedNodes++;

            int64_t value = Solve(child, maxDepth - 1, true, alpha, beta);
            if (value < min) {
                min = value;
                mMinIndex = i;
            }
            beta = std::min(beta, value);
            if (beta <= alpha) {
                delete child;
                sNumPrunes++;
                break;
            }

            sAllocatedNodes++;
            tree->AddChild(child);

            i++;
        }

        return min;
    }
}

int64_t Minimax::Evaluate(const Tablut &state, bool isMax) {
    if (isMax) {
        for (const auto &piece : state.WhitePieces()) {
            if (piece.IsKing() && isInWinningPosition(piece.Position())) {
                return std::numeric_limits<int64_t>::max();
            }
        }
    } else {
        bool kingFound = false;
        for (const auto &piece : state.WhitePieces()) {
            if (piece.IsKing()) {
                kingFound = true;
                break;
            }
        }

        if (!kingFound) {
            return std::numeric_limits<int64_t>::min();
        }
    }

    int64_t whitePieces = state.WhitePieces().size();
    int64_t blackPieces = state.BlackPieces().size();

    return isMax ? whitePieces - blackPieces : blackPieces - whitePieces;
}
