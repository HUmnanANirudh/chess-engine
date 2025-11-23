# 🎯 Chess Engine - START HERE

## Welcome to the Chess Engine Project!

This is a complete implementation of a chess game with an AI opponent that uses **Minimax algorithm with Alpha-Beta pruning** to play strong chess. The system includes **move probability analysis** that shows how correct each human move was based on the AI's deeper thinking.

---

## ⚡ Quick Start (Choose One)

### 🌐 **Option 1: Web GUI (Recommended for Playing)**
```bash
python server.py
```
- Opens in browser automatically
- Click pieces to play
- Beautiful modern interface
- **Best for:** Casual play and learning

### 💻 **Option 2: Console Game**
```bash
chess_engine.exe
```
- Text-based interface
- Enter moves like `e2e4`
- Full game analysis
- **Best for:** Testing and analysis

### 🎨 **Option 3: Qt GUI (Requires Qt5)**
```bash
build_gui.bat
chess_engine_gui.exe
```
- Desktop application
- Professional interface
- **Best for:** Advanced users

---

## 📚 Documentation Guide

| Document | Purpose | Read If... |
|----------|---------|-----------|
| **QUICKSTART.md** | 30-second setup | You want to play NOW |
| **README.md** | Feature overview | You want to know what's included |
| **GUI_README.md** | Web GUI guide | You're using the web interface |
| **PROJECT_SPECIFICATION.md** | Technical details | You want to understand the AI |
| **IMPLEMENTATION_STATUS.md** | What's done | You want to see completion status |

---

## 🎮 How to Play

### Web GUI
1. Run `python server.py`
2. Click a white piece to select it
3. Click a green highlighted square to move
4. AI (Black) responds automatically
5. Game ends on checkmate or stalemate

### Console
1. Run `chess_engine.exe`
2. Choose game mode (option 1 for Human vs AI)
3. Enter moves like `e2e4` (source + destination)
4. AI responds with its move
5. Game ends when checkmate/stalemate occurs

---

## 🤖 What Makes This Special

### Move Probability Analysis
After each game, the system shows how likely the AI would have played each of your moves:

```
Move Analysis:
Move      Score    Probability    Quality
e2e4      +35      85%            Excellent
e5        +20      72%            Good
Nf3       -10      45%            Acceptable
h6        -50      15%            Weak
```

**Interpretation:**
- **85%:** Your move matches AI thinking - strong play
- **45%:** Acceptable move with some deviation
- **15%:** Weak move, room for improvement

### Algorithm
The AI uses:
- **Minimax:** Evaluates all possible moves
- **Alpha-Beta Pruning:** Optimizes search (50-90% faster)
- **Piece-Square Tables:** Positional evaluation
- **Depth 5:** During gameplay (1-2 seconds per move)
- **Depth 7:** For post-game analysis (more thorough)

---

## 📊 Project Details

**Title:** Chess Engine: AI vs Human with Move Probability Analysis using Minimax & Alpha-Beta in C++

**Team Members:**
- Shuvam Bhatt (230457)
- Kushagra (230442)
- Anirudh (230466)
- Rohit Singh (230782)

**Type:** Prototype

**Language:** C++17

**Key Features:**
- ✅ Full chess rules implementation
- ✅ Minimax with Alpha-Beta pruning
- ✅ Move probability analysis
- ✅ Bitboard representation
- ✅ Piece-square tables
- ✅ Web GUI and console interface
- ✅ Post-game analysis
- ✅ Configurable difficulty levels

---

## 📁 File Structure

```
chess/
├── 🎮 Game Files
│   ├── chess_engine.exe          # Console executable
│   ├── main.cpp                  # Console game loop
│   ├── chess.h / chess.cpp       # Core engine
│
├── 🌐 Web GUI
│   ├── gui.html                  # Web interface
│   ├── chess_game.js             # Game logic
│   ├── server.py                 # Web server
│   ├── run_gui.bat               # GUI launcher
│
├── 🔧 Enhanced Version (Bitboards)
│   ├── chess_enhanced.h/cpp      # Bitboard implementation
│   ├── bitboard.h                # Bitboard utilities
│   ├── piece_square_tables.h     # PST arrays
│
├── 📖 Documentation
│   ├── START_HERE.md             # This file
│   ├── QUICKSTART.md             # 30-second guide
│   ├── README.md                 # Full overview
│   ├── PROJECT_SPECIFICATION.md  # Technical spec
│   ├── IMPLEMENTATION_STATUS.md  # Completion status
│   └── GUI_README.md             # GUI guide
│
└── 🛠️ Build Files
    ├── build.bat                 # Build script
    ├── CMakeLists.txt            # CMake config
    └── build_gui.bat             # Qt build script
```

---

## 🚀 Getting Started Steps

### Step 1: Choose Your Interface
- **Want to play casually?** → Use Web GUI (`python server.py`)
- **Want to test/analyze?** → Use Console (`chess_engine.exe`)
- **Want a desktop app?** → Use Qt GUI (requires Qt5)

### Step 2: Read the Right Guide
- **Quick setup?** → Read QUICKSTART.md (5 min)
- **Full features?** → Read README.md (10 min)
- **Technical details?** → Read PROJECT_SPECIFICATION.md (20 min)

### Step 3: Play!
- Start a game
- Make your moves
- Learn from the AI
- Review post-game analysis

### Step 4: Improve
- Play more games
- Study the move probabilities
- Learn opening principles
- Practice tactics

