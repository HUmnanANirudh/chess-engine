#include "chess.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// ==================== ChessBoard Implementation ====================

ChessBoard::ChessBoard() : currentPlayer(WHITE), moveCount(0) {
    initializeBoard();
}

void ChessBoard::initializeBoard() {
    // Clear board
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = Piece();
        }
    }
    
    // Set up black pieces
    board[0][0] = Piece(ROOK, BLACK);
    board[0][1] = Piece(KNIGHT, BLACK);
    board[0][2] = Piece(BISHOP, BLACK);
    board[0][3] = Piece(QUEEN, BLACK);
    board[0][4] = Piece(KING, BLACK);
    board[0][5] = Piece(BISHOP, BLACK);
    board[0][6] = Piece(KNIGHT, BLACK);
    board[0][7] = Piece(ROOK, BLACK);
    
    for (int i = 0; i < 8; i++) {
        board[1][i] = Piece(PAWN, BLACK);
    }
    
    // Set up white pieces
    for (int i = 0; i < 8; i++) {
        board[6][i] = Piece(PAWN, WHITE);
    }
    
    board[7][0] = Piece(ROOK, WHITE);
    board[7][1] = Piece(KNIGHT, WHITE);
    board[7][2] = Piece(BISHOP, WHITE);
    board[7][3] = Piece(QUEEN, WHITE);
    board[7][4] = Piece(KING, WHITE);
    board[7][5] = Piece(BISHOP, WHITE);
    board[7][6] = Piece(KNIGHT, WHITE);
    board[7][7] = Piece(ROOK, WHITE);
    
    currentPlayer = WHITE;
    moveCount = 0;
}

Piece ChessBoard::getPiece(const Position& pos) const {
    if (!pos.isValid()) return Piece();
    return board[pos.row][pos.col];
}

void ChessBoard::setPiece(const Position& pos, const Piece& piece) {
    if (pos.isValid()) {
        board[pos.row][pos.col] = piece;
    }
}

void ChessBoard::movePiece(const Move& move) {
    Piece piece = getPiece(move.from);
    setPiece(move.to, piece);
    setPiece(move.from, Piece());
    
    // Handle pawn promotion
    if (piece.type == PAWN && move.promotionPiece != EMPTY) {
        setPiece(move.to, Piece(move.promotionPiece, piece.color));
    }
    
    moveCount++;
}

void ChessBoard::undoMove(const Move& move, const Piece& capturedPiece) {
    Piece piece = getPiece(move.to);
    setPiece(move.from, piece);
    setPiece(move.to, capturedPiece);
    moveCount--;
}

Position ChessBoard::findKing(Color color) const {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece p = board[i][j];
            if (p.type == KING && p.color == color) {
                return Position(i, j);
            }
        }
    }
    return Position(-1, -1);
}

