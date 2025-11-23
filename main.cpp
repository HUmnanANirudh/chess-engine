#include "chess.h"
#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <iomanip>

class ChessGame {
private:
    ChessBoard board;
    ChessAI ai;
    bool humanIsWhite;
    
    struct HistoryMove {
        Move move;
        bool isHuman;
    };
    std::vector<HistoryMove> history;
    
public:
    ChessGame(bool humanWhite = true, int aiDepth = 4) 
        : ai(aiDepth), humanIsWhite(humanWhite) {}

private:
    void analyzeLastHumanMoveLive() {
        if (history.empty()) return;
        size_t idx = history.size() - 1;
        if (!history[idx].isHuman) return;

        ChessBoard analysisBoard;
        analysisBoard.initializeBoard();

        for (size_t j = 0; j < idx; ++j) {
            analysisBoard.movePiece(history[j].move);
            analysisBoard.switchPlayer();
        }

        int liveDepth = 5; // slightly shallower than post-game analysis
        std::vector<std::pair<Move, int>> evals = ai.evaluateAllMoves(analysisBoard, liveDepth);
        if (evals.empty()) return;

        bool whiteToMove = (analysisBoard.getCurrentPlayer() == WHITE);
        int bestScore = evals[0].second;
        for (const auto& e : evals) {
            if (whiteToMove) {
                if (e.second > bestScore) bestScore = e.second;
            } else {
                if (e.second < bestScore) bestScore = e.second;
            }
        }

        std::vector<Move> topMoves;
        for (const auto& e : evals) {
            if (whiteToMove) {
                if (e.second >= bestScore - 50) topMoves.push_back(e.first);
            } else {
                if (e.second <= bestScore + 50) topMoves.push_back(e.first);
            }
        }

        Move humanMove = history[idx].move;
        bool inTop = false;
        for (const auto& m : topMoves) {
            if (m.from.row == humanMove.from.row && m.from.col == humanMove.from.col &&
                m.to.row   == humanMove.to.row   && m.to.col   == humanMove.to.col) {
                inTop = true;
                break;
            }
        }

        double probability = 0.0;
        if (inTop && !topMoves.empty()) {
            probability = 100.0 / static_cast<double>(topMoves.size());
        }

        std::string quality;
        if (probability >= 70.0)      quality = "Excellent";
        else if (probability >= 50.0) quality = "Good";
        else if (probability >= 30.0) quality = "Okay";
        else if (probability > 0.0)   quality = "Weak";
        else                          quality = "Blunder";

        std::cout << "\n[Live Analysis] Your last move "
                  << humanMove.toAlgebraic()
                  << " | Score: " << bestScore
                  << " | Prob: " << std::fixed << std::setprecision(1) << probability << "%"
                  << " | Quality: " << quality << "\n";
    }
    
public:
    void displayMenu() {
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║     CHESS ENGINE - AI vs HUMAN         ║\n";
        std::cout << "║  Minimax with Alpha-Beta Pruning       ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        std::cout << "\n1. Play as White\n";
        std::cout << "2. Play as Black\n";
        std::cout << "3. AI vs AI\n";
        std::cout << "4. Exit\n";
        std::cout << "Choose option: ";
    }
    
    Position parsePosition(const std::string& input) {
        if (input.length() != 2) return Position(-1, -1);
        
        int col = input[0] - 'a';
        int row = 8 - (input[1] - '0');
        
        if (col < 0 || col > 7 || row < 0 || row > 7) {
            return Position(-1, -1);
        }
        
        return Position(row, col);
    }
    