---

## 💡 Key Concepts

### Minimax Algorithm
The AI evaluates all possible moves and counter-moves to find the best strategy:
```
AI thinks: "If I play move X, human can play Y, then I play Z..."
It calculates the best outcome for each move
```

### Alpha-Beta Pruning
Optimization that eliminates unnecessary branches:
```
"I've found a move worth +50. If this branch can't beat +50, skip it."
Result: 50-90% faster search
```

### Move Probability
Shows how correct your move was:
```
High % = Your move matches AI thinking = Good play
Low % = Your move deviates from AI = Weak or creative play
```

---

## ⚙️ System Requirements

### For Web GUI
- Python 3.x
- Any modern web browser
- No installation needed

### For Console Game
- Windows with MinGW (already compiled)
- Or C++17 compiler to rebuild

### For Qt GUI
- Qt5 framework
- C++17 compiler

---

## 🎯 Difficulty Levels

| Level | Depth | Speed | Best For |
|-------|-------|-------|----------|
| Easy | 2 | Instant | Beginners |
| Medium | 3 | 0.5s | Intermediate |
| Hard | 4 | 1-2s | Advanced |
| Very Hard | 5 | 3-5s | Experts |

---

## 🔧 Troubleshooting

### "Port 8000 already in use"
Edit `server.py` and change `PORT = 8000` to another number

### "Python not found"
Install from https://www.python.org/

### "Game is slow"
Reduce difficulty level or close other applications

### "Can't find chess_engine.exe"
Rebuild with: `g++ -std=c++17 -O2 -o chess_engine.exe main.cpp chess.cpp`

---

## 📞 Need Help?

1. **Quick questions?** → Check QUICKSTART.md
2. **How to play?** → Check README.md
3. **Technical details?** → Check PROJECT_SPECIFICATION.md
4. **Status/completion?** → Check IMPLEMENTATION_STATUS.md
5. **GUI issues?** → Check GUI_README.md

---

## 🎓 Learning Path

### Beginner
1. Play a few games on Easy difficulty
2. Read QUICKSTART.md
3. Play on Medium difficulty
4. Review move probabilities

### Intermediate
1. Play on Hard difficulty
2. Read README.md
3. Study the move analysis
4. Learn opening principles

### Advanced
1. Play on Very Hard difficulty
2. Read PROJECT_SPECIFICATION.md
3. Study the source code
4. Understand Minimax and Alpha-Beta

---

## 🌟 Features Highlight

✨ **Smart AI**
- Evaluates millions of positions
- Plays strong, consistent chess
- Adapts to your moves

📊 **Move Analysis**
- Shows move probability
- Explains move quality
- Helps you improve

🎨 **Beautiful Interface**
- Modern web GUI
- Responsive design
- Easy to use

⚡ **Fast Performance**
- 1-2 seconds per move
- Efficient algorithms
- Optimized search

📚 **Complete Documentation**
- Multiple guides
- Technical specification
- Implementation details

---

## 🎮 Example Game

```
Starting position...

Move 1: You play e2e4 (85% - Excellent)
AI responds: e7e5

Move 2: You play g1f3 (92% - Excellent)
AI responds: b8c6

Move 3: You play f1b5 (78% - Good)
AI responds: a7a6

...game continues...

Game ends: Checkmate!

Post-Game Analysis:
Your moves averaged 72% probability
You played like the AI 72% of the time
Room for improvement: Opening theory
```

---

## 🚀 Next Steps

1. **Right now:** Choose an interface and play!
2. **After first game:** Review the move analysis
3. **After 5 games:** Read PROJECT_SPECIFICATION.md
4. **After 10 games:** Try harder difficulty levels
5. **Advanced:** Study the source code

---

## 📝 Project Status

✅ **COMPLETE AND READY TO USE**

All components implemented and tested:
- Core chess engine
- AI with Minimax + Alpha-Beta
- Move probability analysis
- Web GUI
- Console interface
- Complete documentation

---

## 🎯 Quick Links

| Action | Command |
|--------|---------|
| Play in browser | `python server.py` |
| Play in console | `chess_engine.exe` |
| Read quick guide | Open `QUICKSTART.md` |
| Read full docs | Open `README.md` |
| Understand AI | Open `PROJECT_SPECIFICATION.md` |
| Check status | Open `IMPLEMENTATION_STATUS.md` |

---

## 🎓 Educational Value

This project teaches:
- **Algorithms:** Minimax, Alpha-Beta pruning
- **Data Structures:** Bitboards, move lists
- **Game Theory:** Adversarial search, evaluation
- **C++:** Modern C++17 features
- **Software Design:** Modular architecture

---

## 📞 Support

**Having issues?**
1. Check the relevant documentation
2. Review QUICKSTART.md for common problems
3. Check source code comments
4. Try rebuilding the executable

**Want to improve?**
1. Study the source code
2. Read the specification
3. Experiment with parameters
4. Add new features

---

## 🎉 Ready to Play?

**Choose your interface:**

### 🌐 Web GUI (Easiest)
```bash
python server.py
```

### 💻 Console (Classic)
```bash
chess_engine.exe
```

### 🎨 Qt GUI (Advanced)
```bash
build_gui.bat
chess_engine_gui.exe
```

---

**Let's play chess! ♟**

---

**Version:** 1.0  
**Status:** Complete  
**Last Updated:** November 21, 2025

**Team:** Shuvam Bhatt, Kushagra, Anirudh, Rohit Singh