bool ChessBoard::isSquareAttacked(const Position& pos, Color byColor) {
    // Check if square is attacked by pieces of given color
    int r = pos.row;
    int c = pos.col;

    // Pawn attacks
    int pawnDir = (byColor == WHITE) ? 1 : -1; // white pawns move up (towards lower row index), so they sit below the attacked square
    int pawnRow = r + pawnDir;
    if (pawnRow >= 0 && pawnRow < 8) {
        for (int dc = -1; dc <= 1; dc += 2) {
            int pc = c + dc;
            if (pc >= 0 && pc < 8) {
                Piece p = board[pawnRow][pc];
                if (!p.isEmpty() && p.color == byColor && p.type == PAWN) {
                    return true;
                }
            }
        }
    }

    // Knight attacks
    static const int knightMoves[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2},  {1, 2},  {2, -1},  {2, 1}
    };
    for (const auto& km : knightMoves) {
        int nr = r + km[0];
        int nc = c + km[1];
        if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
            Piece p = board[nr][nc];
            if (!p.isEmpty() && p.color == byColor && p.type == KNIGHT) {
                return true;
            }
        }
    }

    // Bishop / Queen diagonal attacks
    static const int bishopDirs[4][2] = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };
    for (const auto& dir : bishopDirs) {
        int nr = r + dir[0];
        int nc = c + dir[1];
        while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
            Piece p = board[nr][nc];
            if (!p.isEmpty()) {
                if (p.color == byColor && (p.type == BISHOP || p.type == QUEEN)) {
                    return true;
                }
                break;
            }
            nr += dir[0];
            nc += dir[1];
        }
    }

    // Rook / Queen horizontal and vertical attacks
    static const int rookDirs[4][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };
    for (const auto& dir : rookDirs) {
        int nr = r + dir[0];
        int nc = c + dir[1];
        while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
            Piece p = board[nr][nc];
            if (!p.isEmpty()) {
                if (p.color == byColor && (p.type == ROOK || p.type == QUEEN)) {
                    return true;
                }
                break;
            }
            nr += dir[0];
            nc += dir[1];
        }
    }

    // King attacks
    static const int kingDirs[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    for (const auto& dir : kingDirs) {
        int nr = r + dir[0];
        int nc = c + dir[1];
        if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
            Piece p = board[nr][nc];
            if (!p.isEmpty() && p.color == byColor && p.type == KING) {
                return true;
            }
        }
    }

    return false;
}

std::vector<Move> ChessBoard::generatePseudoLegalMoves() {
    std::vector<Move> moves;
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece piece = board[i][j];
            if (piece.isEmpty() || piece.color != currentPlayer) continue;
            
            Position from(i, j);
            
            switch (piece.type) {
                case PAWN: {
                    int direction = (piece.color == WHITE) ? -1 : 1;
                    int startRow = (piece.color == WHITE) ? 6 : 1;
                    
                    // Forward move
                    Position forward(i + direction, j);
                    if (forward.isValid() && getPiece(forward).isEmpty()) {
                        moves.push_back(Move(from, forward));
                        
                        // Double move from start
                        if (i == startRow) {
                            Position doubleForward(i + 2 * direction, j);
                            if (getPiece(doubleForward).isEmpty()) {
                                moves.push_back(Move(from, doubleForward));
                            }
                        }
                    }
                    
                    // Captures
                    for (int dc = -1; dc <= 1; dc += 2) {
                        Position capture(i + direction, j + dc);
                        if (capture.isValid()) {
                            Piece target = getPiece(capture);
                            if (!target.isEmpty() && target.color != piece.color) {
                                moves.push_back(Move(from, capture));
                            }
                        }
                    }
                    break;
                }
                
                case KNIGHT: {
                    int knightMoves[8][2] = {
                        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                        {1, -2}, {1, 2}, {2, -1}, {2, 1}
                    };
                    for (auto& km : knightMoves) {
                        Position to(i + km[0], j + km[1]);
                        if (to.isValid()) {
                            Piece target = getPiece(to);
                            if (target.isEmpty() || target.color != piece.color) {
                                moves.push_back(Move(from, to));
                            }
                        }
                    }
                    break;
                }
                
                case BISHOP:
                case ROOK:
                case QUEEN: {
                    int bishopDirs[4][2] = {
                        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
                    };
                    int rookDirs[4][2] = {
                        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
                    };

                    auto addSliding = [&](int dirs[][2], int dirCount) {
                        for (int d = 0; d < dirCount; ++d) {
                            for (int dist = 1; dist < 8; ++dist) {
                                Position to(i + dirs[d][0] * dist, j + dirs[d][1] * dist);
                                if (!to.isValid()) break;
                                Piece target = getPiece(to);
                                if (target.isEmpty()) {
                                    moves.push_back(Move(from, to));
                                } else {
                                    if (target.color != piece.color) {
                                        moves.push_back(Move(from, to));
                                    }
                                    break;
                                }
                            }
                        }
                    };

                    if (piece.type == BISHOP) {
                        addSliding(bishopDirs, 4);
                    } else if (piece.type == ROOK) {
                        addSliding(rookDirs, 4);
                    } else { // QUEEN
                        addSliding(bishopDirs, 4);
                        addSliding(rookDirs, 4);
                    }
                    break;
                }
                
                case KING: {
                    int directions[8][2] = {
                        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                        {0, 1}, {1, -1}, {1, 0}, {1, 1}
                    };
                    for (auto& dir : directions) {
                        Position to(i + dir[0], j + dir[1]);
                        if (to.isValid()) {
                            Piece target = getPiece(to);
                            if (target.isEmpty() || target.color != piece.color) {
                                moves.push_back(Move(from, to));
                            }
                        }
                    }
                    break;
                }
                
                default:
                    break;
            }
        }
    }
    
    return moves;
}

