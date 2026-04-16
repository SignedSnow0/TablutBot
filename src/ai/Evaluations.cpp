#include "Evaluations.h"

#include <cstdint>
#include <limits>
#include <queue>

#include "state/Tablut.h"
#include "state/Utils.h"

uint8_t EscapeRoutes(const Tablut &t, const PiecePosition &king) {
    uint8_t routes{0};

    bool clear = true;
    for (int8_t row = king.Row - 1; row >= 0; row--) {
        if (!t.IsEmpty(row, king.Column) ||
            PositionIsUnreachable(row, king.Column)) {
            clear = false;
            break;
        }
    }
    if (clear) {
        routes++;
    }

    clear = true;
    for (int8_t column = king.Column - 1; column >= 0; column--) {
        if (!t.IsEmpty(king.Row, column) ||
            PositionIsUnreachable(king.Row, column)) {
            clear = false;
            break;
        }
    }
    if (clear) {
        routes++;
    }

    clear = true;
    for (int8_t row = king.Row + 1; row < BOARD_SIZE; row++) {
        if (!t.IsEmpty(row, king.Column) ||
            PositionIsUnreachable(row, king.Column)) {
            clear = false;
            break;
        }
    }
    if (clear) {
        routes++;
    }

    clear = true;
    for (int8_t column = king.Column + 1; column < BOARD_SIZE; column++) {
        if (!t.IsEmpty(king.Row, column) ||
            PositionIsUnreachable(king.Row, column)) {
            clear = false;
            break;
        }
    }
    if (clear) {
        routes++;
    }

    return routes;
}

uint8_t MercenariesAdjacentKing(const Tablut &t, const PiecePosition &king) {
    uint8_t count{0};

    for (uint8_t row = king.Row - 1; row <= king.Row + 1; row++) {
        for (uint8_t column = king.Column - 1; column <= king.Column + 1;
             column++) {
            if (t.IsType(row, column, PieceType::Mercenary)) {
                count++;
            }
        }
    }

    return count;
}

uint8_t GuardsAdjacentKing(const Tablut &t, const PiecePosition &king) {
    uint8_t count{0};

    for (uint8_t row = king.Row - 1; row <= king.Row + 1; row++) {
        for (uint8_t column = king.Column - 1; column <= king.Column + 1;
             column++) {
            if (t.IsType(row, column, PieceType::Guard)) {
                count++;
            }
        }
    }

    return count;
}

uint8_t BfsDistanceToEdge(const Tablut &t, const PiecePosition &p) {
    bool visited[9][9] = {false};
    std::queue<std::pair<PiecePosition, int>> q;

    q.push({p, 0});
    visited[p.Row][p.Column] = true;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    while (!q.empty()) {
        auto [pos, dist] = q.front();
        q.pop();

        if (pos.Row == 0 || pos.Row == 8 || pos.Column == 0 ||
            pos.Column == 8) {
            return static_cast<uint8_t>(dist);
        }

        for (int d = 0; d < 4; d++) {
            int r = pos.Row + dr[d];
            int c = pos.Column + dc[d];

            while (r >= 0 && r < 9 && c >= 0 && c < 9) {
                if (!t.IsEmpty(r, c) || PositionIsUnreachable(r, c)) {
                    break;
                }

                if (!visited[r][c]) {
                    visited[r][c] = true;
                    q.push({PiecePosition(r, c), dist + 1});
                }

                r += dr[d];
                c += dc[d];
            }
        }
    }

    return std::numeric_limits<uint8_t>::max();
}

bool CapturesNextMove(const Tablut &t, bool isWhite) {
    const auto moves = t.GenAllMoves(isWhite);
    const auto currentPieces =
        isWhite ? t.BlackPieces().size() : t.WhitePieces().size();
    for (const auto &[from, to] : moves) {
        const auto next = t.Move(from, to);
        if ((isWhite ? next.BlackPieces().size() : next.WhitePieces().size()) <
            currentPieces) {
            return true;
        }
    }

    return false;
}

int64_t PositionWeigthedWhite(const Tablut &t) {
    static int64_t whitePST[9][9] = {{-20, -10, -5, -5, -5, -5, -5, -10, -20},
                                     {-10, 5, 10, 10, 10, 10, 10, 5, -10},
                                     {-5, 10, 20, 20, 20, 20, 20, 10, -5},
                                     {-5, 10, 20, 30, 30, 30, 20, 10, -5},
                                     {-5, 10, 20, 30, 40, 30, 20, 10, -5},
                                     {-5, 10, 20, 30, 30, 30, 20, 10, -5},
                                     {-5, 10, 20, 20, 20, 20, 20, 10, -5},
                                     {-10, 5, 10, 10, 10, 10, 10, 5, -10},
                                     {-20, -10, -5, -5, -5, -5, -5, -10, -20}};

    int64_t value{0};
    for (const auto piece : t.WhitePieces()) {
        value += whitePST[piece.Position.Row][piece.Position.Column];
    }

    return value;
}

int64_t PositionWeigthedBlack(const Tablut &t) {
    static int64_t blackPST[9][9] = {{40, 30, 20, 20, 20, 20, 20, 30, 40},
                                     {30, 15, 10, 10, 10, 10, 10, 15, 30},
                                     {20, 10, 5, 5, 5, 5, 5, 10, 20},
                                     {20, 10, 5, -5, -5, -5, 5, 10, 20},
                                     {20, 10, 5, -5, -20, -5, 5, 10, 20},
                                     {20, 10, 5, -5, -5, -5, 5, 10, 20},
                                     {20, 10, 5, 5, 5, 5, 5, 10, 20},
                                     {30, 15, 10, 10, 10, 10, 10, 15, 30},
                                     {40, 30, 20, 20, 20, 20, 20, 30, 40}};

    int64_t value{0};
    for (const auto piece : t.WhitePieces()) {
        value += blackPST[piece.Position.Row][piece.Position.Column];
    }

    return value;
}

int64_t PositionWeigthedKing(const Tablut &t) {
    static int64_t kingPST[9][9] = {{0, 60, 40, 30, 30, 30, 40, 60, 0},
                                    {60, 20, 10, 10, 10, 10, 10, 20, 60},
                                    {40, 10, 5, 5, 5, 5, 5, 10, 40},
                                    {30, 10, 5, 0, 0, 0, 5, 10, 30},
                                    {30, 10, 5, 0, -30, 0, 5, 10, 30},
                                    {30, 10, 5, 0, 0, 0, 5, 10, 30},
                                    {40, 10, 5, 5, 5, 5, 5, 10, 40},
                                    {60, 20, 10, 10, 10, 10, 10, 20, 60},
                                    {0, 60, 40, 30, 30, 30, 40, 60, 0}};

    return kingPST[t.King().Position.Row][t.King().Position.Column];
}
