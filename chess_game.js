const PIECES = { PAWN: 1, KNIGHT: 2, BISHOP: 3, ROOK: 4, QUEEN: 5, KING: 6 };
const COLORS = { WHITE: 0, BLACK: 1 };
const SYMBOLS = { 1: '♙♟', 2: '♘♞', 3: '♗♝', 4: '♖♜', 5: '♕♛', 6: '♔♚' };

class Game {
    constructor() {
        this.board = Array(64).fill(0);
        this.player = COLORS.WHITE;
        this.moves = [];
        this.selected = null;
        this.init();
    }

    init() {
        this.board = Array(64).fill(0);
        const setup = [4,2,3,5,6,3,2,4, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 4,2,3,5,6,3,2,4];
        for (let i = 0; i < 64; i++) {
            if (setup[i]) this.board[i] = (setup[i] << 1) | (i < 16 ? COLORS.BLACK : COLORS.WHITE);
        }
        this.player = COLORS.WHITE;
        this.moves = [];
        this.render();
        const analysisDiv = document.getElementById('analysis');
        if (analysisDiv) {
            analysisDiv.textContent = 'Play a full game to see move probabilities here.';
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
            if (this.moves_for(j).includes(i)) return true;
        }
        return false;
    }

    move(from, to) {
        this.board[to] = this.board[from];
        this.board[from] = 0;
        this.moves.push([from, to]);
        this.player = 1 - this.player;
    }

    eval() {
        const vals = [0, 1, 3, 3, 5, 9, 0];
        let score = 0;
        for (let i = 0; i < 64; i++) {
            const p = this.piece(i);
            if (p.type) score += vals[p.type] * (p.color === COLORS.WHITE ? 1 : -1);
        }
        return score;
    }