bool ChessBoard::isInCheck(Color color) {
    Position kingPos = findKing(color);
    if (!kingPos.isValid()) return false;
    
    Color enemyColor = (Color)(1 - color);
    return isSquareAttacked(kingPos, enemyColor);
}

std::vector<Move> ChessBoard::generateLegalMoves() {
    std::vector<Move> pseudoLegal = generatePseudoLegalMoves();
    std::vector<Move> legal;
    
    for (const auto& move : pseudoLegal) {
        Piece captured = getPiece(move.to);
        movePiece(move);
        switchPlayer();
        
        if (!isInCheck(currentPlayer == WHITE ? BLACK : WHITE)) {
            legal.push_back(move);
        }
        
        switchPlayer();
        undoMove(move, captured);
    }
    
    return legal;
}

bool ChessBoard::isLegalMove(const Move& move) {
    std::vector<Move> legal = generateLegalMoves();
    for (const auto& m : legal) {
        if (m.from == move.from && m.to == move.to) {
            return true;
        }
    }
    return false;
}

bool ChessBoard::isCheckmate(Color color) {
    if (!isInCheck(color)) return false;
    
    Color temp = currentPlayer;
    currentPlayer = color;
    std::vector<Move> moves = generateLegalMoves();
    currentPlayer = temp;
    
    return moves.empty();
}

bool ChessBoard::isStalemate(Color color) {
    if (isInCheck(color)) return false;
    
    Color temp = currentPlayer;
    currentPlayer = color;
    std::vector<Move> moves = generateLegalMoves();
    currentPlayer = temp;
    
    return moves.empty();
}

int ChessBoard::evaluateBoard() const {
    int score = 0;
    int pieceValues[] = {0, 100, 300, 300, 500, 900, 0};
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece p = board[i][j];
            if (p.isEmpty()) continue;
            
            int value = pieceValues[p.type];
            if (p.color == WHITE) {
                score += value;
            } else {
                score -= value;
            }
        }
    }
    
    return score;
}

std::string ChessBoard::boardToString() const {
    std::stringstream ss;
    ss << "\n  a b c d e f g h\n";
    
    for (int i = 0; i < 8; i++) {
        ss << (8 - i) << " ";
        for (int j = 0; j < 8; j++) {
            Piece p = board[i][j];
            char c = ' ';
            
            if (!p.isEmpty()) {
                switch (p.type) {
                    case PAWN: c = 'P'; break;
                    case KNIGHT: c = 'N'; break;
                    case BISHOP: c = 'B'; break;
                    case ROOK: c = 'R'; break;
                    case QUEEN: c = 'Q'; break;
                    case KING: c = 'K'; break;
                    default: break;
                }
                
                if (p.color == BLACK) c = tolower(c);
            }
            
            ss << c << " ";
        }
        ss << (8 - i) << "\n";
    }
    
    ss << "  a b c d e f g h\n";
    return ss.str();
}

// ==================== ChessAI Implementation ====================

ChessAI::ChessAI(int depth) : maxDepth(depth), nodesEvaluated(0) {}

