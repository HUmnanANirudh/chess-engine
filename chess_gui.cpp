#include "chess_gui.h"
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFont>
#include <QColor>
#include <QMessageBox>
#include <sstream>

// ==================== ChessBoardWidget Implementation ====================

ChessBoardWidget::ChessBoardWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(SQUARE_SIZE * 8, SQUARE_SIZE * 8);
    setMaximumSize(SQUARE_SIZE * 8, SQUARE_SIZE * 8);
    setStyleSheet("background-color: #f0f0f0;");
}

void ChessBoardWidget::setBoard(const ChessBoard& board) {
    currentBoard = board;
    update();
}

void ChessBoardWidget::setSelectedSquare(int row, int col) {
    selectedRow = row;
    selectedCol = col;
    update();
}

void ChessBoardWidget::setHighlightedSquares(const std::vector<Move>& moves) {
    highlightedMoves = moves;
    update();
}

void ChessBoardWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    drawBoard(painter);
    drawHighlights(painter);
    drawPieces(painter);
}

void ChessBoardWidget::drawBoard(QPainter& painter) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int x = col * SQUARE_SIZE;
            int y = row * SQUARE_SIZE;
            
            QColor squareColor;
            if ((row + col) % 2 == 0) {
                squareColor = QColor(240, 217, 181);  // Light square
            } else {
                squareColor = QColor(181, 136, 99);   // Dark square
            }
            
            painter.fillRect(x, y, SQUARE_SIZE, SQUARE_SIZE, squareColor);
            painter.drawRect(x, y, SQUARE_SIZE, SQUARE_SIZE);
        }
    }
    
    // Draw coordinates
    QFont font("Arial", 8);
    painter.setFont(font);
    painter.setPen(Qt::black);
    
    for (int i = 0; i < 8; i++) {
        painter.drawText(5, i * SQUARE_SIZE + 15, QString(QChar('8' - i)));
        painter.drawText(SQUARE_SIZE * 8 - 15, (i + 1) * SQUARE_SIZE - 5, QString(QChar('a' + i)));
    }
}

void ChessBoardWidget::drawHighlights(QPainter& painter) {
    if (selectedRow >= 0 && selectedCol >= 0) {
        int x = selectedCol * SQUARE_SIZE;
        int y = selectedRow * SQUARE_SIZE;
        painter.fillRect(x, y, SQUARE_SIZE, SQUARE_SIZE, QColor(255, 255, 0, 100));
    }
    
    for (const auto& move : highlightedMoves) {
        int x = move.to.col * SQUARE_SIZE;
        int y = move.to.row * SQUARE_SIZE;
        painter.fillRect(x, y, SQUARE_SIZE, SQUARE_SIZE, QColor(0, 255, 0, 80));
    }
}

void ChessBoardWidget::drawPieces(QPainter& painter) {
    QFont pieceFont("Arial", 36, QFont::Bold);
    painter.setFont(pieceFont);
    
    const char* pieceSymbols[] = {"", "♟", "♞", "♝", "♜", "♛", "♚"};
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece piece = currentBoard.getPiece(Position(row, col));
            if (piece.isEmpty()) continue;
            
            int x = col * SQUARE_SIZE;
            int y = row * SQUARE_SIZE;
            
            painter.setPen(piece.color == WHITE ? Qt::white : Qt::black);
            painter.drawText(x + 12, y + 50, QString(pieceSymbols[piece.type]));
        }
    }
}

void ChessBoardWidget::mousePressEvent(QMouseEvent* event) {
    int col = event->x() / SQUARE_SIZE;
    int row = event->y() / SQUARE_SIZE;
    
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        emit squareClicked(row, col);
    }
}

// ==================== ChessGUI Implementation ====================

ChessGUI::ChessGUI(QWidget* parent) 
    : QMainWindow(parent), ai(4) {
    setWindowTitle("Chess Engine - AI vs Human");
    setWindowIcon(QIcon("chess.ico"));
    
    aiTimer = new QTimer(this);
    connect(aiTimer, &QTimer::timeout, this, &ChessGUI::onAIMove);
    
    setupUI();
    onNewGame();
}

