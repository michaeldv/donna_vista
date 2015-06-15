// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Parses Donna chess format string for one side. Besides [K]ing, [Q]ueen, [R]ook,
// [B]ishop, and k[N]ight the following pseudo pieces could be specified:
//
// [M]ove:      specifies the right to move along with the optional move number.
//              For example, "M42" for Black means the Black is making 42nd move.
//              Default value is "M1" for White.
//
// [C]astle:    specifies castle right squares. For example, "Cg1" and "Cc8" encode
//              allowed kingside castle for White, and queenside castle for Black.
//              By default all castles are allowed, i.e. defult value is "Cc1,Cg1"
//              for White and "Cc8,Cg8" for Black. The actual castle rights are
//              checked during position setup to make sure they do not violate
//              chess rules. If castle rights are specified incorrectly they get
//              quietly ignored.
//
// [E]npassant: specifies en-passant square if any. For example, "Ed3" marks D3
//              square as en-passant. Default value is no en-passant.
//
//------------------------------------------------------------------------------
Position *setup_side(Position *self, char *side, uint8 color) {
    char *buffer = (char *)calloc(strlen(side) + 1, 1); // strdup() the string so we could tokenize it.
    strcat(buffer, side);

    char *move = strtok(buffer, ",");
    while(move) {
        if (move[0] == 'M') { // TODO: parse move number.
            self->color = color;
        } else {
            int sq, size = strlen(move);
            assert(size == 2 || size == 3); // Don't panic.

            if (size == 2) { // Pawn, ex. "e4".
                sq = square(move[1] - '1', move[0] - 'a');
            } else { // Piece, ex. "Qd1".
                sq = square(move[2] - '1', move[1] - 'a');
            }

            switch(move[0]) {
            case 'K':
                self->pieces[sq] = king(color);
                break;
            case 'Q':
                self->pieces[sq] = queen(color);
                break;
            case 'R':
                self->pieces[sq] = rook(color);
                break;
            case 'B':
                self->pieces[sq] = bishop(color);
                break;
            case 'N':
                self->pieces[sq] = knight(color);
                break;
            case 'E':
                self->enpassant = (uint8)sq;
                break;
            case 'C':
                if (sq == C1 + (int)color || sq == C8 + (int)color) {
                    self->castles |= castleQueenside[color];
                } else if (sq == G1 + (int)color || sq == G8 + (int)color) {
                    self->castles |= castleKingside[color];
                }
                break;
            default:
                // When everything else fails, read the instructions.
                self->pieces[sq] = pawn(color);
            }
        }
        move = strtok(NULL, ","); // <-- Get next move token.
    }
    free(buffer);

    return self;
}

// Computes initial values of position's polyglot id and pawn id. When making a
// move these values get updated incrementally.
//------------------------------------------------------------------------------
void polyglot(Position *self) {
    Bitmask board = self->board;

    while(any(board)) {
        int square = pop(&board);
        Piece piece = self->pieces[square];
        uint64 random = poly(piece, square);
        self->id ^= random;
        if is_pawn(piece) {
            self->pawnId ^= random;
        }
    }

    self->id ^= hashCastle[self->castles];
    if (self->enpassant != 0) {
        self->id ^= hashEnpassant[self->enpassant & 7]; // self->enpassant column.
    }
    if (self->color == White) {
        self->id ^= polyglotRandomWhite;
    }
}

// Computes positional valuation score based on PST. When making a move the
// valuation tally gets updated incrementally.
//------------------------------------------------------------------------------
void valuation(Position *self) {
    self->tally = (Score){};
    Bitmask board = self->board;

    while(any(board)) {
        int square = pop(&board);
        Piece piece = self->pieces[square];
        self->tally = add(self->tally, pst[piece][square]);
    }
}

// Returns true if material balance is insufficient to win the game.
//------------------------------------------------------------------------------
bool insufficient(Position *self) {
    return (materialBase[self->balance].flags & materialDraw) != 0;
}


//------------------------------------------------------------------------------
Position *new_position(char *white, char *black) {
    tree[node] = (Position){};
    Position *self = &tree[node];

    setup_side(self, white, White);
    setup_side(self, black, Black);

    self->castles = castleKingside[White] | castleQueenside[White] | castleKingside[Black] | castleQueenside[Black];
    if (self->pieces[E1] != King || self->pieces[H1] != Rook) {
        self->castles &= ~castleKingside[White];
    }
    if (self->pieces[E1] != King || self->pieces[A1] != Rook) {
        self->castles &= ~castleQueenside[White];
    }
    if (self->pieces[E8] != BlackKing || self->pieces[H8] != BlackRook) {
        self->castles &= ~castleKingside[Black];
    }
    if (self->pieces[E8] != BlackKing || self->pieces[A8] != BlackRook) {
        self->castles &= ~castleQueenside[Black];
    }

    for (int sq = A1; sq < len(self->pieces); sq++) {
        Piece piece = self->pieces[sq];
        if (piece != 0) {
            self->outposts[piece] |= bit[sq];
            self->outposts[color(piece)] |= bit[sq];
            if (is_king(piece)) {
                self->king[color(piece)] = (uint8)sq;
            }
            self->balance += materialBalance[piece];
        }
    }

    self->reversible = true;
    self->board = self->outposts[White] | self->outposts[Black];

    polyglot(self);  // Calculate self->id and self->pawnId.
    valuation(self); // Calculate self->tally.

    return &tree[node];
}

