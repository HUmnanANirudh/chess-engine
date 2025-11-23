#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <vector>
#include <string>

typedef uint64_t Bitboard;

// Bitboard utility functions
class BitboardUtils {
public:
    // Bit manipulation
    static int popcount(Bitboard b) {
        return __builtin_popcountll(b);
    }

    static int lsb(Bitboard b) {
        return __builtin_ctzll(b);
    }

    static Bitboard lsb_board(Bitboard b) {
        return b & -b;
    }

    static Bitboard pop_lsb(Bitboard& b) {
        Bitboard lsb = b & -b;
        b ^= lsb;
        return lsb;
    }

    // Square operations
    static Bitboard square_to_board(int sq) {
        return 1ULL << sq;
    }

    static int board_to_square(Bitboard b) {
        return lsb(b);
    }

    // Coordinate conversion
    static int coords_to_square(int row, int col) {
        return row * 8 + col;
    }

    static void square_to_coords(int sq, int& row, int& col) {
        row = sq / 8;
        col = sq % 8;
    }

    static std::string square_to_notation(int sq) {
        int row = sq / 8;
        int col = sq % 8;
        std::string result;
        result += char('a' + col);
        result += char('8' - row);
        return result;
    }

    static int notation_to_square(const std::string& notation) {
        if (notation.length() < 2) return -1;
        int col = notation[0] - 'a';
        int row = 8 - (notation[1] - '0');
        if (col < 0 || col > 7 || row < 0 || row > 7) return -1;
        return coords_to_square(row, col);
    }

    // Bitboard printing
    static void print(Bitboard b) {
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                int sq = coords_to_square(row, col);
                std::cout << ((b & square_to_board(sq)) ? "1 " : ". ");
            }
            std::cout << "\n";
        }
    }
};

#endif // BITBOARD_H