void ChessGUI::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Left side - Board
    QVBoxLayout* boardLayout = new QVBoxLayout();
    boardWidget = new ChessBoardWidget();
    connect(boardWidget, &ChessBoardWidget::squareClicked, this, &ChessGUI::onBoardSquareClicked);
    boardLayout->addWidget(boardWidget);
    boardLayout->setAlignment(boardWidget, Qt::AlignCenter);
    
    // Right side - Controls and Info
    QVBoxLayout* rightLayout = new QVBoxLayout();
    
    // Game Status
    statusLabel = new QLabel("Game Started - White to Move");
    statusLabel->setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;");
    rightLayout->addWidget(statusLabel);
    
    // Difficulty Selection
    QGroupBox* difficultyGroup = new QGroupBox("AI Difficulty");
    QVBoxLayout* diffLayout = new QVBoxLayout();
    difficultyCombo = new QComboBox();
    difficultyCombo->addItem("Easy (Depth 2)");
    difficultyCombo->addItem("Medium (Depth 3)");
    difficultyCombo->addItem("Hard (Depth 4)");
    difficultyCombo->addItem("Very Hard (Depth 5)");
    difficultyCombo->setCurrentIndex(2);
    connect(difficultyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChessGUI::onDifficultyChanged);
    diffLayout->addWidget(difficultyCombo);
    difficultyGroup->setLayout(diffLayout);
    rightLayout->addWidget(difficultyGroup);
    
    // Move History
    QGroupBox* historyGroup = new QGroupBox("Move History");
    QVBoxLayout* historyLayout = new QVBoxLayout();
    moveHistoryLabel = new QLabel("");
    moveHistoryLabel->setWordWrap(true);
    moveHistoryLabel->setStyleSheet("background-color: #f9f9f9; padding: 5px;");
    historyLayout->addWidget(moveHistoryLabel);
    historyGroup->setLayout(historyLayout);
    rightLayout->addWidget(historyGroup);
    
    // Analysis
    QGroupBox* analysisGroup = new QGroupBox("Move Analysis");
    QVBoxLayout* analysisLayout = new QVBoxLayout();
    analysisText = new QTextEdit();
    analysisText->setReadOnly(true);
    analysisText->setMaximumHeight(150);
    analysisText->setStyleSheet("background-color: #f9f9f9;");
    analysisLayout->addWidget(analysisText);
    analysisGroup->setLayout(analysisLayout);
    rightLayout->addWidget(analysisGroup);
    
    // Buttons
    QGroupBox* buttonGroup = new QGroupBox("Controls");
    QVBoxLayout* buttonLayout = new QVBoxLayout();
    
    newGameBtn = new QPushButton("New Game");
    connect(newGameBtn, &QPushButton::clicked, this, &ChessGUI::onNewGame);
    buttonLayout->addWidget(newGameBtn);
    
    undoBtn = new QPushButton("Undo Move");
    connect(undoBtn, &QPushButton::clicked, this, &ChessGUI::onUndoMove);
    buttonLayout->addWidget(undoBtn);
    
    QPushButton* resetBtn = new QPushButton("Reset Board");
    connect(resetBtn, &QPushButton::clicked, this, &ChessGUI::onResetGame);
    buttonLayout->addWidget(resetBtn);
    
    buttonGroup->setLayout(buttonLayout);
    rightLayout->addWidget(buttonGroup);
    
    rightLayout->addStretch();
    
    // Add layouts to main
    mainLayout->addLayout(boardLayout, 2);
    mainLayout->addLayout(rightLayout, 1);
    
    centralWidget->setLayout(mainLayout);
    resize(1000, 600);
}

void ChessGUI::onNewGame() {
    board.initializeBoard();
    gameOver = false;
    moveHistory.clear();
    selectedFrom = Position(-1, -1);
    
    updateBoardDisplay();
    updateGameStatus();
    moveHistoryLabel->setText("");
    analysisText->setText("");
}

void ChessGUI::onDifficultyChanged(int index) {
    ai = ChessAI(index + 2);  // 2 to 5
}

