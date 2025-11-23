#ifndef CHESS_H
#define CHESS_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>

// Piece types
enum PieceType { EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

// Colors
enum Color { WHITE = 0, BLACK = 1 };

// Piece structure
struct Piece {
    PieceType type;
    Color color;
    
    Piece() : type(EMPTY), color(WHITE) {}
    Piece(PieceType t, Color c) : type(t), color(c) {}
    
    bool isEmpty() const { return type == EMPTY; }
};

// Position structure
struct Position {
    int row, col;
    Position() : row(-1), col(-1) {}
    Position(int r, int c) : row(r), col(c) {}
    
    bool isValid() const {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }
    
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

// Move structure
struct Move {
    Position from, to;
    PieceType promotionPiece;
    
    Move() : promotionPiece(EMPTY) {}
    Move(Position f, Position t) : from(f), to(t), promotionPiece(EMPTY) {}
    
    std::string toAlgebraic() const {
        std::string result;
        result += char('a' + from.col);
        result += char('8' - from.row);
        result += char('a' + to.col);
        result += char('8' - to.row);
        return result;
    }
};

// Chess Board
class ChessBoard {
private:
    Piece board[8][8];
    Color currentPlayer;
    int moveCount;
    
public:
    ChessBoard();
    
    void initializeBoard();
    Piece getPiece(const Position& pos) const;
    void setPiece(const Position& pos, const Piece& piece);
    void movePiece(const Move& move);
    void undoMove(const Move& move, const Piece& capturedPiece);
    
    Color getCurrentPlayer() const { return currentPlayer; }
    void switchPlayer() { currentPlayer = (Color)(1 - currentPlayer); }
    
    std::vector<Move> generateLegalMoves();
    std::vector<Move> generatePseudoLegalMoves();
    bool isLegalMove(const Move& move);
    bool isInCheck(Color color);
    bool isCheckmate(Color color);
    bool isStalemate(Color color);
    
    int evaluateBoard() const;
    std::string boardToString() const;
    
    Position findKing(Color color) const;
    bool isSquareAttacked(const Position& pos, Color byColor);
};

// AI Engine with Minimax and Alpha-Beta Pruning
class ChessAI {
private:
    int maxDepth;
    int nodesEvaluated;
    std::map<std::string, std::pair<int, int>> moveProbabilities; // move -> (count, score)
    
    int minimax(ChessBoard& board, int depth, int alpha, int beta, bool isMaximizing);
    int evaluatePosition(const ChessBoard& board);
    
public:
    ChessAI(int depth = 4);
    
    Move findBestMove(ChessBoard& board);
    std::vector<std::pair<Move, int>> evaluateAllMoves(ChessBoard& board, int depth);
    void analyzeMovesProbability(ChessBoard& board, int depth);
    void printMoveAnalysis();
    
    int getNodesEvaluated() const { return nodesEvaluated; }
};

#endif // CHESS_H
