#define CHESS_GUI_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QTimer>
#include "chess.h"

class ChessBoardWidget : public QWidget {
    Q_OBJECT

public:
    ChessBoardWidget(QWidget* parent = nullptr);
    void setBoard(const ChessBoard& board);
    void setSelectedSquare(int row, int col);
    void setHighlightedSquares(const std::vector<Move>& moves);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void squareClicked(int row, int col);

private:
    ChessBoard currentBoard;
    int selectedRow = -1, selectedCol = -1;
    std::vector<Move> highlightedMoves;
    const int SQUARE_SIZE = 60;
    
    void drawBoard(QPainter& painter);
    void drawPieces(QPainter& painter);
    void drawHighlights(QPainter& painter);
};

class ChessGUI : public QMainWindow {
    Q_OBJECT

public:
    ChessGUI(QWidget* parent = nullptr);

private slots:
    void onNewGame();
    void onDifficultyChanged(int index);
    void onBoardSquareClicked(int row, int col);
    void onAIMove();
    void onUndoMove();
    void onResetGame();

private:
    void setupUI();
    void updateBoardDisplay();
    void updateGameStatus();
    void makeMove(const Move& move);
    
    ChessBoard board;
    ChessAI ai;
    ChessBoardWidget* boardWidget;
    QLabel* statusLabel;
    QLabel* moveHistoryLabel;
    QTextEdit* analysisText;
    QPushButton* newGameBtn;
    QPushButton* undoBtn;
    QComboBox* difficultyCombo;
    QTimer* aiTimer;
    
    bool humanIsWhite = true;
    bool gameOver = false;
    Position selectedFrom = Position(-1, -1);
    std::vector<Move> moveHistory;
};

#endif // CHESS_GUI_H