int ChessAI::minimax(ChessBoard& board, int depth, int alpha, int beta, bool isMaximizing) {
    nodesEvaluated++;
    
    if (depth == 0) {
        return board.evaluateBoard();
    }
    
    std::vector<Move> moves = board.generateLegalMoves();
    
    if (moves.empty()) {
        if (board.isInCheck(board.getCurrentPlayer())) {
            return isMaximizing ? -10000 : 10000;
        }
        return 0;
    }
    
    if (isMaximizing) {
        int maxEval = -100000;
        for (const auto& move : moves) {
            Piece captured = board.getPiece(move.to);
            board.movePiece(move);
            board.switchPlayer();
            
            int eval = minimax(board, depth - 1, alpha, beta, false);
            
            board.switchPlayer();
            board.undoMove(move, captured);
            
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = 100000;
        for (const auto& move : moves) {
            Piece captured = board.getPiece(move.to);
            board.movePiece(move);
            board.switchPlayer();
            
            int eval = minimax(board, depth - 1, alpha, beta, true);
            
            board.switchPlayer();
            board.undoMove(move, captured);
            
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

Move ChessAI::findBestMove(ChessBoard& board) {
    nodesEvaluated = 0;
    std::vector<Move> moves = board.generateLegalMoves();
    
    if (moves.empty()) return Move();
    
    Move bestMove = moves[0];
    int bestEval = (board.getCurrentPlayer() == WHITE) ? -100000 : 100000;
    bool isMaximizing = (board.getCurrentPlayer() == WHITE);
    
    for (const auto& move : moves) {
        Piece captured = board.getPiece(move.to);
        board.movePiece(move);
        board.switchPlayer();
        
        int eval = minimax(board, maxDepth - 1, -100000, 100000, !isMaximizing);
        
        board.switchPlayer();
        board.undoMove(move, captured);
        
        if ((isMaximizing && eval > bestEval) || (!isMaximizing && eval < bestEval)) {
            bestEval = eval;
            bestMove = move;
        }
    }
    
    return bestMove;
}

std::vector<std::pair<Move, int>> ChessAI::evaluateAllMoves(ChessBoard& board, int depth) {
    nodesEvaluated = 0;
    std::vector<std::pair<Move, int>> result;
    std::vector<Move> moves = board.generateLegalMoves();

    if (moves.empty()) {
        return result;
    }

    bool isMaximizing = (board.getCurrentPlayer() == WHITE);

    for (const auto& move : moves) {
        Piece captured = board.getPiece(move.to);
        board.movePiece(move);
        board.switchPlayer();

        int eval = minimax(board, depth - 1, -1000000, 1000000, !isMaximizing);

        board.switchPlayer();
        board.undoMove(move, captured);

        result.push_back(std::make_pair(move, eval));
    }

    return result;
}

void ChessAI::analyzeMovesProbability(ChessBoard& board, int depth) {
    moveProbabilities.clear();
    std::vector<Move> moves = board.generateLegalMoves();
    
    for (const auto& move : moves) {
        Piece captured = board.getPiece(move.to);
        board.movePiece(move);
        board.switchPlayer();
        
        int score = minimax(board, depth - 1, -100000, 100000, board.getCurrentPlayer() == WHITE);
        
        board.switchPlayer();
        board.undoMove(move, captured);
        
        std::string moveStr = move.toAlgebraic();
        moveProbabilities[moveStr] = {1, score};
    }
}

void ChessAI::printMoveAnalysis() {
    std::cout << "\n=== Move Analysis ===\n";
    std::cout << std::left << std::setw(10) << "Move" << std::setw(15) << "Score" << "\n";
    std::cout << std::string(25, '-') << "\n";
    
    for (const auto& pair : moveProbabilities) {
        std::cout << std::setw(10) << pair.first 
                  << std::setw(15) << pair.second.second << "\n";
    }
}