// Decodes FEN string and creates new position.
//------------------------------------------------------------------------------
Position *new_position_from_fen(char *fen) {
    // printf("FEN [%s]\n", fen);
    tree[node] = (Position){};
    Position *self = &tree[node];

    char *buffer = (char *)calloc(strlen(fen) + 1, 1); // strdup() the string so we could tokenize it.
    strcat(buffer, fen);

    // Expected matches of interest are as follows:
    // [0] - Pieces (entire board).
    // [1] - Color of side to move.
    // [2] - Castle rights.
    // [3] - En-passant square.
    // [4] - Number of half-moves.
    // [5] - Number of full moves.
    char *matches[6] = {};

    char *token = strtok(buffer, " ");
    for (int i = 0; i < len(matches) && token; i++) {
        matches[i] = token;
        token = strtok(NULL, " ");
    }

    // First four parts are required.
    if (!matches[4]) {
        return NULL;
    }

    // [0] - Pieces (entire board).
    int sq = A8;
    for (char *ch = matches[0]; *ch; ch++) {
        Piece piece = (Piece)0;
        switch(*ch) {
        case 'P':
            piece = Pawn;
            break;
        case 'p':
            piece = BlackPawn;
            break;
        case 'N':
            piece = Knight;
            break;
        case 'n':
            piece = BlackKnight;
            break;
        case 'B':
            piece = Bishop;
            break;
        case 'b':
            piece = BlackBishop;
            break;
        case 'R':
            piece = Rook;
            break;
        case 'r':
            piece = BlackRook;
            break;
        case 'Q':
            piece = Queen;
            break;
        case 'q':
            piece = BlackQueen;
            break;
        case 'K':
            piece = King;
            self->king[White] = (uint8)sq;
            break;
        case 'k':
            piece = BlackKing;
            self->king[Black] = (uint8)sq;
            break;
        case '/':
            sq -= 16;
            break;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8':
            sq += *ch - '0';
        }
        if (piece) {
            self->pieces[sq] = piece;
            self->outposts[piece] |= bit[sq];
            self->outposts[color(piece)] |= bit[sq];
            self->balance += materialBalance[piece];
            sq++;
        }
    }

    // [1] - Color of side to move.
    if (matches[1][0] == 'w') {
        self->color = White;
    } else {
        self->color = Black;
    }

    // [2] - Castle rights.
    for (char *ch = matches[2]; *ch; ch++) {
        switch(*ch) {
        case 'K':
            self->castles |= castleKingside[White];
            break;
        case 'Q':
            self->castles |= castleQueenside[White];
            break;
        case 'k':
            self->castles |= castleKingside[Black];
            break;
        case 'q':
            self->castles |= castleQueenside[Black];
            break;
        case '-':
            ; // No castling rights.
        }
    }

    // [3] - En-passant square.
    if (matches[3][0] != '-') {
        self->enpassant = (uint8)(square(matches[3][1] - '1', matches[3][0] - 'a'));
    }

    self->reversible = true;
    self->board = self->outposts[White] | self->outposts[Black];

    polyglot(self);
    valuation(self);

    free(buffer);
    return self;
}

