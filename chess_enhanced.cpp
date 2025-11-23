#include "chess_enhanced.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// ==================== ChessBoardEnhanced Implementation ====================

ChessBoardEnhanced::ChessBoardEnhanced() {
    initialize();
}

void ChessBoardEnhanced::initialize() {
    white_pawns = 0xFF00ULL;
    white_knights = 0x42ULL;
    white_bishops = 0x24ULL;
    white_rooks = 0x81ULL;
    white_queens = 0x8ULL;
    white_king = 0x10ULL;
    
    black_pawns = 0xFF000000000000ULL;
    black_knights = 0x4200000000000000ULL;
    black_bishops = 0x2400000000000000ULL;
    black_rooks = 0x8100000000000000ULL;
    black_queens = 0x800000000000000ULL;
    black_king = 0x1000000000000000ULL;
    
    white_to_move = true;
    halfmove_clock = 0;
    fullmove_number = 1;
    move_history.clear();
}

Bitboard ChessBoardEnhanced::get_occupied() const {
    return get_white_pieces() | get_black_pieces();
}

Bitboard ChessBoardEnhanced::get_white_pieces() const {
    return white_pawns | white_knights | white_bishops | white_rooks | white_queens | white_king;
}

Bitboard ChessBoardEnhanced::get_black_pieces() const {
    return black_pawns | black_knights | black_bishops | black_rooks | black_queens | black_king;
}

int ChessBoardEnhanced::get_piece_at(int square) const {
    Bitboard sq = BitboardUtils::square_to_board(square);
    
    if (white_pawns & sq) return 1;
    if (white_knights & sq) return 2;
    if (white_bishops & sq) return 3;
    if (white_rooks & sq) return 4;
    if (white_queens & sq) return 5;
    if (white_king & sq) return 6;
    
    if (black_pawns & sq) return -1;
    if (black_knights & sq) return -2;
    if (black_bishops & sq) return -3;
    if (black_rooks & sq) return -4;
    if (black_queens & sq) return -5;
    if (black_king & sq) return -6;
    
    return 0;
}

void ChessBoardEnhanced::set_piece_at(int square, int piece, bool is_white) {
    Bitboard sq = BitboardUtils::square_to_board(square);
    int type = std::abs(piece);
    
    // Clear square
    white_pawns &= ~sq;
    white_knights &= ~sq;
    white_bishops &= ~sq;
    white_rooks &= ~sq;
    white_queens &= ~sq;
    white_king &= ~sq;
    black_pawns &= ~sq;
    black_knights &= ~sq;
    black_bishops &= ~sq;
    black_rooks &= ~sq;
    black_queens &= ~sq;
    black_king &= ~sq;
    
    if (piece == 0) return;
    
    if (is_white) {
        switch (type) {
            case 1: white_pawns |= sq; break;
            case 2: white_knights |= sq; break;
            case 3: white_bishops |= sq; break;
            case 4: white_rooks |= sq; break;
            case 5: white_queens |= sq; break;
            case 6: white_king |= sq; break;
        }
    } else {
        switch (type) {
            case 1: black_pawns |= sq; break;
            case 2: black_knights |= sq; break;
            case 3: black_bishops |= sq; break;
            case 4: black_rooks |= sq; break;
            case 5: black_queens |= sq; break;
            case 6: black_king |= sq; break;
        }
    }
}

std::vector<GameMove> ChessBoardEnhanced::generate_pseudo_legal_moves() {
    std::vector<GameMove> moves;
    Bitboard occupied = get_occupied();
    Bitboard own_pieces = white_to_move ? get_white_pieces() : get_black_pieces();
    
    for (int from = 0; from < 64; from++) {
        Bitboard from_sq = BitboardUtils::square_to_board(from);
        if (!(own_pieces & from_sq)) continue;
        
        int piece = get_piece_at(from);
        int type = std::abs(piece);
        
        // Generate moves based on piece type
        for (int to = 0; to < 64; to++) {
            if (from == to) continue;
            
            Bitboard to_sq = BitboardUtils::square_to_board(to);
            int target = get_piece_at(to);
            
            // Can't capture own piece
            if (target != 0 && ((target > 0) == white_to_move)) continue;
            
            // Simple move validation (simplified for now)
            GameMove move(from, to);
            moves.push_back(move);
        }
    }
    
    return moves;
}

