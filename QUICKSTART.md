# Chess Engine - Quick Start Guide

## 🚀 Get Started in 30 Seconds

### Option 1: Play in Web Browser (Easiest)

1. **Open Terminal/Command Prompt** in the chess folder
2. **Run:** `python server.py`
3. **Browser opens automatically** at http://localhost:8000
4. **Click pieces to play!** (You are White)

### Option 2: Play in Console

1. **Open Terminal/Command Prompt** in the chess folder
2. **Run:** `chess_engine.exe`
3. **Choose game mode** (1 = Human vs AI as White)
4. **Enter moves** like `e2e4` (from-square to-square)

---

## 📋 Move Notation

Enter moves as: **`source_square` + `destination_square`**

Examples:
- `e2e4` - Move pawn from e2 to e4
- `g1f3` - Move knight from g1 to f3
- `e7e8q` - Move pawn from e7 to e8 and promote to queen

**Squares:** a-h (columns) and 1-8 (rows)

```
  a b c d e f g h
8 ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜ 8
7 ♟ ♟ ♟ ♟ ♟ ♟ ♟ ♟ 7
6 . . . . . . . . 6
5 . . . . . . . . 5
4 . . . . . . . . 4
3 . . . . . . . . 3
2 ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙ 2
1 ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖ 1
  a b c d e f g h
```

---

## 🎮 Game Controls

### Console Version
- **Enter move:** Type move in algebraic notation
- **Quit:** Type `quit`
- **New game:** Restart the program

### Web GUI
- **Select piece:** Click on a white piece
- **See legal moves:** Green highlighted squares
- **Move piece:** Click on a highlighted square
- **New Game:** Click "New Game" button
- **Undo:** Click "Undo Move" button
- **Change difficulty:** Select from dropdown

---

## 🤖 AI Difficulty Levels

| Level | Depth | Speed | Strength |
|-------|-------|-------|----------|
| Easy | 2 | Instant | Weak |
| Medium | 3 | 0.5s | Moderate |
| Hard | 4 | 1-2s | Strong |
| Very Hard | 5 | 3-5s | Very Strong |

---

## 📊 Game Status

The game ends when:
- **Checkmate:** One player has no legal moves and is in check
- **Stalemate:** One player has no legal moves but is NOT in check (draw)
- **You quit:** Type `quit` in console version

---

## 🎯 Tips for Playing

1. **Control the center** - Occupy d4, e4, d5, e5
2. **Develop pieces** - Get knights and bishops out early
3. **Protect your king** - Castle early if possible
4. **Watch for tactics** - Look for forks, pins, skewers
5. **Count material** - Don't lose pieces for nothing

---

## ⚙️ Troubleshooting

### "Port 8000 already in use"
- Change port in `server.py` (line with `PORT = 8000`)
- Or close other applications using port 8000

### "Python not found"
- Install Python from https://www.python.org/
- Make sure to check "Add Python to PATH" during installation

### "chess_engine.exe not found"
- Make sure you're in the chess folder
- Or run: `g++ -std=c++17 -O2 -o chess_engine.exe main.cpp chess.cpp`

### "Browser doesn't open automatically"
- Manually open http://localhost:8000/gui.html

### "Game is too slow"
- Reduce AI difficulty level
- Close other applications

---

## 📁 Important Files

| File | Purpose |
|------|---------|
| `chess_engine.exe` | Console game executable |
| `gui.html` | Web GUI interface |
| `server.py` | Web server |
| `run_gui.bat` | One-click GUI launcher |
| `README.md` | Full documentation |
| `PROJECT_SPECIFICATION.md` | Technical details |

---

## 🔗 Useful Links

- **ChessProgramming Wiki:** https://www.chessprogramming.org/
- **Lichess:** https://lichess.org/ (play online)
- **Chess.com:** https://www.chess.com/ (learn & play)

---

## 📞 Need Help?

1. **For gameplay:** Check "Tips for Playing" above
2. **For technical issues:** See "Troubleshooting" above
3. **For detailed info:** Read `README.md` or `PROJECT_SPECIFICATION.md`

---

## 🎓 Learning Resources

### Understanding the AI
- The AI uses **Minimax algorithm** with **Alpha-Beta pruning**
- It evaluates positions based on **material** and **piece placement**
- It searches 4-5 moves ahead (depth 4-5)

### Improving Your Play
- Play multiple games to learn patterns
- Analyze your moves after each game
- Study opening principles
- Practice tactics and puzzles

---

**Enjoy the game! ♟**

---

**Quick Commands:**

```bash
# Start web GUI
python server.py

# Start console game
chess_engine.exe

# Rebuild console game
g++ -std=c++17 -O2 -o chess_engine.exe main.cpp chess.cpp

# Stop web server
Ctrl+C (in terminal)
```

---

**Version:** 1.0  
**Last Updated:** November 21, 2025