    render() {
        const board = document.getElementById('board');
        board.innerHTML = '';
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
        if (this.player !== COLORS.WHITE) return;
        if (!this.selected) {
            const p = this.piece(i);
            if (p.type && p.color === COLORS.WHITE) {
                this.selected = i;
                document.querySelectorAll('.square')[i].classList.add('selected');
                for (let j of this.moves_for(i)) {
                    document.querySelectorAll('.square')[j].classList.add('highlight');
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
                // Update live analysis after each human move
                this.post_game_analysis();
                setTimeout(() => this.ai_move(), 500);
            } else {
                const p = this.piece(i);
                if (p.type && p.color === COLORS.WHITE) {
                    this.selected = i;
                    document.querySelectorAll('.square')[i].classList.add('selected');
                    for (let j of this.moves_for(i)) {
                        document.querySelectorAll('.square')[j].classList.add('highlight');
                    }
                } else {
                    this.selected = null;
                }
            }
        }
    }

    ai_move() {
        const moves = this.legal_moves();
        if (!moves.length) {
            document.getElementById('status').textContent = this.player === COLORS.WHITE ? 'Black Wins!' : 'White Wins!';
            this.post_game_analysis();
            return;
        }

        const depth = parseInt(document.getElementById('difficulty').value, 10) || 3; // search depth for AI

        const minimax = (depth, alpha, beta, maximizing) => {
            const legal = this.legal_moves();
            if (depth === 0 || !legal.length) {
                return this.eval();
            }

            if (maximizing) {
                let maxEval = -Infinity;
                for (const [from, to] of legal) {
                    const savedFrom = this.board[from];
                    const savedTo = this.board[to];
                    this.board[to] = this.board[from];
                    this.board[from] = 0;
                    this.player = 1 - this.player;

                    const evalScore = minimax(depth - 1, alpha, beta, false);

                    this.player = 1 - this.player;
                    this.board[from] = savedFrom;
                    this.board[to] = savedTo;

                    if (evalScore > maxEval) maxEval = evalScore;
                    if (evalScore > alpha) alpha = evalScore;
                    if (beta <= alpha) break;
                }
                return maxEval;
            } else {
                let minEval = Infinity;
                for (const [from, to] of legal) {
                    const savedFrom = this.board[from];
                    const savedTo = this.board[to];
                    this.board[to] = this.board[from];
                    this.board[from] = 0;
                    this.player = 1 - this.player;

                    const evalScore = minimax(depth - 1, alpha, beta, true);

                    this.player = 1 - this.player;
                    this.board[from] = savedFrom;
                    this.board[to] = savedTo;

                    if (evalScore < minEval) minEval = evalScore;
                    if (evalScore < beta) beta = evalScore;
                    if (beta <= alpha) break;
                }
                return minEval;
            }
        };

        let bestMove = moves[0];
        let bestEval = Infinity;
        // Black is minimizing player in eval(): white positive, black negative
        for (const [from, to] of moves) {
            const savedFrom = this.board[from];
            const savedTo = this.board[to];
            this.board[to] = this.board[from];
            this.board[from] = 0;
            this.player = 1 - this.player;

            const evalScore = minimax(depth - 1, -Infinity, Infinity, true);

            this.player = 1 - this.player;
            this.board[from] = savedFrom;
            this.board[to] = savedTo;

            if (evalScore < bestEval) {
                bestEval = evalScore;
                bestMove = [from, to];
            }
        }

        this.move(bestMove[0], bestMove[1]);
        this.render();
    }

    update_status() {
        const moves = this.legal_moves();
        const status = document.getElementById('status');
        if (!moves.length) {
            status.textContent = this.player === COLORS.WHITE ? 'Black Wins!' : 'White Wins!';
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
        if (!analysisDiv) return;

        // Only analyze if there is at least one move
        if (this.moves.length === 0) {
            return;
        }

        // Save current state
        const savedBoard = this.board.slice();
        const savedPlayer = this.player;
        const savedMoves = this.moves.slice();
        const savedRender = this.render;

        // Disable rendering during analysis to avoid flicker
        this.render = () => {};

        const allMoves = savedMoves.slice();
        const rows = [];

        const algebraic = (sq) => {
            const file = String.fromCharCode(97 + (sq % 8));
            const rank = 8 - Math.floor(sq / 8);
            return file + rank;
        };

        const analysisDepth = 4; // deeper search for analysis in GUI

        const minimax = (depth, alpha, beta, maximizing) => {
            const legal = this.legal_moves();
            if (depth === 0 || !legal.length) {
                return this.eval();
            }

            if (maximizing) {
                let maxEval = -Infinity;
                for (const [from, to] of legal) {
                    const savedFrom = this.board[from];
                    const savedTo = this.board[to];
                    this.board[to] = this.board[from];
                    this.board[from] = 0;
                    this.player = 1 - this.player;

                    const evalScore = minimax(depth - 1, alpha, beta, false);

                    this.player = 1 - this.player;
                    this.board[from] = savedFrom;
                    this.board[to] = savedTo;

                    if (evalScore > maxEval) maxEval = evalScore;
                    if (evalScore > alpha) alpha = evalScore;
                    if (beta <= alpha) break;
                }
                return maxEval;
            } else {
                let minEval = Infinity;
                for (const [from, to] of legal) {
                    const savedFrom = this.board[from];
                    const savedTo = this.board[to];
                    this.board[to] = this.board[from];
                    this.board[from] = 0;
                    this.player = 1 - this.player;

                    const evalScore = minimax(depth - 1, alpha, beta, true);

                    this.player = 1 - this.player;
                    this.board[from] = savedFrom;
                    this.board[to] = savedTo;

                    if (evalScore < minEval) minEval = evalScore;
                    if (evalScore < beta) beta = evalScore;
                    if (beta <= alpha) break;
                }
                return minEval;
            }
        };

        const evaluateAllMoves = (depth) => {
            const results = [];
            const legal = this.legal_moves();
            if (!legal.length) return results;

            const maximizing = (this.player === COLORS.WHITE);

            for (const [from, to] of legal) {
                const savedFrom = this.board[from];
                const savedTo = this.board[to];
                this.board[to] = this.board[from];
                this.board[from] = 0;
                this.player = 1 - this.player;

                const evalScore = minimax(depth - 1, -Infinity, Infinity, !maximizing);

                this.player = 1 - this.player;
                this.board[from] = savedFrom;
                this.board[to] = savedTo;

                results.push({ move: [from, to], score: evalScore });
            }
            return results;
        };

        let humanMoveIndex = 0;
        for (let i = 0; i < allMoves.length; i += 2) { // human is always White, moves at even indices
            const humanMove = allMoves[i];

            // Reconstruct position before this human move
            this.init();
            this.player = COLORS.WHITE;
            this.moves = [];
            for (let j = 0; j < i; ++j) {
                this.move(allMoves[j][0], allMoves[j][1]);
            }

            // Evaluate all moves from this position
            const evals = evaluateAllMoves(analysisDepth);
            if (!evals.length) continue;

            const whiteToMove = (this.player === COLORS.WHITE);
            let bestScore = evals[0].score;
            for (const e of evals) {
                if (whiteToMove) {
                    if (e.score > bestScore) bestScore = e.score;
                } else {
                    if (e.score < bestScore) bestScore = e.score;
                }
            }

            // Determine set G of top moves within 50 centipawns
            const topMoves = [];
            for (const e of evals) {
                if (whiteToMove) {
                    if (e.score >= bestScore - 50) topMoves.push(e.move);
                } else {
                    if (e.score <= bestScore + 50) topMoves.push(e.move);
                }
            }

            // Is human move in G?
            let inTop = false;
            for (const m of topMoves) {
                if (m[0] === humanMove[0] && m[1] === humanMove[1]) {
                    inTop = true;
                    break;
                }
            }

            let probability = 0.0;
            if (inTop && topMoves.length > 0) {
                probability = 100.0 / topMoves.length;
            }

            let quality = '';
            if (probability >= 70.0) quality = 'Excellent';
            else if (probability >= 50.0) quality = 'Good';
            else if (probability >= 30.0) quality = 'Okay';
            else if (probability > 0.0)  quality = 'Weak';
            else                         quality = 'Blunder';

            humanMoveIndex += 1;
            const moveStr = algebraic(humanMove[0]) + algebraic(humanMove[1]);

            rows.push({
                index: humanMoveIndex,
                move: moveStr,
                score: bestScore,
                probability: probability.toFixed(1),
                quality
            });
        }

        // Restore state and rendering
        this.board = savedBoard.slice();
        this.player = savedPlayer;
        this.moves = savedMoves.slice();
        this.render = savedRender;
        this.render();

        if (!rows.length) {
            analysisDiv.innerHTML = 'No analysis available.';
            return;
        }

        let html = '<table style="width:100%; font-size:12px; border-collapse:collapse;">';
        html += '<tr><th style="text-align:left; padding:2px;">#</th>' +
                '<th style="text-align:left; padding:2px;">Move</th>' +
                '<th style="text-align:left; padding:2px;">Score</th>' +
                '<th style="text-align:left; padding:2px;">Prob(%)</th>' +
                '<th style="text-align:left; padding:2px;">Quality</th></tr>';
        for (const r of rows) {
            html += `<tr>
                <td style="padding:2px;">${r.index}</td>
                <td style="padding:2px;">${r.move}</td>
                <td style="padding:2px;">${r.score}</td>
                <td style="padding:2px;">${r.probability}</td>
                <td style="padding:2px;">${r.quality}</td>
            </tr>`;
        }
        html += '</table>';
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
document.getElementById('undoBtn').onclick = () => {
    if (game.moves.length >= 2) {
        game.moves.pop();
        game.moves.pop();
        game.init();
        for (let m of game.moves) game.move(m[0], m[1]);
        game.render();
    }
};
