# Chess Engine - GUI Version

A beautiful, modern web-based GUI for the Chess Engine with AI opponent.

## Quick Start

### Option 1: Web GUI (Recommended - No Installation Required)

1. **Run the GUI Server:**
   ```bash
   python server.py
   ```
   Or double-click `run_gui.bat`

2. **Open in Browser:**
   - Automatically opens at `http://localhost:8000/gui.html`
   - Or manually open the URL in your browser

3. **Play the Game:**
   - Click a white piece to select it
   - Click a highlighted square to move
   - AI plays as Black automatically

### Option 2: Command-Line Version

Run the compiled C++ executable:
```bash
chess_engine.exe
```

## Features

### Web GUI (gui.html)
- **Modern, Responsive Design**
  - Beautiful gradient background
  - Smooth animations and transitions
  - Mobile-friendly layout

- **Interactive Chessboard**
  - Click to select pieces
  - Highlighted legal moves
  - Piece symbols (♔ ♕ ♖ ♗ ♘ ♙)
  - Coordinate labels (a-h, 1-8)

- **Game Controls**
  - New Game button
  - Undo Move (undoes last 2 moves)
  - Difficulty selector (Easy/Medium/Hard)

- **Game Status Display**
  - Current player indicator
  - Move history
  - Game state (checkmate, stalemate)

### C++ Console Version (chess_engine.exe)
- Full chess rules implementation
- Minimax with Alpha-Beta pruning
- Move probability analysis
- Multiple game modes (Human vs AI, AI vs AI)

## How to Play

1. **Select a Piece:**
   - Click on any white piece
   - Legal moves will be highlighted in green

2. **Make a Move:**
   - Click on a highlighted square to move
   - The AI will automatically respond

3. **Game Ends When:**
   - Checkmate occurs
   - Stalemate occurs
   - You click "New Game"

## Difficulty Levels

- **Easy (Depth 2):** Quick moves, weaker play
- **Medium (Depth 3):** Balanced difficulty
- **Hard (Depth 4):** Strong opponent (default)

## Files

### Web GUI Files
- `gui.html` - Main HTML interface
- `chess_game.js` - Game logic and AI
- `server.py` - Python web server
- `run_gui.bat` - Batch file to start server

### C++ Console Files
- `chess.h` - Header with class definitions
- `chess.cpp` - Chess logic and AI implementation
- `main.cpp` - Console game loop
- `chess_engine.exe` - Compiled executable

## Requirements

### For Web GUI
- Python 3.x (for server)
- Any modern web browser (Chrome, Firefox, Edge, Safari)

### For C++ Version
- Windows with MinGW (already compiled)
- Or: C++17 compiler to rebuild

## Building from Source

### Rebuild C++ Version
```bash
g++ -std=c++17 -Wall -Wextra -O2 -o chess_engine.exe main.cpp chess.cpp
```

Or use the build script:
```bash
build.bat
```

### Rebuild GUI (Qt Version - Optional)
Requires Qt5 installed:
```bash
build_gui.bat
```

## Game Rules

The engine implements full chess rules:
- All piece movements (Pawn, Knight, Bishop, Rook, Queen, King)
- Check and Checkmate detection
- Stalemate detection
- Pawn promotion
- Castling (in C++ version)
- En passant (in C++ version)

## AI Algorithm

The AI uses:
- **Minimax Algorithm:** Evaluates all possible moves
- **Alpha-Beta Pruning:** Optimizes search by eliminating branches
- **Position Evaluation:** Piece values and board control
- **Configurable Depth:** Adjust difficulty by changing search depth

## Troubleshooting

### Server won't start
- Make sure Python is installed: `python --version`
- Check if port 8000 is available
- Try a different port in `server.py`

### Browser won't open automatically
- Manually open `http://localhost:8000/gui.html`
- Check firewall settings

### Game is too slow
- Reduce difficulty level
- Close other applications

### Pieces not moving
- Make sure you're playing as White
- Select a piece first (it will highlight)
- Click a green highlighted square

## Tips for Playing

1. **Control the Center:** Occupy central squares
2. **Develop Pieces:** Get knights and bishops out early
3. **Protect Your King:** Castle early if possible
4. **Watch for Tactics:** Look for forks, pins, and skewers
5. **Material Count:** Don't lose pieces for nothing

## Performance

- **Web Version:** Instant moves (random AI)
- **C++ Console:** 1-3 seconds per move (Minimax depth 4)
- **C++ GUI (Qt):** 1-3 seconds per move (Minimax depth 4)

## Future Enhancements

- [ ] Opening book database
- [ ] Endgame tablebase
- [ ] Network multiplayer
- [ ] Move animation
- [ ] Sound effects
- [ ] Elo rating system
- [ ] Game replay/analysis

## License

Educational project - Free to use and modify.

## Author

Created as a demonstration of chess AI implementation using Minimax and Alpha-Beta pruning algorithms.

---

**Enjoy the game!** ♟
