const PIECES = { PAWN: 1, KNIGHT: 2, BISHOP: 3, ROOK: 4, QUEEN: 5, KING: 6 };
const COLORS = { WHITE: 0, BLACK: 1 };
const SYMBOLS = { 1: '♙♟', 2: '♘♞', 3: '♗♝', 4: '♖♜', 5: '♕♛', 6: '♔♚' };

class Game {
    constructor() {
        this.board = Array(64).fill(0);
        this.player = COLORS.WHITE;
        this.moves = [];
        this.selected = null;
        this.isThinking = false;
        this.gameOver = false;
        this.humanColor = COLORS.WHITE; // Player's color
        this.enPassantTarget = -1; // Square where en passant capture is possible
        // Castling rights tracking
        this.castlingRights = {
            whiteKingSide: true,
            whiteQueenSide: true,
            blackKingSide: true,
            blackQueenSide: true
        };
        this.init();
    }

    init() {
        console.log('INIT CALLED - Resetting board');
        console.trace(); // This will show where init was called from
        
        // Get player color choice
        const colorSelect = document.getElementById('playerColor');
        this.humanColor = colorSelect && colorSelect.value === 'black' ? COLORS.BLACK : COLORS.WHITE;
        
        this.board = Array(64).fill(0);
        const setup = [4,2,3,5,6,3,2,4, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 4,2,3,5,6,3,2,4];
        for (let i = 0; i < 64; i++) {
            if (setup[i]) this.board[i] = (setup[i] << 1) | (i < 16 ? COLORS.BLACK : COLORS.WHITE);
        }
        this.player = COLORS.WHITE;
        this.moves = [];
        this.isThinking = false;
        this.gameOver = false;
        this.enPassantTarget = -1;
        // Reset castling rights
        this.castlingRights = {
            whiteKingSide: true,
            whiteQueenSide: true,
            blackKingSide: true,
            blackQueenSide: true
        };
        this.render();
        const analysisDiv = document.getElementById('analysis');
        if (analysisDiv) {
            analysisDiv.textContent = 'Play the game. Analysis will be shown after game ends.';
        }
        
        // If player chose black, AI makes first move
        if (this.humanColor === COLORS.BLACK) {
            setTimeout(() => this.ai_move(), 500);
        }
    }

    piece(i) { return { type: this.board[i] >> 1, color: this.board[i] & 1 }; }
    set(i, type, color) { this.board[i] = (type << 1) | color; }

    moves_for(i) {
        const [r, c] = [Math.floor(i / 8), i % 8];
        const p = this.piece(i);
        if (!p.type) return [];
        const moves = [];
        const add = (nr, nc) => {
            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                const j = nr * 8 + nc;
                const t = this.piece(j);
                if (!t.type || t.color !== p.color) moves.push(j);
            }
        };

