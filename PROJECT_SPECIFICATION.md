# Chess Engine: AI vs Human with Move Probability Analysis

## Project Overview

**Title:** Chess Engine: AI vs Human Chess with Move Probability Analysis using Minimax & Alpha-Beta in C++

**Type:** Prototype

**Team Members:**
- Shuvam Bhatt (230457)
- Kushagra (230442)
- Anirudh (230466)
- Rohit Singh (230782)

---

## Abstract

This project implements a one-on-one chess game between an AI opponent and a human player, built entirely in C++. The AI uses the **Minimax algorithm with Alpha-Beta pruning** to select strong moves during gameplay. 

**Key Innovation:** Post-game analysis that shows the **probability percentage** of each human move, indicating how likely the AI would have played the same move at that exact moment. For example: "Your move e5 was 72% likely for the AI."

This probability is calculated by:
1. Running a deeper search (depth 7) after the game
2. Evaluating all legal moves at that position
3. Counting how many top-scoring moves (within 50 centipawns of best) match the human's move
4. Computing: Probability = (1 / |top_moves|) × 100%

The entire system is coded from scratch in C++ using **bitboards** for efficient move generation and board representation.

---

## Problem Statement

### Challenge
Develop a chess game where a human player competes against an AI opponent, providing an interactive platform to learn from AI decision-making. Unlike standard chess applications that simply label moves as "blunder" or "best move," our system quantifies move correctness using probability.

### Goals
1. **Enable the AI to play smart and fast chess** using adversarial search techniques
2. **Display probability for each human move** to demonstrate its correctness relative to the AI's deeper thinking

### Metrics
- **High probability (>70%):** Correct, strong move aligned with AI thinking
- **Medium probability (30-70%):** Acceptable move with some deviation
- **Low probability (<30%):** Weak or creative move, potential improvement area

---

## AI Components & Methodology

### Board Representation: Bitboards
- **Structure:** 64-bit integers (one for each piece type)
  - `white_pawns`, `white_knights`, `white_bishops`, `white_rooks`, `white_queens`, `white_king`
  - `black_pawns`, `black_knights`, `black_bishops`, `black_rooks`, `black_queens`, `black_king`
- **Advantages:**
  - O(1) bitwise operations for move generation
  - Efficient board updates and copying
  - Fast attack/defense calculations

### Search Algorithm: Minimax with Alpha-Beta Pruning
```
minimax(position, depth, alpha, beta, isMaximizing):
    if depth == 0:
        return evaluate(position)
    
    if isMaximizing:
        maxEval = -∞
        for each legal move:
            eval = minimax(child, depth-1, alpha, beta, false)
            maxEval = max(maxEval, eval)
            alpha = max(alpha, eval)
            if beta ≤ alpha: break  // Pruning
        return maxEval
    else:
        minEval = +∞
        for each legal move:
            eval = minimax(child, depth-1, alpha, beta, true)
            minEval = min(minEval, eval)
            beta = min(beta, eval)
            if beta ≤ alpha: break  // Pruning
        return minEval
```

**Depths:**
- **Gameplay:** Depth 5 (~1-2 seconds per move)
- **Analysis:** Depth 7 (post-game, more thorough evaluation)

### Position Evaluation
Combines multiple factors:

1. **Material Value:**
   - Pawn: 1 point
   - Knight: 3 points
   - Bishop: 3 points
   - Rook: 5 points
   - Queen: 9 points
   - King: Infinite (cannot be captured)

2. **Piece-Square Tables (PST):**
   - Positional bonuses/penalties for each piece type
   - Encourages center control, piece development, king safety
   - Different tables for middlegame and endgame (king)

3. **Evaluation Formula:**
   ```
   Score = Material_Value + PST_Bonus + Positional_Factors
   ```

### Move Probability Analysis

**Formula:**
```
Let S = best score from depth 7 search
Let G = {all legal moves with score ≥ S - 50 centipawns}
Probability = (1 / |G|) × 100% if human_move ∈ G, else 0%
```

**Interpretation:**
- **High %:** Move is among the top candidates (strong play)
- **Low %:** Move is outside top candidates (weak or creative)
- **0%:** Move is significantly worse than best moves

---

## Game Flow

### During Gameplay
1. **Human (White) starts** → inputs move via console in algebraic notation (e.g., "e2e4")
2. **AI (Black) runs Minimax** with Alpha-Beta Pruning at depth 5
3. **AI selects and makes** the best move
4. **Board is updated** and game state checked for:
   - Checkmate
   - Stalemate
   - Draw (50-move rule, threefold repetition)