void ChessGUI::onBoardSquareClicked(int row, int col) {
    if (gameOver) return;
    if (board.getCurrentPlayer() != WHITE) return;  // Only human (white) can click
    
    Position clickedPos(row, col);
    
    if (selectedFrom.row == -1) {
        // First click - select piece
        Piece piece = board.getPiece(clickedPos);
        if (!piece.isEmpty() && piece.color == WHITE) {
            selectedFrom = clickedPos;
            boardWidget->setSelectedSquare(row, col);
            
            // Show legal moves
            std::vector<Move> legalMoves = board.generateLegalMoves();
            std::vector<Move> pieceMoves;
            for (const auto& move : legalMoves) {
                if (move.from == selectedFrom) {
                    pieceMoves.push_back(move);
                }
            }
            boardWidget->setHighlightedSquares(pieceMoves);
        }
    } else {
        // Second click - make move
        Move move(selectedFrom, clickedPos);
        
        if (board.isLegalMove(move)) {
            makeMove(move);
            selectedFrom = Position(-1, -1);
            boardWidget->setSelectedSquare(-1, -1);
            boardWidget->setHighlightedSquares({});
            
            // AI move after short delay
            aiTimer->start(500);
        } else {
            // Try selecting different piece
            Piece piece = board.getPiece(clickedPos);
            if (!piece.isEmpty() && piece.color == WHITE) {
                selectedFrom = clickedPos;
                boardWidget->setSelectedSquare(row, col);
                
                std::vector<Move> legalMoves = board.generateLegalMoves();
                std::vector<Move> pieceMoves;
                for (const auto& move : legalMoves) {
                    if (move.from == selectedFrom) {
                        pieceMoves.push_back(move);
                    }
                }
                boardWidget->setHighlightedSquares(pieceMoves);
            } else {
                selectedFrom = Position(-1, -1);
                boardWidget->setSelectedSquare(-1, -1);
                boardWidget->setHighlightedSquares({});
            }
        }
    }
}

void ChessGUI::onAIMove() {
    aiTimer->stop();
    
    if (gameOver || board.getCurrentPlayer() == WHITE) return;
    
    statusLabel->setText("AI is thinking...");
    QApplication::processEvents();
    
    ai.analyzeMovesProbability(board, 3);
    Move aiMove = ai.findBestMove(board);
    
    if (aiMove.from.row != -1) {
        makeMove(aiMove);
        
        // Update analysis
        std::stringstream ss;
        ss << "Nodes evaluated: " << ai.getNodesEvaluated() << "\n";
        analysisText->setText(QString::fromStdString(ss.str()));
    }
    
    updateGameStatus();
}

void ChessGUI::makeMove(const Move& move) {
    Piece captured = board.getPiece(move.to);
    board.movePiece(move);
    moveHistory.push_back(move);
    board.switchPlayer();
    
    updateBoardDisplay();
    
    // Update move history
    std::string moveStr = move.toAlgebraic();
    QString historyText = moveHistoryLabel->text();
    if (!historyText.isEmpty()) historyText += " ";
    historyText += QString::fromStdString(moveStr);
    moveHistoryLabel->setText(historyText);
    
    // Check game state
    Color currentPlayer = board.getCurrentPlayer();
    if (board.isCheckmate(currentPlayer)) {
        gameOver = true;
        statusLabel->setText("Checkmate! " + QString(currentPlayer == WHITE ? "Black" : "White") + " wins!");
        return;
    }
    
    if (board.isStalemate(currentPlayer)) {
        gameOver = true;
        statusLabel->setText("Stalemate! Game is a draw.");
        return;
    }
    
    if (board.isInCheck(currentPlayer)) {
        statusLabel->setText(QString(currentPlayer == WHITE ? "White" : "Black") + " is in CHECK!");
    } else {
        statusLabel->setText(QString(currentPlayer == WHITE ? "White" : "Black") + " to move");
    }
}

void ChessGUI::onUndoMove() {
    if (moveHistory.empty() || gameOver) return;
    
    // Undo last two moves (AI and human)
    int movesToUndo = std::min(2, (int)moveHistory.size());
    for (int i = 0; i < movesToUndo; i++) {
        moveHistory.pop_back();
    }
    
    board.initializeBoard();
    for (const auto& move : moveHistory) {
        Piece captured = board.getPiece(move.to);
        board.movePiece(move);
        board.switchPlayer();
    }
    
    gameOver = false;
    updateBoardDisplay();
    updateGameStatus();
    
    // Update move history display
    QString historyText;
    for (const auto& move : moveHistory) {
        if (!historyText.isEmpty()) historyText += " ";
        historyText += QString::fromStdString(move.toAlgebraic());
    }
    moveHistoryLabel->setText(historyText);
}

void ChessGUI::onResetGame() {
    onNewGame();
}

void ChessGUI::updateBoardDisplay() {
    boardWidget->setBoard(board);
    boardWidget->setSelectedSquare(-1, -1);
    boardWidget->setHighlightedSquares({});
}

void ChessGUI::updateGameStatus() {
    if (gameOver) return;
    
    Color currentPlayer = board.getCurrentPlayer();
    QString playerText = (currentPlayer == WHITE) ? "White" : "Black";
    
    if (board.isInCheck(currentPlayer)) {
        statusLabel->setText(playerText + " is in CHECK!");
    } else {
        statusLabel->setText(playerText + " to move");
    }
}