// Sets up initial chess position.
//------------------------------------------------------------------------------
Position *new_initial_position() {
    return new_position_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

// Reports game status for current position or after the given move. The status
// helps to determine whether to continue with search or if the game is over.
//------------------------------------------------------------------------------
Status standing(Position *self, Move move, int score) {
    Status status = InProgress;

    if (move) {
        self = make_move(self, move);
    }

    int ply = PLY(); // After the ^^^ move is made.

    if (score == 0) {
        if (ply == 1) {
            if (insufficient(self)) {
                status = Insufficient;
            } else if (third_repetition(self)) {
                status = Repetition;
            } else if (fifty(self)) {
                status = FiftyMoves;
            }
        }
        if (status == InProgress) {
            MoveGen *gen = new_gen(self, MaxPly);
            if (!any_valid(generate_moves(gen))) {
                status = Stalemate;
            }
        }
    } else if (score == Checkmate - ply) {
        if (is_in_check(self, self->color)) {
            status = (self->color == White ? BlackWon : WhiteWon);
        } else {
            status = Stalemate;
        }
    } else if (score > Checkmate - MaxDepth && (score + ply) / 2 > 0) {
        status = (self->color == White ? BlackWinning : WhiteWinning);
    }

    if (move) {
        self = undo_last_move(self);
    }

    return status;
}

// // Encodes position as FEN string.
// func (p *Position) fen() (fen string) {
//     fancy = engine.fancy
//     engine.fancy = false; defer func() { engine.fancy = fancy }()
//
//     // Board: start from A8->H8 going down to A1->H1.
//     empty = 0
//     for row = A8H8; row >= A1H1; row-- {
//         for col = A1A8; col <= H1H8; col++ {
//             square = square(row, col)
//             piece = p.pieces[square]
//
//             if piece != 0 {
//                 if empty != 0 {
//                     fen += fmt.Sprintf("%d", empty)
//                     empty = 0
//                 }
//                 fen += piece.String()
//             } else {
//                 empty++
//             }
//
//             if col == 7 {
//                 if empty != 0 {
//                     fen += fmt.Sprintf("%d", empty)
//                     empty = 0
//                 }
//                 if row != 0 {
//                     fen += "/"
//                 }
//             }
//         }
//     }
//
//     // Side to move.
//     if p.color == White {
//         fen += " w"
//     } else {
//         fen += " b"
//     }
//
//     // Castle rights for both sides, if any.
//     if p.castles & 0x0F != 0 {
//         fen += " "
//         if p.castles & castleKingside[White] != 0 {
//             fen += "K"
//         }
//         if p.castles & castleQueenside[White] != 0 {
//             fen += "Q"
//         }
//         if p.castles & castleKingside[Black] != 0 {
//             fen += "k"
//         }
//         if p.castles & castleQueenside[Black] != 0 {
//             fen += "q"
//         }
//     } else {
//         fen += " -"
//     }
//
//     // En-passant square, if any.
//     if p.enpassant != 0 {
//         row, col = coordinate(int(p.enpassant))
//         fen += fmt.Sprintf(" %c%d", col + 'a', row + 1)
//     } else {
//         fen += " -"
//     }
//
//     // TODO: Number of half-moves and number of full moves.
//     fen += " 0 1"
//
//     return
// }
//
// // Encodes position as DCF string (Donna Chess Format).
// func (p *Position) dcf() string {
//     fancy = engine.fancy
//     engine.fancy = false; defer func() { engine.fancy = fancy }()
//
//     encode = func (square int) string {
//         var buffer bytes.Buffer
//
//         buffer.WriteByte(byte(col(square)) + 'a')
//         buffer.WriteByte(byte(row(square)) + '1')
//
//         return buffer.String()
//     }
//
//     var pieces [2][]string
//
//     for color = uint8(White); color <= uint8(Black); color++ {
//         // Right to move and (TODO) move number.
//         if color == p.color && color == Black {
//             pieces[color] = append(pieces[color], "M")
//         }
//
//         // King.
//         pieces[color] = append(pieces[color], "K" + encode(int(p.king[color])))
//
//         // Queens, Rooks, Bishops, and Knights.
//         outposts = p.outposts[queen(color)]
//         for outposts != 0 {
//             pieces[color] = append(pieces[color], "Q" + encode(outposts.pop()))
//         }
//         outposts = p.outposts[rook(color)]
//         for outposts != 0 {
//             pieces[color] = append(pieces[color], "R" + encode(outposts.pop()))
//         }
//         outposts = p.outposts[bishop(color)]
//         for outposts != 0 {
//             pieces[color] = append(pieces[color], "B" + encode(outposts.pop()))
//         }
//         outposts = p.outposts[knight(color)]
//         for outposts != 0 {
//             pieces[color] = append(pieces[color], "N" + encode(outposts.pop()))
//         }
//
//         // Castle rights.
//         if p.castles & castleQueenside[color] == 0 || p.castles & castleKingside[color] == 0 {
//             if p.castles & castleQueenside[color] != 0 {
//                 pieces[color] = append(pieces[color], "C" + encode(C1 + 56 * int(color)))
//             }
//             if p.castles & castleKingside[color] != 0 {
//                 pieces[color] = append(pieces[color], "C" + encode(G1 + 56 * int(color)))
//             }
//         }
//
//         // En-passant square if any. Note that this gets assigned to the
//         // current side to move.
//         if p.enpassant != 0 && color == p.color {
//             pieces[color] = append(pieces[color], "E" + encode(int(p.enpassant)))
//         }
//
//         // Pawns.
//         outposts = p.outposts[pawn(color)]
//         for outposts != 0 {
//             pieces[color] = append(pieces[color], encode(outposts.pop()))
//         }
//     }
//
//     return strings.Join(pieces[White], ",") + " : " + strings.Join(pieces[Black], ",")
// }
//
char *position_string(Position *p, char *buffer) {
    strcpy(buffer, "  a b c d e f g h");
    if (!is_in_check(p, p->color)) {
        strcat(buffer, "\n");
    } else {
        sprintf(buffer + strlen(buffer), "  Check to %s\n", p->color ? "black" : "white");
    }
    for (int row = 7; row >= 0; row--) {
        int last = strlen(buffer);
        buffer[last] = '1' + row;
        buffer[last + 1] = '\0';
        for (int col = 0; col <= 7; col++) {
            strcat(buffer, " ");
            Piece piece = p->pieces[square(row, col)];
            if (piece) {
                strcat(buffer, glyph(piece)); // Set
            } else {
                strcat(buffer, "\xE2\x8B\x85"); // Clear
            }
        }
        strcat(buffer, "\n");
    }
    return buffer;
}