        if (p.type === PIECES.PAWN) {
            const d = p.color === COLORS.WHITE ? -1 : 1;
            const start = p.color === COLORS.WHITE ? 6 : 1;
            const nr = r + d;
            if (nr >= 0 && nr < 8 && !this.piece(nr * 8 + c).type) {
                add(nr, c);
                if (r === start && !this.piece((r + 2*d) * 8 + c).type) add(r + 2*d, c);
            }
            // Regular captures
            for (let dc of [-1, 1]) {
                const nj = (r + d) * 8 + (c + dc);
                if (r + d >= 0 && r + d < 8 && c + dc >= 0 && c + dc < 8) {
                    const t = this.piece(nj);
                    if (t.type && t.color !== p.color) moves.push(nj);
                }
            }
            // En passant capture
            if (this.enPassantTarget >= 0) {
                const epRow = Math.floor(this.enPassantTarget / 8);
                const epCol = this.enPassantTarget % 8;
                if (epRow === r + d && Math.abs(epCol - c) === 1) {
                    moves.push(this.enPassantTarget);
                }
            }
        } else if (p.type === PIECES.KNIGHT) {
            for (let [dr, dc] of [[-2,-1],[-2,1],[-1,-2],[-1,2],[1,-2],[1,2],[2,-1],[2,1]]) add(r+dr, c+dc);
        } else if (p.type === PIECES.BISHOP || p.type === PIECES.ROOK || p.type === PIECES.QUEEN) {
            const bishopDirs = [[1,1],[1,-1],[-1,1],[-1,-1]];
            const rookDirs = [[1,0],[-1,0],[0,1],[0,-1]];
            const dirs = p.type === PIECES.BISHOP ? bishopDirs :
                         p.type === PIECES.ROOK ? rookDirs :
                         bishopDirs.concat(rookDirs);
            for (let [dr, dc] of dirs) {
                for (let k = 1; k < 8; k++) {
                    const nr = r + dr*k, nc = c + dc*k;
                    if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) break;
                    const j = nr * 8 + nc;
                    const t = this.piece(j);
                    if (!t.type) moves.push(j);
                    else { if (t.color !== p.color) moves.push(j); break; }
                }
            }
        } else if (p.type === PIECES.KING) {
            const kingDirs = [[1,0],[-1,0],[0,1],[0,-1],[1,1],[1,-1],[-1,1],[-1,-1]];
            for (let [dr, dc] of kingDirs) {
                add(r + dr, c + dc);
            }
            
            // Castling moves
            if (p.color === COLORS.WHITE && r === 7 && c === 4) {
                // White king-side castling (e1-g1)
                if (this.castlingRights.whiteKingSide) {
                    if (!this.piece(61).type && !this.piece(62).type) { // f1 and g1 empty
                        const rook = this.piece(63);
                        if (rook.type === PIECES.ROOK && rook.color === COLORS.WHITE) {
                            // Check if king doesn't pass through check
                            if (!this.attacked(60, COLORS.BLACK) && !this.attacked(61, COLORS.BLACK) && !this.attacked(62, COLORS.BLACK)) {
                                moves.push(62); // g1
                            }
                        }
                    }
                }
                // White queen-side castling (e1-c1)
                if (this.castlingRights.whiteQueenSide) {
                    if (!this.piece(59).type && !this.piece(58).type && !this.piece(57).type) { // d1, c1, b1 empty
                        const rook = this.piece(56);
                        if (rook.type === PIECES.ROOK && rook.color === COLORS.WHITE) {
                            // Check if king doesn't pass through check
                            if (!this.attacked(60, COLORS.BLACK) && !this.attacked(59, COLORS.BLACK) && !this.attacked(58, COLORS.BLACK)) {
                                moves.push(58); // c1
                            }
                        }
                    }
                }
            } else if (p.color === COLORS.BLACK && r === 0 && c === 4) {
                // Black king-side castling (e8-g8)
                if (this.castlingRights.blackKingSide) {
                    if (!this.piece(5).type && !this.piece(6).type) { // f8 and g8 empty
                        const rook = this.piece(7);
                        if (rook.type === PIECES.ROOK && rook.color === COLORS.BLACK) {
                            // Check if king doesn't pass through check
                            if (!this.attacked(4, COLORS.WHITE) && !this.attacked(5, COLORS.WHITE) && !this.attacked(6, COLORS.WHITE)) {
                                moves.push(6); // g8
                            }
                        }
                    }
                }
                // Black queen-side castling (e8-c8)
                if (this.castlingRights.blackQueenSide) {
                    if (!this.piece(3).type && !this.piece(2).type && !this.piece(1).type) { // d8, c8, b8 empty
                        const rook = this.piece(0);
                        if (rook.type === PIECES.ROOK && rook.color === COLORS.BLACK) {
                            // Check if king doesn't pass through check
                            if (!this.attacked(4, COLORS.WHITE) && !this.attacked(3, COLORS.WHITE) && !this.attacked(2, COLORS.WHITE)) {
                                moves.push(2); // c8
                            }
                        }
                    }
                }
            }
        }
        return moves;
    }

    legal_moves() {
        const moves = [];
        for (let i = 0; i < 64; i++) {
            const p = this.piece(i);
            if (!p.type || p.color !== this.player) continue;
            for (let j of this.moves_for(i)) {
                const saved = this.board[j];
                this.board[j] = this.board[i];
                this.board[i] = 0;
                const king = this.find_king(p.color);
                const safe = !this.attacked(king, 1 - p.color);
                this.board[i] = this.board[j];
                this.board[j] = saved;
                if (safe) moves.push([i, j]);
            }
        }
        return moves;
    }

    find_king(color) {
        for (let i = 0; i < 64; i++) {
            const p = this.piece(i);
            if (p.type === PIECES.KING && p.color === color) return i;
        }
        return -1;
    }

    attacked(i, by_color) {
        for (let j = 0; j < 64; j++) {
            const p = this.piece(j);
            if (!p.type || p.color !== by_color) continue;
            // Use simple moves without castling to avoid infinite recursion
            if (this.moves_for_simple(j).includes(i)) return true;
        }
        return false;
    }
    
    moves_for_simple(i) {
        // Same as moves_for but WITHOUT castling (used for attack detection)
        const [r, c] = [Math.floor(i / 8), i % 8];
        const p = this.piece(i);
        if (!p.type) return [];
        const moves = [];
        const add = (nr, nc) => {
            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                const j = nr * 8 + nc;
                const t = this.piece(j);
                if (!t.type || t.color !== p.color) moves.push(j);
            }
        };

        if (p.type === PIECES.PAWN) {
            const d = p.color === COLORS.WHITE ? -1 : 1;
            const start = p.color === COLORS.WHITE ? 6 : 1;
            const nr = r + d;
            if (nr >= 0 && nr < 8 && !this.piece(nr * 8 + c).type) {
                add(nr, c);
                if (r === start && !this.piece((r + 2*d) * 8 + c).type) add(r + 2*d, c);
            }
            for (let dc of [-1, 1]) {
                const nj = (r + d) * 8 + (c + dc);
                if (r + d >= 0 && r + d < 8 && c + dc >= 0 && c + dc < 8) {
                    const t = this.piece(nj);
                    if (t.type && t.color !== p.color) moves.push(nj);
                }
            }
        } else if (p.type === PIECES.KNIGHT) {
            for (let [dr, dc] of [[-2,-1],[-2,1],[-1,-2],[-1,2],[1,-2],[1,2],[2,-1],[2,1]]) add(r+dr, c+dc);
        } else if (p.type === PIECES.BISHOP || p.type === PIECES.ROOK || p.type === PIECES.QUEEN) {
            const bishopDirs = [[1,1],[1,-1],[-1,1],[-1,-1]];
            const rookDirs = [[1,0],[-1,0],[0,1],[0,-1]];
            const dirs = p.type === PIECES.BISHOP ? bishopDirs :
                         p.type === PIECES.ROOK ? rookDirs :
                         bishopDirs.concat(rookDirs);
            for (let [dr, dc] of dirs) {
                for (let k = 1; k < 8; k++) {
                    const nr = r + dr*k, nc = c + dc*k;
                    if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) break;
                    const j = nr * 8 + nc;
                    const t = this.piece(j);
                    if (!t.type) moves.push(j);
                    else { if (t.color !== p.color) moves.push(j); break; }
                }
            }
        } else if (p.type === PIECES.KING) {
            const kingDirs = [[1,0],[-1,0],[0,1],[0,-1],[1,1],[1,-1],[-1,1],[-1,-1]];
            for (let [dr, dc] of kingDirs) {
                add(r + dr, c + dc);
            }
            // NO CASTLING in simple moves (to avoid infinite recursion with attacked())
        }
        return moves;
    }

    move(from, to, recordMove = true) {
        const piece = this.piece(from);
        
        // Handle en passant capture
        if (piece.type === PIECES.PAWN && to === this.enPassantTarget && this.enPassantTarget >= 0) {
            // Remove the captured pawn
            const capturedPawnRow = piece.color === COLORS.WHITE ? Math.floor(to / 8) + 1 : Math.floor(to / 8) - 1;
            const capturedPawnPos = capturedPawnRow * 8 + (to % 8);
            this.board[capturedPawnPos] = 0;
        }
        
        // Update en passant target
        if (piece.type === PIECES.PAWN && Math.abs(Math.floor(from / 8) - Math.floor(to / 8)) === 2) {
            // Pawn moved two squares, set en passant target
            this.enPassantTarget = piece.color === COLORS.WHITE ? from - 8 : from + 8;
        } else {
            this.enPassantTarget = -1;
        }
        
        // Check if this is a castling move
        if (piece.type === PIECES.KING) {
            const fromCol = from % 8;
            const toCol = to % 8;
            
            // King-side castling
            if (fromCol === 4 && toCol === 6) {
                // Move rook
                if (piece.color === COLORS.WHITE) {
                    this.board[61] = this.board[63]; // Move rook from h1 to f1
                    this.board[63] = 0;
                } else {
                    this.board[5] = this.board[7]; // Move rook from h8 to f8
                    this.board[7] = 0;
                }
            }
            // Queen-side castling
            else if (fromCol === 4 && toCol === 2) {
                // Move rook
                if (piece.color === COLORS.WHITE) {
                    this.board[59] = this.board[56]; // Move rook from a1 to d1
                    this.board[56] = 0;
                } else {
                    this.board[3] = this.board[0]; // Move rook from a8 to d8
                    this.board[0] = 0;
                }
            }
            
            // Update castling rights
            if (piece.color === COLORS.WHITE) {
                this.castlingRights.whiteKingSide = false;
                this.castlingRights.whiteQueenSide = false;
            } else {
                this.castlingRights.blackKingSide = false;
                this.castlingRights.blackQueenSide = false;
            }
        }
        
        // Update castling rights if rook moves
        if (piece.type === PIECES.ROOK) {
            if (piece.color === COLORS.WHITE) {
                if (from === 56) this.castlingRights.whiteQueenSide = false; // a1
                if (from === 63) this.castlingRights.whiteKingSide = false; // h1
            } else {
                if (from === 0) this.castlingRights.blackQueenSide = false; // a8
                if (from === 7) this.castlingRights.blackKingSide = false; // h8
            }
        }
        
        // Update castling rights if rook is captured
        if (to === 56) this.castlingRights.whiteQueenSide = false;
        if (to === 63) this.castlingRights.whiteKingSide = false;
        if (to === 0) this.castlingRights.blackQueenSide = false;
        if (to === 7) this.castlingRights.blackKingSide = false;
        
        this.board[to] = this.board[from];
        this.board[from] = 0;
        
        // Pawn promotion
        if (piece.type === PIECES.PAWN) {
            const toRow = Math.floor(to / 8);
            if (toRow === 0 || toRow === 7) {
                // Promote to queen (automatic promotion for simplicity)
                this.board[to] = (PIECES.QUEEN << 1) | piece.color;
            }
        }
        
        // Only record move in history for actual game moves, not during evaluation
        if (recordMove) {
            this.moves.push([from, to]);
        }
        
        this.player = 1 - this.player;
    }

    eval() {
        // GOAL-BASED EVALUATION: Maximum points for checkmate
        // Note: Checkmate detection is handled in minimax, not here for performance
        
        const vals = [0, 1, 3, 3.2, 5, 9, 200];
        let score = 0;
        
        // Material evaluation
        for (let i = 0; i < 64; i++) {
            const p = this.piece(i);
            if (p.type) {
                const materialValue = vals[p.type];
                score += materialValue * (p.color === COLORS.WHITE ? 1 : -1);
                
                // Piece-square tables for positional play
                const [r, c] = [Math.floor(i / 8), i % 8];
                let positional = 0;
                
                if (p.type === PIECES.PAWN) {
                    const pawnTable = [
                        0,  0,  0,  0,  0,  0,  0,  0,
                        5, 10, 10,-20,-20, 10, 10,  5,
                        5, -5,-10,  0,  0,-10, -5,  5,
                        0,  0,  0, 20, 20,  0,  0,  0,
                        5,  5, 10, 25, 25, 10,  5,  5,
                       10, 10, 20, 30, 30, 20, 10, 10,
                       50, 50, 50, 50, 50, 50, 50, 50,
                        0,  0,  0,  0,  0,  0,  0,  0
                    ];
                    positional = p.color === COLORS.WHITE ? 
                        pawnTable[i] / 100 : pawnTable[63 - i] / 100;
                } else if (p.type === PIECES.KNIGHT) {
                    const knightTable = [
                       -50,-40,-30,-30,-30,-30,-40,-50,
                       -40,-20,  0,  5,  5,  0,-20,-40,
                       -30,  5, 10, 15, 15, 10,  5,-30,
                       -30,  0, 15, 20, 20, 15,  0,-30,
                       -30,  5, 15, 20, 20, 15,  5,-30,
                       -30,  0, 10, 15, 15, 10,  0,-30,
                       -40,-20,  0,  0,  0,  0,-20,-40,
                       -50,-40,-30,-30,-30,-30,-40,-50
                    ];
                    positional = p.color === COLORS.WHITE ? 
                        knightTable[i] / 100 : knightTable[63 - i] / 100;
                } else if (p.type === PIECES.BISHOP) {
                    positional = 0.1;
                } else if (p.type === PIECES.KING) {
                    const kingTable = [
                        20, 30, 10,  0,  0, 10, 30, 20,
                        20, 20,  0,  0,  0,  0, 20, 20,
                       -10,-20,-20,-20,-20,-20,-20,-10,
                       -20,-30,-30,-40,-40,-30,-30,-20,
                       -30,-40,-40,-50,-50,-40,-40,-30,
                       -30,-40,-40,-50,-50,-40,-40,-30,
                       -30,-40,-40,-50,-50,-40,-40,-30,
                       -30,-40,-40,-50,-50,-40,-40,-30
                    ];
                    positional = p.color === COLORS.WHITE ? 
                        kingTable[i] / 100 : kingTable[63 - i] / 100;
                }
                
                score += positional * (p.color === COLORS.WHITE ? 1 : -1);
            }
        }
        
        // GOAL: Aggressive play - reward attacking opponent king (lightweight check)
        const whiteKing = this.find_king(COLORS.WHITE);
        const blackKing = this.find_king(COLORS.BLACK);
        
        // Simple king pressure without expensive legal move calculation
        let kingPressure = 0;
        for (let i = 0; i < 64; i++) {
            const p = this.piece(i);
            if (p.type && p.type !== PIECES.KING) {
                const [r, c] = [Math.floor(i / 8), i % 8];
                const targetKing = p.color === COLORS.WHITE ? blackKing : whiteKing;
                const [kr, kc] = [Math.floor(targetKing / 8), targetKing % 8];
                
                // Reward pieces close to opponent king
                const dist = Math.abs(r - kr) + Math.abs(c - kc);
                if (dist <= 3) {
                    kingPressure += (4 - dist) * 0.2 * (p.color === COLORS.WHITE ? 1 : -1);
                }
            }
        }
        score += kingPressure;
        
        // GOAL: Reward checking the opponent (lightweight)
        const whiteInCheck = this.attacked(whiteKing, COLORS.BLACK) ? 1 : 0;
        const blackInCheck = this.attacked(blackKing, COLORS.WHITE) ? 1 : 0;
        score += (blackInCheck - whiteInCheck) * 5; // Strong bonus for check
        
        return score;
    }

    render() {
        const board = document.getElementById('board');
        board.innerHTML = '';
        // Apply flipped class if human is playing as Black
        board.className = this.humanColor === COLORS.BLACK ? 'chessboard flipped' : 'chessboard';
        for (let i = 0; i < 64; i++) {
            const sq = document.createElement('div');
            const [r, c] = [Math.floor(i / 8), i % 8];
            sq.className = `square ${(r + c) % 2 ? 'dark' : 'light'}`;
            const p = this.piece(i);
            if (p.type) sq.textContent = SYMBOLS[p.type][p.color];
            sq.onclick = () => this.click(i);
            board.appendChild(sq);
        }
        this.update_status();
        this.update_eval_bar();
    }

    click(i) {
        // Only allow clicks when it's the human's turn
        if (this.player !== this.humanColor || this.isThinking || this.gameOver) {
            console.log('Click blocked:', {
                player: this.player,
                humanColor: this.humanColor,
                isThinking: this.isThinking,
                gameOver: this.gameOver
            });
            return;
        }
        
        if (!this.selected) {
            const p = this.piece(i);
            if (p.type && p.color === this.humanColor) {
                this.selected = i;
                const squares = document.querySelectorAll('.square');
                squares[i].classList.add('selected');
                const validMoves = this.moves_for(i);
                const legalMoves = this.legal_moves().filter(m => m[0] === i).map(m => m[1]);
                for (let j of legalMoves) {
                    squares[j].classList.add('highlight');
                }
            }
        } else {
            const moves = this.legal_moves();
            const found = moves.find(m => m[0] === this.selected && m[1] === i);
            document.querySelectorAll('.square').forEach(s => s.classList.remove('selected', 'highlight'));
            
            if (found) {
                this.move(this.selected, i);
                this.selected = null;
                this.render();
                // Don't call update_status() again - render() already calls it
                setTimeout(() => this.ai_move(), 100);
            } else {
                const p = this.piece(i);
                if (p.type && p.color === this.humanColor) {
                    this.selected = i;
                    const squares = document.querySelectorAll('.square');
                    squares[i].classList.add('selected');
                    const legalMoves = this.legal_moves().filter(m => m[0] === i).map(m => m[1]);
                    for (let j of legalMoves) {
                        squares[j].classList.add('highlight');
                    }
                } else {
                    this.selected = null;
                }
            }
        }
    }

    ai_move() {
        if (this.gameOver) return;
        
        // Only make a move if it's the AI's turn
        if (this.player === this.humanColor) return;
        
        this.isThinking = true;
        const statusEl = document.getElementById('status');
        const difficultyName = ['Beginner', 'Easy', 'Medium', 'Hard', 'Expert'][parseInt(document.getElementById('difficulty').value) - 1] || 'Medium';
        statusEl.textContent = `AI is thinking (${difficultyName})...`;
        statusEl.classList.add('thinking');
        
        setTimeout(() => {
            const moves = this.legal_moves();
            if (!moves.length) {
                this.gameOver = true;
                const inCheck = this.attacked(this.find_king(this.player), 1 - this.player);
                statusEl.classList.remove('thinking');
                if (inCheck) {
                    statusEl.textContent = 'Checkmate! ' + (this.player === COLORS.WHITE ? 'Black Wins!' : 'White Wins!');
                } else {
                    statusEl.textContent = 'Stalemate! Draw!';
                }
                this.isThinking = false;
                this.post_game_analysis();
                return;
            }

            // GOAL: Check for immediate checkmate first!
            for (const [from, to] of moves) {
                const savedBoard = this.board.slice();
                const savedPlayer = this.player;
                const savedCastling = JSON.parse(JSON.stringify(this.castlingRights));
                const savedEnPassant = this.enPassantTarget;
                
                this.move(from, to, false); // Don't record during testing
                
                const oppMoves = this.legal_moves();
                if (oppMoves.length === 0) {
                    const oppKing = this.find_king(this.player);
                    const inCheck = this.attacked(oppKing, 1 - this.player);
                    
                    // Restore state
                    this.board = savedBoard;
                    this.player = savedPlayer;
                    this.castlingRights = savedCastling;
                    this.enPassantTarget = savedEnPassant;
                    
                    if (inCheck) {
                        // IMMEDIATE CHECKMATE FOUND!
                        console.log('🎯 IMMEDIATE CHECKMATE FOUND!');
                        this.move(from, to, true); // Now record the actual move
                        this.isThinking = false;
                        statusEl.classList.remove('thinking');
                        this.render();
                        return;
                    }
                }
                
                // Restore state
                this.board = savedBoard;
                this.player = savedPlayer;
                this.castlingRights = savedCastling;
                this.enPassantTarget = savedEnPassant;
            }

            const depth = parseInt(document.getElementById('difficulty').value, 10) || 3;
            let nodesEvaluated = 0;
            const maxNodes = 50000; // Safety limit

            // GOAL-BASED MINIMAX: Prefer faster checkmates, avoid slower ones
            const minimax = (depth, alpha, beta, maximizing, plyFromRoot) => {
                nodesEvaluated++;
                
                // Safety check to prevent hanging
                if (nodesEvaluated > maxNodes) {
                    return this.eval();
                }
                
                const legal = this.legal_moves();
                
                if (legal.length === 0) {
                    // Terminal position - checkmate or stalemate
                    const kingPos = this.find_king(this.player);
                    const inCheck = this.attacked(kingPos, 1 - this.player);
                    
                    if (inCheck) {
                        // Checkmate - adjust score by depth to prefer faster mates
                        const mateScore = 100000;
                        return maximizing ? -(mateScore - plyFromRoot) : (mateScore - plyFromRoot);
                    } else {
                        // Stalemate
                        return 0;
                    }
                }
                
                if (depth === 0) {
                    return this.eval();
                }

                if (maximizing) {
                    let maxEval = -Infinity;
                    for (const [from, to] of legal) {
                        const savedBoard = this.board.slice();
                        const savedPlayer = this.player;
                        const savedCastling = JSON.parse(JSON.stringify(this.castlingRights));
                        const savedEnPassant = this.enPassantTarget;
                        
                        this.move(from, to, false); // Use move() to handle en passant and promotion

                        const evalScore = minimax(depth - 1, alpha, beta, false, plyFromRoot + 1);

                        this.board = savedBoard;
                        this.player = savedPlayer;
                        this.castlingRights = savedCastling;
                        this.enPassantTarget = savedEnPassant;

                        if (evalScore > maxEval) maxEval = evalScore;
                        if (evalScore > alpha) alpha = evalScore;
                        if (beta <= alpha) break; // Alpha-beta pruning
                    }
                    return maxEval;
                } else {
                    let minEval = Infinity;
                    for (const [from, to] of legal) {
                        const savedBoard = this.board.slice();
                        const savedPlayer = this.player;
                        const savedCastling = JSON.parse(JSON.stringify(this.castlingRights));
                        const savedEnPassant = this.enPassantTarget;
                        
                        this.move(from, to, false); // Use move() to handle en passant and promotion

                        const evalScore = minimax(depth - 1, alpha, beta, true, plyFromRoot + 1);

                        this.board = savedBoard;
                        this.player = savedPlayer;
                        this.castlingRights = savedCastling;
                        this.enPassantTarget = savedEnPassant;

                        if (evalScore < minEval) minEval = evalScore;
                        if (evalScore < beta) beta = evalScore;
                        if (beta <= alpha) break; // Alpha-beta pruning
                    }
                    return minEval;
                }
            };

            // GOAL: Find best move - prioritize checkmate above all else
            let bestMove = moves[0];
            let bestEval = this.player === COLORS.BLACK ? Infinity : -Infinity;
            
            for (const [from, to] of moves) {
                const savedBoard = this.board.slice();
                const savedPlayer = this.player;
                const savedCastling = JSON.parse(JSON.stringify(this.castlingRights));
                const savedEnPassant = this.enPassantTarget;
                
                this.move(from, to, false); // Use move() to handle en passant and promotion

                const evalScore = minimax(depth - 1, -Infinity, Infinity, this.player === COLORS.WHITE, 1);

                this.board = savedBoard;
                this.player = savedPlayer;
                this.castlingRights = savedCastling;
                this.enPassantTarget = savedEnPassant;

                // Black AI wants to minimize (from white's perspective)
                if (this.player === COLORS.BLACK) {
                    if (evalScore < bestEval) {
                        bestEval = evalScore;
                        bestMove = [from, to];
                    }
                } else {
                    if (evalScore > bestEval) {
                        bestEval = evalScore;
                        bestMove = [from, to];
                    }
                }
            }

            // Display goal achievement
            if (Math.abs(bestEval) > 90000) {
                console.log('🎯 GOAL: Checkmate sequence found! Score:', bestEval);
            }
            console.log(`AI evaluated ${nodesEvaluated} positions at depth ${depth}`);

            this.move(bestMove[0], bestMove[1]);
            this.isThinking = false;
            statusEl.classList.remove('thinking');
            this.render();
        }, 50);
    }

    update_status() {
        if (this.gameOver) return;
        
        const moves = this.legal_moves();
        const status = document.getElementById('status');
        if (!moves.length) {
            this.gameOver = true;
            const inCheck = this.attacked(this.find_king(this.player), 1 - this.player);
            if (inCheck) {
                status.textContent = 'Checkmate! ' + (this.player === COLORS.WHITE ? 'Black Wins!' : 'White Wins!');
            } else {
                status.textContent = 'Stalemate! Draw!';
            }
            this.post_game_analysis();
        } else {
            status.textContent = (this.player === COLORS.WHITE ? 'White' : 'Black') + ' to Move';
        }
        const hist = document.getElementById('moves');
        hist.innerHTML = this.moves.map((m, i) => {
            const from = String.fromCharCode(97 + (m[0] % 8)) + (8 - Math.floor(m[0] / 8));
            const to = String.fromCharCode(97 + (m[1] % 8)) + (8 - Math.floor(m[1] / 8));
            return `${i % 2 ? '' : '<br>'}<strong>${Math.floor(i/2)+1}.</strong> ${from}${to}`;
        }).join(' ');
    }

    post_game_analysis() {
        const analysisDiv = document.getElementById('analysis');
        if (!analysisDiv || !this.gameOver) return;

        // Only analyze if there is at least one move
        if (this.moves.length === 0) {
            analysisDiv.textContent = 'No moves to analyze.';
            return;
        }

        // Only proceed after game is completely over
        if (!this.gameOver) return;

        analysisDiv.innerHTML = '<p>Analyzing your moves with Stockfish...</p>';

        // Analyze ONLY human moves
        // If human is White: analyze even indices (0, 2, 4, ...)
        // If human is Black: analyze odd indices (1, 3, 5, ...)
        const startIndex = this.humanColor === COLORS.WHITE ? 0 : 1;
        const positionsToAnalyze = [];
        
        for (let i = startIndex; i < this.moves.length; i += 2) {
            if (i >= this.moves.length) break;
            
            // Build position before human's move
            const tempBoard = Array(64).fill(0);
            const setup = [4,2,3,5,6,3,2,4, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 4,2,3,5,6,3,2,4];
            for (let k = 0; k < 64; k++) {
                if (setup[k]) tempBoard[k] = (setup[k] << 1) | (k < 16 ? COLORS.BLACK : COLORS.WHITE);
            }
            
            let tempPlayer = COLORS.WHITE;
            let tempCastling = {
                whiteKingSide: true,
                whiteQueenSide: true,
                blackKingSide: true,
                blackQueenSide: true
            };
            let tempEnPassant = -1;
            
            // Replay moves up to (but not including) this human move
            for (let j = 0; j < i; j++) {
                const from = this.moves[j][0];
                const to = this.moves[j][1];
                const piece = tempBoard[from];
                const pieceType = piece >> 1;
                const pieceColor = piece & 1;
                
                // Handle castling rook movement
                if (pieceType === PIECES.KING) {
                    const fromCol = from % 8;
                    const toCol = to % 8;
                    if (fromCol === 4 && toCol === 6) {
                        if (pieceColor === COLORS.WHITE) {
                            tempBoard[61] = tempBoard[63];
                            tempBoard[63] = 0;
                        } else {
                            tempBoard[5] = tempBoard[7];
                            tempBoard[7] = 0;
                        }
                    } else if (fromCol === 4 && toCol === 2) {
                        if (pieceColor === COLORS.WHITE) {
                            tempBoard[59] = tempBoard[56];
                            tempBoard[56] = 0;
                        } else {
                            tempBoard[3] = tempBoard[0];
                            tempBoard[0] = 0;
                        }
                    }
                    if (pieceColor === COLORS.WHITE) {
                        tempCastling.whiteKingSide = false;
                        tempCastling.whiteQueenSide = false;
                    } else {
                        tempCastling.blackKingSide = false;
                        tempCastling.blackQueenSide = false;
                    }
                }
                
                // Handle en passant
                if (pieceType === PIECES.PAWN) {
                    const distance = Math.abs(Math.floor(to / 8) - Math.floor(from / 8));
                    if (distance === 2) {
                        tempEnPassant = pieceColor === COLORS.WHITE ? to + 8 : to - 8;
                    } else if (tempEnPassant >= 0 && to === tempEnPassant) {
                        const captureSquare = pieceColor === COLORS.WHITE ? to + 8 : to - 8;
                        tempBoard[captureSquare] = 0;
                        tempEnPassant = -1;
                    } else {
                        tempEnPassant = -1;
                    }
                } else {
                    tempEnPassant = -1;
                }
                
                tempBoard[to] = tempBoard[from];
                tempBoard[from] = 0;
                tempPlayer = 1 - tempPlayer;
            }
            
            const fen = this.boardToFEN(tempBoard, tempPlayer, tempCastling, tempEnPassant);
            const humanMove = this.algebraic(this.moves[i][0]) + this.algebraic(this.moves[i][1]);
            
            positionsToAnalyze.push({
                moveNum: Math.floor(i / 2) + 1,
                fen: fen,
                humanMove: humanMove
            });
        }

        // Analyze positions sequentially
        let analysisResults = [];
        let currentIndex = 0;
        let failedRequests = 0;

        const analyzeNext = () => {
            if (currentIndex >= positionsToAnalyze.length) {
                if (analysisResults.length === 0 && failedRequests > 0) {
                    analysisDiv.innerHTML = '<p style="color: #e74c3c;">Analysis failed. The Stockfish API may be unavailable or blocked by CORS policy.</p>';
                } else {
                    this.displayAnalysisResults(analysisResults);
                }
                return;
            }

            const pos = positionsToAnalyze[currentIndex];
            const url = `https://stockfish.online/api/s/v2.php?fen=${encodeURIComponent(pos.fen)}&depth=12`;

            fetch(url)
                .then(response => {
                    if (!response.ok) {
                        throw new Error(`HTTP error! status: ${response.status}`);
                    }
                    return response.json();
                })
                .then(data => {
                    console.log('API response:', data);
                    if (data.success) {
                        analysisResults.push({
                            moveNum: pos.moveNum,
                            humanMove: pos.humanMove,
                            eval: data.evaluation,
                            mate: data.mate,
                            bestMove: data.bestmove,
                            continuation: data.continuation
                        });
                    } else {
                        failedRequests++;
                        console.warn('API returned success=false for move', pos.moveNum);
                    }
                    currentIndex++;
                    analyzeNext();
                })
                .catch(error => {
                    failedRequests++;
                    console.error('Analysis error for move', pos.moveNum, ':', error);
                    currentIndex++;
                    analyzeNext();
                });
        };

        analyzeNext();
    }

    boardToFEN(board, player, castlingRights, enPassantTarget) {
        let fen = '';
        for (let rank = 0; rank < 8; rank++) {
            let emptyCount = 0;
            for (let file = 0; file < 8; file++) {
                const idx = rank * 8 + file;
                const piece = board[idx];
                if (piece === 0) {
                    emptyCount++;
                } else {
                    if (emptyCount > 0) {
                        fen += emptyCount;
                        emptyCount = 0;
                    }
                    const type = piece >> 1;
                    const color = piece & 1;
                    const pieceChar = ['', 'p', 'n', 'b', 'r', 'q', 'k'][type];
                    fen += color === COLORS.WHITE ? pieceChar.toUpperCase() : pieceChar;
                }
            }
            if (emptyCount > 0) fen += emptyCount;
            if (rank < 7) fen += '/';
        }
        
        fen += player === COLORS.WHITE ? ' w ' : ' b ';
        
        let castling = '';
        if (castlingRights.whiteKingSide) castling += 'K';
        if (castlingRights.whiteQueenSide) castling += 'Q';
        if (castlingRights.blackKingSide) castling += 'k';
        if (castlingRights.blackQueenSide) castling += 'q';
        fen += castling || '-';
        
        fen += ' ';
        fen += enPassantTarget !== null ? this.algebraic(enPassantTarget) : '-';
        fen += ' 0 1';
        
        return fen;
    }

    algebraic(sq) {
        const file = String.fromCharCode(97 + (sq % 8));
        const rank = 8 - Math.floor(sq / 8);
        return file + rank;
    }

    displayAnalysisResults(results) {
        const analysisDiv = document.getElementById('analysis');
        if (results.length === 0) {
            // This shouldn't happen now as we handle it in analyzeNext, but keep as fallback
            analysisDiv.innerHTML = '<p style="color: #e74c3c;">No moves could be analyzed. The API may be unavailable.</p>';
            return;
        }

        let html = '<h4 style="margin-bottom: 10px;">Your Move Analysis</h4>';
        html += '<div style="font-size: 12px; line-height: 1.6; max-height: 300px; overflow-y: auto;">';
        
        let previousEval = null;
        
        results.forEach((result, index) => {
            const currentEval = result.mate !== null ? (result.mate > 0 ? 10000 : -10000) : (result.eval || 0);
            
            // Extract best move
            const bestMoveMatch = result.bestMove ? result.bestMove.match(/bestmove\s+(\S+)/) : null;
            const bestMove = bestMoveMatch ? bestMoveMatch[1] : 'N/A';
            
            // Determine if this was a mistake (reduced winning chances)
            let quality = '';
            let color = '#f8f9fa';
            
            if (previousEval !== null) {
                const evalDrop = previousEval - currentEval; // Positive means position got worse for White
                
                if (evalDrop > 3.0) {
                    quality = ' - Blunder!';
                    color = '#ffebee';
                } else if (evalDrop > 1.5) {
                    quality = ' - Mistake';
                    color = '#fff3e0';
                } else if (evalDrop > 0.5) {
                    quality = ' - Inaccuracy';
                    color = '#fffde7';
                } else if (evalDrop < -0.5) {
                    quality = ' - Excellent!';
                    color = '#e8f5e9';
                } else {
                    quality = ' - Good';
                    color = '#f1f8ff';
                }
            }
            
            const evalText = result.mate !== null 
                ? `Mate in ${Math.abs(result.mate)}` 
                : `${currentEval > 0 ? '+' : ''}${currentEval.toFixed(1)}`;
            
            html += `<div style="margin-bottom: 6px; padding: 8px; background: ${color}; border-radius: 4px; border-left: 3px solid #3498db;">`;
            html += `<strong>Move ${result.moveNum}:</strong> ${result.humanMove}${quality}<br>`;
            html += `<span style="font-size: 11px; color: #555;">Position: ${evalText} | Best was: ${bestMove}</span>`;
            html += `</div>`;
            
            previousEval = currentEval;
        });
        
        html += '</div>';
        analysisDiv.innerHTML = html;
    }

    update_eval_bar() {
        const whiteBar = document.getElementById('evalWhite');
        const blackBar = document.getElementById('evalBlack');
        const scoreSpan = document.getElementById('evalScore');
        if (!whiteBar || !blackBar || !scoreSpan) return;

        const pawnScore = this.eval(); // white-positive material score in pawns
        const cpScore = pawnScore * 100; // approximate centipawns
        const maxCp = 800;
        const clamped = Math.max(-maxCp, Math.min(maxCp, cpScore));
        const whitePercent = 50 + (clamped / maxCp) * 50; // 0..100
        const whiteClamped = Math.max(0, Math.min(100, whitePercent));
        const blackClamped = 100 - whiteClamped;

        whiteBar.style.height = whiteClamped + '%';
        blackBar.style.height = blackClamped + '%';

        const pawnEval = cpScore / 100.0;
        const sign = pawnEval > 0 ? '+' : pawnEval < 0 ? '' : '';
        scoreSpan.textContent = sign + pawnEval.toFixed(2);
    }
}

const game = new Game();
document.getElementById('newBtn').onclick = () => game.init();

// When player changes color, start a new game
document.getElementById('playerColor').onchange = () => game.init();

// Remove undo button code since it doesn't exist in HTML