    void playGame(bool aiVsAi = false) {
        board.initializeBoard();
        int moveNumber = 1;
        history.clear();
        bool endedByPlayer = false;
        
        std::cout << board.boardToString();
        
        while (true) {
            Color currentColor = board.getCurrentPlayer();
            std::string playerName = (currentColor == WHITE) ? "White" : "Black";
            
            bool isHumanTurn = aiVsAi ? false : 
                              ((currentColor == WHITE && humanIsWhite) || 
                               (currentColor == BLACK && !humanIsWhite));
            
            std::cout << "\n--- Move " << moveNumber << " - " << playerName << "'s Turn ---\n";
            
            Move move;
            
            if (isHumanTurn) {
                move = getHumanMove();
                if (move.from.row == -1) {
                    std::cout << "Game ended by player.\n";
                    endedByPlayer = true;
                    break;
                }
            } else {
                std::cout << "AI is thinking";
                for (int i = 0; i < 3; i++) {
                    std::cout << ".";
                    std::cout.flush();
                }
                std::cout << "\n";
                
                ai.analyzeMovesProbability(board, 3);
                move = ai.findBestMove(board);
                
                std::cout << "AI Move: " << move.toAlgebraic() << "\n";
                std::cout << "Nodes evaluated: " << ai.getNodesEvaluated() << "\n";
                ai.printMoveAnalysis();
            }
            
            Piece captured = board.getPiece(move.to);
            board.movePiece(move);
            board.switchPlayer();
            history.push_back({move, isHumanTurn});
            
            std::cout << board.boardToString();

            if (isHumanTurn) {
                analyzeLastHumanMoveLive();
            }
            
            // Check game state
            Color nextPlayer = board.getCurrentPlayer();
            if (board.isCheckmate(nextPlayer)) {
                std::cout << "\n╔════════════════════════════════════════╗\n";
                std::cout << "║          CHECKMATE!                    ║\n";
                std::cout << "║  " << playerName << " wins!                          ║\n";
                std::cout << "╚════════════════════════════════════════╝\n";
                break;
            }
            
            if (board.isStalemate(nextPlayer)) {
                std::cout << "\n╔════════════════════════════════════════╗\n";
                std::cout << "║          STALEMATE!                    ║\n";
                std::cout << "║          Game is a Draw                ║\n";
                std::cout << "╚════════════════════════════════════════╝\n";
                break;
            }
            
            if (board.isInCheck(nextPlayer)) {
                std::cout << "\n⚠️  " << (nextPlayer == WHITE ? "White" : "Black") << " is in CHECK!\n";
            }
            
            moveNumber++;
        }

        // Post-game probability analysis (only if game reached a terminal state)
        if (!endedByPlayer && !history.empty()) {
            std::cout << "\n╔════════════════════════════════════════╗\n";
            std::cout << "║         POST-GAME AI ANALYSIS          ║\n";
            std::cout << "╚════════════════════════════════════════╝\n";

            // For each human move, reconstruct position before that move
            int analysisDepth = 7; // deeper search for analysis

            std::cout << "\n";
            std::cout << std::left
                      << std::setw(8)  << "Move#"
                      << std::setw(10) << "Move"
                      << std::setw(12) << "Score"
                      << std::setw(15) << "Prob(%)"
                      << std::setw(12) << "Quality" << "\n";
            std::cout << std::string(57, '-') << "\n";

            int humanMoveIndex = 0;
            for (size_t i = 0; i < history.size(); ++i) {
                if (!history[i].isHuman) continue;

                // Reconstruct board state before this human move
                ChessBoard analysisBoard;
                analysisBoard.initializeBoard();

                // White to move in initial position
                for (size_t j = 0; j < i; ++j) {
                    analysisBoard.movePiece(history[j].move);
                    analysisBoard.switchPlayer();
                }

                // Evaluate all moves from this position with deeper search
                std::vector<std::pair<Move, int>> evals = ai.evaluateAllMoves(analysisBoard, analysisDepth);
                if (evals.empty()) continue;

                bool whiteToMove = (analysisBoard.getCurrentPlayer() == WHITE);

                int bestScore = evals[0].second;
                for (const auto& e : evals) {
                    if (whiteToMove) {
                        if (e.second > bestScore) bestScore = e.second;
                    } else {
                        if (e.second < bestScore) bestScore = e.second;
                    }
                }

                // Determine the set G of top moves within 50 centipawns
                std::vector<Move> topMoves;
                for (const auto& e : evals) {
                    if (whiteToMove) {
                        if (e.second >= bestScore - 50) topMoves.push_back(e.first);
                    } else {
                        if (e.second <= bestScore + 50) topMoves.push_back(e.first);
                    }
                }

                // Check if human move is in G
                Move humanMove = history[i].move;
                bool inTop = false;
                for (const auto& m : topMoves) {
                    if (m.from.row == humanMove.from.row && m.from.col == humanMove.from.col &&
                        m.to.row   == humanMove.to.row   && m.to.col   == humanMove.to.col) {
                        inTop = true;
                        break;
                    }
                }

                double probability = 0.0;
                if (inTop && !topMoves.empty()) {
                    probability = 100.0 / static_cast<double>(topMoves.size());
                }

                // Quality label based on probability
                std::string quality;
                if (probability >= 70.0)      quality = "Excellent";
                else if (probability >= 50.0) quality = "Good";
                else if (probability >= 30.0) quality = "Okay";
                else if (probability > 0.0)   quality = "Weak";
                else                          quality = "Blunder";

                ++humanMoveIndex;
                std::string moveStr = humanMove.toAlgebraic();

                std::cout << std::setw(8)  << humanMoveIndex
                          << std::setw(10) << moveStr
                          << std::setw(12) << bestScore
                          << std::setw(15) << std::fixed << std::setprecision(1) << probability
                          << std::setw(12) << quality << "\n";
            }
        }
    }
    
    Move getHumanMove() {
        while (true) {
            std::cout << "\nEnter move (e.g., e2e4) or 'quit' to exit: ";
            std::string input;
            std::getline(std::cin, input);
            
            if (input == "quit") {
                return Move(Position(-1, -1), Position(-1, -1));
            }
            
            if (input.length() != 4) {
                std::cout << "Invalid format. Use algebraic notation (e.g., e2e4).\n";
                continue;
            }
            
            Position from = parsePosition(input.substr(0, 2));
            Position to = parsePosition(input.substr(2, 2));
            
            if (!from.isValid() || !to.isValid()) {
                std::cout << "Invalid position. Use a-h for columns and 1-8 for rows.\n";
                continue;
            }
            
            Move move(from, to);
            
            if (!board.isLegalMove(move)) {
                std::cout << "Illegal move. Try again.\n";
                continue;
            }
            
            return move;
        }
    }
    
    void run() {
        while (true) {
            displayMenu();
            std::string choice;
            std::getline(std::cin, choice);
            
            if (choice == "1") {
                humanIsWhite = true;
                playGame(false);
            } else if (choice == "2") {
                humanIsWhite = false;
                playGame(false);
            } else if (choice == "3") {
                playGame(true);
            } else if (choice == "4") {
                std::cout << "Thanks for playing! Goodbye.\n";
                break;
            } else {
                std::cout << "Invalid option. Try again.\n";
            }
        }
    }
};

int main() {
    ChessGame game(true, 4);
    game.run();
    return 0;
}
