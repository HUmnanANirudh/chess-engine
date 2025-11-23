# Chess Engine - AI vs Human

A sophisticated chess engine implementation in C++ featuring AI gameplay with Minimax algorithm and Alpha-Beta pruning, complete with move probability analysis.

## Features

- **Complete Chess Implementation**
  - Full board representation with all piece types
  - Legal move generation with check/checkmate/stalemate detection
  - Piece movement validation for all chess rules

- **AI Engine**
  - Minimax algorithm with Alpha-Beta pruning for optimal move selection
  - Configurable search depth for difficulty levels
  - Move probability analysis showing evaluated positions
  - Node counting for performance metrics

- **Game Modes**
  - Human vs AI (play as White or Black)
  - AI vs AI (watch two engines play)
  - Interactive command-line interface

- **Move Analysis**
  - Displays all legal moves with their evaluation scores
  - Shows number of nodes evaluated during search
  - Helps understand AI decision-making process

## Building the Project

### Prerequisites
- C++17 compatible compiler (MSVC, GCC, or Clang)
- CMake 3.10 or higher

### Build Instructions

#### Windows (MSVC)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

#### Linux/macOS (GCC/Clang)
```bash
mkdir build
cd build
cmake ..
make
```

## Running the Game

```bash
./chess_engine
```

Or on Windows:
```bash
chess_engine.exe
```

## How to Play

1. **Start the Game**: Choose from the menu:
   - Option 1: Play as White (you move first)
   - Option 2: Play as Black (AI moves first)
   - Option 3: Watch AI vs AI
   - Option 4: Exit

2. **Enter Moves**: Use algebraic notation (e.g., `e2e4`)
   - First two characters: source square (column a-h, row 1-8)
   - Last two characters: destination square
   - Example: `e2e4` moves piece from e2 to e4

3. **Game Status**:
   - Board is displayed after each move
   - Check status is indicated with ⚠️
   - Game ends on checkmate or stalemate

## Algorithm Details

### Minimax with Alpha-Beta Pruning

The AI uses the Minimax algorithm to evaluate positions:

```
minimax(position, depth, alpha, beta, isMaximizing):
    if depth == 0:
        return evaluate(position)
    
    if isMaximizing:
        maxEval = -infinity
        for each move:
            eval = minimax(child, depth-1, alpha, beta, false)
            maxEval = max(maxEval, eval)
            alpha = max(alpha, eval)
            if beta <= alpha: break  // Alpha-Beta cutoff
        return maxEval
    else:
        minEval = +infinity
        for each move:
            eval = minimax(child, depth-1, alpha, beta, true)
            minEval = min(minEval, eval)
            beta = min(beta, eval)
            if beta <= alpha: break  // Alpha-Beta cutoff
        return minEval
```

### Position Evaluation

Pieces are valued as follows:
- Pawn: 1 point
- Knight: 3 points
- Bishop: 3 points
- Rook: 5 points
- Queen: 9 points
- King: Infinite (cannot be captured)

### Move Probability Analysis

The engine analyzes all legal moves and displays:
- Each possible move in algebraic notation
- The evaluation score for that move
- Total nodes evaluated during the search

## File Structure

- `chess.h` - Header file with class definitions
- `chess.cpp` - Implementation of chess logic and AI
- `main.cpp` - Game loop and user interface
- `CMakeLists.txt` - Build configuration
- `README.md` - This file

## Performance Tips

- **Adjust Difficulty**: Modify the depth parameter in `ChessGame` constructor (default: 4)
  - Depth 3: Fast, suitable for casual play
  - Depth 4: Balanced difficulty
  - Depth 5+: Stronger AI, slower moves

- **Alpha-Beta Pruning**: Significantly reduces nodes evaluated compared to pure Minimax
  - Typical reduction: 50-90% fewer nodes

## Example Game Session

```
╔════════════════════════════════════════╗
║     CHESS ENGINE - AI vs HUMAN         ║
║  Minimax with Alpha-Beta Pruning       ║
╚════════════════════════════════════════╝

  a b c d e f g h
8 r n b q k b n r 8
7 p p p p p p p p 7
6 . . . . . . . . 6
5 . . . . . . . . 5
4 . . . . . . . . 4
3 . . . . . . . . 3
2 P P P P P P P P 2
1 R N B Q K B N R 1
  a b c d e f g h

--- Move 1 - White's Turn ---
Enter move (e.g., e2e4) or 'quit' to exit: e2e4
```

## License

This project is provided as-is for educational purposes.

## Author

Created as a demonstration of chess AI implementation using Minimax and Alpha-Beta pruning algorithms.