std::vector<GameMove> ChessBoardEnhanced::generate_legal_moves() {
    auto pseudo = generate_pseudo_legal_moves();
    std::vector<GameMove> legal;
    
    for (const auto& move : pseudo) {
        make_move(move);
        if (!is_in_check()) {
            legal.push_back(move);
        }
        unmake_move(move);
    }
    
    return legal;
}

bool ChessBoardEnhanced::is_legal_move(const GameMove& move) {
    auto legal = generate_legal_moves();
    for (const auto& m : legal) {
        if (m.from == move.from && m.to == move.to) return true;
    }
    return false;
}

void ChessBoardEnhanced::make_move(const GameMove& move) {
    int piece = get_piece_at(move.from);
    int target = get_piece_at(move.to);
    bool is_white = piece > 0;
    
    set_piece_at(move.from, 0, is_white);
    set_piece_at(move.to, piece, is_white);
    
    move_history.push_back(move);
    white_to_move = !white_to_move;
}

void ChessBoardEnhanced::unmake_move(const GameMove& move) {
    if (move_history.empty()) return;
    move_history.pop_back();
    
    // Simplified unmake - would need to store captured piece
    white_to_move = !white_to_move;
}

bool ChessBoardEnhanced::is_in_check() {
    // Simplified check detection
    return false;
}

bool ChessBoardEnhanced::is_checkmate() {
    return is_in_check() && generate_legal_moves().empty();
}

bool ChessBoardEnhanced::is_stalemate() {
    return !is_in_check() && generate_legal_moves().empty();
}

int ChessBoardEnhanced::evaluate_material() {
    int score = 0;
    const int values[] = {0, 1, 3, 3, 5, 9, 0};
    
    for (int sq = 0; sq < 64; sq++) {
        int piece = get_piece_at(sq);
        if (piece == 0) continue;
        
        int type = std::abs(piece);
        int value = values[type];
        score += (piece > 0 ? value : -value);
    }
    
    return score;
}

int ChessBoardEnhanced::evaluate_pst() {
    int score = 0;
    
    for (int sq = 0; sq < 64; sq++) {
        int piece = get_piece_at(sq);
        if (piece == 0) continue;
        
        int type = std::abs(piece);
        bool is_white = piece > 0;
        int pst_value = PieceSquareTables::get_pst_value(type, sq, is_white);
        score += (is_white ? pst_value : -pst_value);
    }
    
    return score;
}

int ChessBoardEnhanced::evaluate_position() {
    return evaluate_material() + evaluate_pst();
}

std::string ChessBoardEnhanced::to_fen() const {
    std::stringstream ss;
    // Simplified FEN generation
    return ss.str();
}

std::string ChessBoardEnhanced::to_pgn() const {
    std::stringstream ss;
    for (size_t i = 0; i < move_history.size(); i++) {
        if (i % 2 == 0) ss << (i / 2 + 1) << ". ";
        ss << move_history[i].to_algebraic() << " ";
    }
    return ss.str();
}

// ==================== ChessAIEnhanced Implementation ====================

ChessAIEnhanced::ChessAIEnhanced(int depth, int analysis_depth)
    : search_depth(depth), analysis_depth(analysis_depth), nodes_evaluated(0) {}