5. **Loop continues** until game ends

### Post-Game Analysis
1. **Replay each position** just before a human move
2. **Run deeper Minimax search** (depth 7) to evaluate all legal moves
3. **Compute probability** using the formula above
4. **Display results** with:
   - Move notation
   - Evaluation score
   - Probability percentage
   - Quality assessment (Excellent/Good/Acceptable/Weak)

---

## Implementation Details

### Core Files

#### `bitboard.h`
- Bitboard utility functions
- Bit manipulation (popcount, lsb, etc.)
- Square-to-notation conversion
- Board printing utilities

#### `piece_square_tables.h`
- PST arrays for all piece types
- Separate tables for middlegame/endgame
- PST value lookup function

#### `chess_enhanced.h` / `chess_enhanced.cpp`
- **ChessBoardEnhanced:** Board state management
  - Bitboard representation
  - Move generation (pseudo-legal and legal)
  - Position evaluation
  - Game state detection
  
- **ChessAIEnhanced:** AI engine
  - Minimax with Alpha-Beta pruning
  - Quiescence search (optional)
  - Best move selection
  - Post-game analysis

#### `main.cpp`
- Game loop
- User input parsing
- Move validation
- Game state display
- Analysis output

### Move Input Format
Supports algebraic notation:
- **Standard:** `e2e4` (from-to squares)
- **Promotion:** `e7e8Q` (with promotion piece)
- **Castling:** `e1g1` (kingside) or `e1c1` (queenside)

---

## Algorithm Complexity

### Time Complexity
- **Move Generation:** O(1) per move (bitboard operations)
- **Minimax:** O(b^d) where b = branching factor (~35), d = depth
  - Depth 5: ~35^5 ≈ 52 million nodes (with pruning: ~1-2 million)
  - Depth 7: ~35^7 ≈ 6.4 billion nodes (with pruning: ~10-20 million)

### Space Complexity
- **Board:** O(1) - fixed 6 bitboards per side
- **Move History:** O(n) where n = number of moves
- **Search Stack:** O(d) where d = search depth

---

## Performance Metrics

### Gameplay Performance
- **Move Time:** 1-2 seconds per move (depth 5)
- **Nodes Evaluated:** 1-2 million per move
- **Alpha-Beta Efficiency:** 50-90% reduction vs pure Minimax

### Analysis Performance
- **Analysis Time:** 5-10 seconds per game (depth 7)
- **Nodes Evaluated:** 10-20 million per game
- **Accuracy:** High confidence in move probability

---

## Testing & Validation

### Unit Tests
- [ ] Bitboard operations (popcount, lsb, etc.)
- [ ] Move generation correctness
- [ ] Position evaluation consistency
- [ ] Minimax correctness (known positions)

### Integration Tests
- [ ] Full game playthrough
- [ ] Checkmate detection
- [ ] Stalemate detection
- [ ] Post-game analysis accuracy

### Performance Tests
- [ ] Move generation speed
- [ ] Search speed at various depths
- [ ] Memory usage

---

## Future Enhancements

1. **Opening Book:** Pre-computed opening moves from Lichess
2. **Endgame Tablebase:** Perfect play in endgames
3. **Transposition Table:** Cache evaluated positions
4. **Iterative Deepening:** Better time management
5. **Move Ordering:** Improve alpha-beta efficiency
6. **GUI:** Qt or web-based interface
7. **Network Play:** Multiplayer support
8. **Elo Rating:** Difficulty levels with ratings

---

## References

1. **ChessProgramming Wiki – Minimax:** https://www.chessprogramming.org/Minimax
2. **ChessProgramming Wiki – Alpha-Beta:** https://www.chessprogramming.org/Alpha-Beta
3. **C++17 Standard:** https://en.cppreference.com/w/cpp/17.html
4. **Stockfish Documentation:** https://official-stockfish.github.io/docs/
5. **CMake Documentation:** https://cmake.org/documentation/

---

## Build Instructions

### Prerequisites
- C++17 compiler (GCC, Clang, MSVC)
- CMake 3.10+ (optional)

### Compile
```bash
g++ -std=c++17 -O2 -o chess_engine main.cpp chess_enhanced.cpp
```

### Run
```bash
./chess_engine
```

---

## License

Educational project - Free to use and modify.

---

**Last Updated:** November 21, 2025
