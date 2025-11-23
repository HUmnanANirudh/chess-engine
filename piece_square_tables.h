#ifndef PIECE_SQUARE_TABLES_H
#define PIECE_SQUARE_TABLES_H

#include <array>

// Piece-square tables for positional evaluation
class PieceSquareTables {
public:
    // Pawn piece-square table
    static constexpr std::array<int, 64> pawn_table = {{
        0,   0,   0,   0,   0,   0,   0,   0,
        50,  50,  50,  50,  50,  50,  50,  50,
        10,  10,  20,  30,  30,  20,  10,  10,
        5,   5,   10,  25,  25,  10,  5,   5,
        0,   0,   0,   20,  20,  0,   0,   0,
        5,   -5,  -10, 0,   0,   -10, -5,  5,
        5,   10,  10,  -20, -20, 10,  10,  5,
        0,   0,   0,   0,   0,   0,   0,   0
    }};

    // Knight piece-square table
    static constexpr std::array<int, 64> knight_table = {{
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0,   0,   0,   0,   -20, -40,
        -30, 0,   10,  15,  15,  10,  0,   -30,
        -30, 5,   15,  20,  20,  15,  5,   -30,
        -30, 0,   15,  20,  20,  15,  0,   -30,
        -30, 5,   10,  15,  15,  10,  5,   -30,
        -40, -20, 0,   5,   5,   0,   -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50
    }};

    // Bishop piece-square table
    static constexpr std::array<int, 64> bishop_table = {{
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0,   0,   0,   0,   0,   0,   -10,
        -10, 0,   5,   10,  10,  5,   0,   -10,
        -10, 5,   5,   10,  10,  5,   5,   -10,
        -10, 0,   10,  10,  10,  10,  0,   -10,
        -10, 10,  10,  10,  10,  10,  10,  -10,
        -10, 5,   0,   0,   0,   0,   5,   -10,
        -20, -10, -10, -10, -10, -10, -10, -20
    }};

    // Rook piece-square table
    static constexpr std::array<int, 64> rook_table = {{
        0,   0,   0,   0,   0,   0,   0,   0,
        5,   10,  10,  10,  10,  10,  10,  5,
        -5,  0,   0,   0,   0,   0,   0,   -5,
        -5,  0,   0,   0,   0,   0,   0,   -5,
        -5,  0,   0,   0,   0,   0,   0,   -5,
        -5,  0,   0,   0,   0,   0,   0,   -5,
        -5,  0,   0,   0,   0,   0,   0,   -5,
        0,   0,   0,   5,   5,   0,   0,   0
    }};

    // Queen piece-square table
    static constexpr std::array<int, 64> queen_table = {{
        -20, -10, -10, -5,  -5,  -10, -10, -20,
        -10, 0,   0,   0,   0,   0,   0,   -10,
        -10, 0,   5,   5,   5,   5,   0,   -10,
        -5,  0,   5,   5,   5,   5,   0,   -5,
        0,   0,   5,   5,   5,   5,   0,   -5,
        -10, 5,   5,   5,   5,   5,   0,   -10,
        -10, 0,   5,   0,   0,   0,   0,   -10,
        -20, -10, -10, -5,  -5,  -10, -10, -20
    }};

    // King piece-square table (middlegame)
    static constexpr std::array<int, 64> king_table_mg = {{
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20,  20,  0,   0,   0,   0,   20,  20,
        20,  30,  10,  0,   0,   10,  30,  20
    }};

    // King piece-square table (endgame)
    static constexpr std::array<int, 64> king_table_eg = {{
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10, 0,   0,   -10, -20, -30,
        -30, -10, 20,  30,  30,  20,  -10, -30,
        -30, -10, 30,  40,  40,  30,  -10, -30,
        -30, -10, 30,  40,  40,  30,  -10, -30,
        -30, -10, 20,  30,  30,  20,  -10, -30,
        -30, -30, 0,   0,   0,   0,   -30, -30,
        -50, -30, -30, -30, -30, -30, -30, -50
    }};

    static int get_pst_value(int piece_type, int square, bool is_white) {
        if (!is_white) square = 63 - square;  // Flip for black
        
        switch (piece_type) {
            case 1: return pawn_table[square];
            case 2: return knight_table[square];
            case 3: return bishop_table[square];
            case 4: return rook_table[square];
            case 5: return queen_table[square];
            case 6: return king_table_mg[square];
            default: return 0;
        }
    }
};

#endif // PIECE_SQUARE_TABLES_H