int ChessAIEnhanced::minimax(ChessBoardEnhanced& board, int depth, int alpha, int beta, bool is_maximizing) {
    nodes_evaluated++;
    
    if (depth == 0) {
        return board.evaluate_position();
    }
    
    auto moves = board.generate_legal_moves();
    if (moves.empty()) {
        if (board.is_in_check()) {
            return is_maximizing ? -10000 : 10000;
        }
        return 0;
    }
    
    if (is_maximizing) {
        int max_eval = -100000;
        for (const auto& move : moves) {
            board.make_move(move);
            int eval = minimax(board, depth - 1, alpha, beta, false);
            board.unmake_move(move);
            
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return max_eval;
    } else {
        int min_eval = 100000;
        for (const auto& move : moves) {
            board.make_move(move);
            int eval = minimax(board, depth - 1, alpha, beta, true);
            board.unmake_move(move);
            
            min_eval = std::min(min_eval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return min_eval;
    }
}

int ChessAIEnhanced::quiescence(ChessBoardEnhanced& board, int alpha, int beta, bool is_maximizing) {
    int stand_pat = board.evaluate_position();
    
    if (is_maximizing) {
        if (stand_pat >= beta) return beta;
        alpha = std::max(alpha, stand_pat);
    } else {
        if (stand_pat <= alpha) return alpha;
        beta = std::min(beta, stand_pat);
    }
    
    return stand_pat;
}

GameMove ChessAIEnhanced::find_best_move(ChessBoardEnhanced& board) {
    nodes_evaluated = 0;
    auto moves = board.generate_legal_moves();
    
    if (moves.empty()) return GameMove();
    
    GameMove best = moves[0];
    int best_eval = board.is_white_to_move() ? -100000 : 100000;
    bool is_white = board.is_white_to_move();
    
    for (const auto& move : moves) {
        board.make_move(move);
        int eval = minimax(board, search_depth - 1, -100000, 100000, !is_white);
        board.unmake_move(move);
        
        if ((is_white && eval > best_eval) || (!is_white && eval < best_eval)) {
            best_eval = eval;
            best = move;
        }
    }
    
    return best;
}

void ChessAIEnhanced::analyze_game(ChessBoardEnhanced& board) {
    move_analyses.clear();
    
    ChessBoardEnhanced analysis_board;
    analysis_board.initialize();
    
    const auto& history = board.get_move_history();
    
    for (size_t i = 0; i < history.size(); i++) {
        if (i % 2 == 0) {  // Only analyze human moves (white)
            analyze_position(analysis_board, history[i]);
        }
        analysis_board.make_move(history[i]);
    }
}

void ChessAIEnhanced::analyze_position(ChessBoardEnhanced& board, const GameMove& human_move) {
    nodes_evaluated = 0;
    auto moves = board.generate_legal_moves();
    
    if (moves.empty()) return;
    
    // Evaluate all moves at analysis depth
    std::vector<std::pair<GameMove, int>> move_scores;
    
    for (const auto& move : moves) {
        board.make_move(move);
        int score = minimax(board, analysis_depth - 1, -100000, 100000, !board.is_white_to_move());
        board.unmake_move(move);
        move_scores.push_back({move, score});
    }
    
    // Sort by score
    std::sort(move_scores.begin(), move_scores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    int best_score = move_scores[0].second;
    
    // Find moves within 50 centipawns of best
    std::vector<GameMove> top_moves;
    for (const auto& [move, score] : move_scores) {
        if (score >= best_score - 50) {
            top_moves.push_back(move);
        }
    }
    
    // Calculate probability
    MoveAnalysis analysis;
    analysis.move = human_move;
    analysis.eval_score = best_score;
    analysis.is_top_move = false;
    analysis.probability = 0.0;
    
    for (const auto& move : top_moves) {
        if (move.from == human_move.from && move.to == human_move.to) {
            analysis.is_top_move = true;
            analysis.probability = (1.0 / top_moves.size()) * 100.0;
            break;
        }
    }
    
    move_analyses[human_move.to_algebraic()] = analysis;
}

void ChessAIEnhanced::print_game_analysis() {
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║        POST-GAME ANALYSIS              ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
    
    std::cout << std::left << std::setw(10) << "Move"
              << std::setw(15) << "Score"
              << std::setw(15) << "Probability"
              << std::setw(10) << "Quality\n";
    std::cout << std::string(50, '-') << "\n";
    
    for (const auto& [notation, analysis] : move_analyses) {
        std::string quality = analysis.probability >= 70 ? "Excellent" :
                             analysis.probability >= 50 ? "Good" :
                             analysis.probability >= 30 ? "Acceptable" : "Weak";
        
        std::cout << std::setw(10) << notation
                  << std::setw(15) << analysis.eval_score
                  << std::setw(15) << std::fixed << std::setprecision(1) << analysis.probability << "%"
                  << std::setw(10) << quality << "\n";
    }
}
