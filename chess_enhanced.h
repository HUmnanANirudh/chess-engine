#ifndef CHESS_ENHANCED_H
#define CHESS_ENHANCED_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include "bitboard.h"
#include "piece_square_tables.h"

struct GameMove {
    int from, to;
    int promotion = 0;
    std::string notation;
    
    GameMove() : from(-1), to(-1) {}
    GameMove(int f, int t) : from(f), to(t) {}
    
    std::string to_algebraic() const {
        return BitboardUtils::square_to_notation(from) + 
               BitboardUtils::square_to_notation(to);
    }
};

struct MoveAnalysis {
    GameMove move;
    int eval_score;
    double probability;
    bool is_top_move;
};

class ChessBoardEnhanced {
private:
    // Bitboards for each piece type
    Bitboard white_pawns, white_knights, white_bishops, white_rooks, white_queens, white_king;
    Bitboard black_pawns, black_knights, black_bishops, black_rooks, black_queens, black_king;
    
    bool white_to_move;
    int halfmove_clock;
    int fullmove_number;
    std::vector<GameMove> move_history;
    
public:
    ChessBoardEnhanced();
    
    void initialize();
    void make_move(const GameMove& move);
    void unmake_move(const GameMove& move);
    
    std::vector<GameMove> generate_legal_moves();
    std::vector<GameMove> generate_pseudo_legal_moves();
    bool is_legal_move(const GameMove& move);
    
    bool is_in_check();
    bool is_checkmate();
    bool is_stalemate();
    
    int evaluate_position();
    int evaluate_material();
    int evaluate_pst();
    
    bool is_white_to_move() const { return white_to_move; }
    void switch_player() { white_to_move = !white_to_move; }
    
    std::string to_fen() const;
    std::string to_pgn() const;
    
    Bitboard get_occupied() const;
    Bitboard get_white_pieces() const;
    Bitboard get_black_pieces() const;
    
    int get_piece_at(int square) const;
    void set_piece_at(int square, int piece, bool is_white);
    
    const std::vector<GameMove>& get_move_history() const { return move_history; }
};

class ChessAIEnhanced {
private:
    int search_depth;
    int analysis_depth;
    long long nodes_evaluated;
    std::map<std::string, MoveAnalysis> move_analyses;
    
    int minimax(ChessBoardEnhanced& board, int depth, int alpha, int beta, bool is_maximizing);
    int quiescence(ChessBoardEnhanced& board, int alpha, int beta, bool is_maximizing);
    
public:
    ChessAIEnhanced(int depth = 5, int analysis_depth = 7);
    
    GameMove find_best_move(ChessBoardEnhanced& board);
    void analyze_game(ChessBoardEnhanced& board);
    void analyze_position(ChessBoardEnhanced& board, const GameMove& human_move);
    
    const std::map<std::string, MoveAnalysis>& get_analyses() const { return move_analyses; }
    long long get_nodes_evaluated() const { return nodes_evaluated; }
    
    void print_game_analysis();
};

#endif // CHESS_ENHANCED_H
