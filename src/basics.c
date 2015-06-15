// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

typedef enum { false = 0, true  = 1 } bool;
//typedef unsigned char bool;
typedef unsigned char *string;
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int int64;
typedef unsigned long long int uint64;

typedef uint8 Piece;
typedef uint32 Move;
typedef uint64 Bitmask;
typedef struct timeval Timestamp;

#if !(defined_WIN32 || _WIN64)
#define ANSI_RED   "\033[0;31m"
#define ANSI_GREEN "\033[0;32m"
#define ANSI_TEAL  "\033[0;36m"
#define ANSI_NONE  "\033[0m"
#else
#define ANSI_RED   ""
#define ANSI_GREEN ""
#define ANSI_TEAL  ""
#define ANSI_NONE  ""
#endif

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define len(x) (sizeof(x) / sizeof((x)[0]))

// The primary purpose of the DATA statement is to give names to constants;
// instead of referring to pi as 3.141592653589793 at every appearance, the
// variable PI can be given that value with a DATA statement and used instead
// of the longer form of the constant.
//
// This also simplifies modifying the program, should the value of pi change.
//
//                                      -― FORTRAN manual for Xerox Computers

// Your chess engine hates you when you are working on a new version.
#define VERSION "1.0" // May 11, 2015

// Limits and conventions.
#define White 0
#define Black 1
#define MaxPly 64
#define MaxDepth 32
#define Checkmate 0x7FFE      // = 32,766
#define UnknownScore 0x7FFF   // = math.MaxInt16 = 32,767

// Evaluation flags.
#define whiteKingSafety    0x01  // Should we worry about white king's safety?
#define blackKingSafety    0x02  // Ditto for the black king.
#define materialDraw       0x04  // King vs. King (with minor)
#define knownEndgame       0x08  // Where we calculate exact score.
#define lesserKnownEndgame 0x10  // Where we set score markdown value.
#define singleBishops      0x20  // Sides might have bishops on opposite color squares.

// Position cache flags.
#define cachedNone         0x00
#define cacheExact         0x01
#define cacheAlpha         0x02 // Upper bound.
#define cacheBeta          0x04 // Lower bound.

// Game pieces.
enum _Piece {
    Pawn        = 2,
    Knight      = 4,
    Bishop      = 6,
    Rook        = 8,
    Queen       = 10,
    King        = 12,
    BlackPawn   = Pawn | 1,
    BlackKnight = Knight | 1,
    BlackBishop = Bishop | 1,
    BlackRook   = Rook | 1,
    BlackQueen  = Queen | 1,
    BlackKing   = King | 1
};

// Game status.
typedef enum _Status {
    InProgress = 0,
    WhiteWon,        // White checkmated.
    BlackWon,        // Black checkmated.
    Stalemate,       // Draw by stalemate, forced or self-imposed.
    Insufficient,    // Draw by insufficient material.
    Repetition,      // Draw by repetition.
    FiftyMoves,      // Draw by 50 moves rule.
    WhiteWinning =  Checkmate / 10,    // Decisive advantage for White.
    BlackWinning = -Checkmate / 10 + 1 // Decisive advantage for Black.    
} Status;

// Square indices.
enum _Square {
    A1 = 0,
        B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8   
};

// Rank and file indices.
enum _Rows { A1H1 = 0, A2H2, A3H3, A4H4, A5H5, A6H6, A7H7, A8H8 };
enum _Cols { A1A8 = 0, B1B8, C1C8, D1D8, E1E8, F1F8, G1G8, H1H8 };

typedef struct _Score {
    int16       midgame;
    int16       endgame;
} Score, *PScore;

typedef struct _MoveInfo {
    int         from;
    int         to;
    Piece       piece;
    Piece       capture;
} MoveInfo, *PMoveInfo;

typedef struct _Promo {
    Move        q, r, b, n;
} Promo, *PPromo;

typedef struct _PawnEntry {
    uint64      id;                     // Pawn hash key.
    uint8       king[2];                // King square for both sides.
    Score       score;                  // Static score for the given pawn structure.
    Score       cover[2];               // King cover penalties for both sides.
    Bitmask     passers[2];             // Passed pawn bitmasks for both sides.
} PawnEntry, *PPawnEntry;

typedef PawnEntry PawnCache[8192 * 2];

typedef struct _CacheEntry {
    uint32      id;         // 4
    Move        move;       // 4 = 8
    int16       score;      // 2 = 10
    int16       depth;      // 2 = 12
    uint8       flags;      // 1 = 13
    uint8       token;      // 1 = 14
} CacheEntry, *PCacheEntry;

typedef CacheEntry Cache;

typedef struct _RootPv {
    int         size;
    Move        moves[MaxPly];
} RootPv, *PRootPv;

typedef struct _Game {
    int         nodes;                  // Number of regular nodes searched.
    int         qnodes;                 // Number of quiescence nodes searched.
    uint8       token;                  // Cache's expiration token.
    bool        deepening;              // True when searching first root move.
    bool        improving;              // True when root search score is not falling.
    float       volatility;             // Root search stability count.
    char        *fen;                   // Initial position (FEN).
    char        *dcf[2];                // Initial position (Donna chess format).
    int         history[14][64];        // Good moves history.
    Move        killers[MaxPly][2];     // Killer moves.
    RootPv      rootpv;                 // Principal variation for root moves.
    RootPv      pv[MaxPly];             // Principal variations for each ply.
    PawnCache   pawnCache;              // Cache of pawn structures.
    Cache       *cache;                 // Transposition table.
    uint32      csize;                  // Transposition table size in bytes.
} Game, *PGame;

typedef struct _Position {
    uint64      id;                     // Polyglot hash value for the position.
    uint64      pawnId;                 // Polyglot hash value for position's pawn structure.
    Bitmask     board;                  // Bitmask of all pieces on the board.
    uint8       king[2];                // King's square for both colors.
    Piece       pieces[64];             // Array of 64 squares with pieces on them.
    Bitmask     outposts[14];           // Bitmasks of each piece on the board; [0] all white, [1] all black.
    Score       tally;                  // Positional valuation score based on PST.
    int         balance;                // Material balance index.
    bool        reversible;             // Is this position reversible?
    uint8       color;                  // Side to make next move.
    uint8       enpassant;              // En-passant square caused by previous move.
    uint8       castles;                // Castle rights mask.
} Position, *PPosition;

// King safety information; used only in the middle game when there is enough
// material to worry about the king safety.
typedef struct _Safety {
    Bitmask     fort;                   // Squares around the king plus one extra row in front.
    int         threats;                // A sum of treats: each based on attacking piece type.
    int         attacks;                // Number of attacks on squares adjacent to the king.
    int         attackers;              // Number of pieces attacking king's fort.
} Safety, *PSafety;

// Helper structure used for evaluation tracking.
typedef struct _Total {
    Score       white;
    Score       black;
} Total, *PTotal;

// http://www.newty.de/fpt/fpt.html
typedef int(*Function)();
typedef void(*Ticker)();

typedef struct _MaterialEntry {
    Score       score;                  // Score adjustment for the given material.
    Function    endgame;                // Function to analyze an endgame position.
    int         phase;                  // Game phase based on available material.
    int         turf;                   // Home turf score for the game opening.
    uint8       flags;                  // Evaluation flags based on material balance.
} MaterialEntry, *PMaterialEntry;

typedef MaterialEntry Material;

typedef struct _Evaluation {
    Score       score;
    Safety      safety[2];
    Bitmask     attacks[14];
    PawnEntry   *pawns;
    Position    *position;
    Material    *material;
} Evaluation, *PEvaluation;

typedef struct _Magic {
    Bitmask     mask;
    Bitmask     magic;
} Magic, *PMagic;

typedef struct _ScoredMove {
    Move        move;
    int         score;
} ScoredMove, *PScoredMove;

typedef struct _MoveGen {
    Position    *p;
    ScoredMove  list[128];
    int         ply;
    int         head;
    int         tail;
    Bitmask     pins;
} MoveGen, *PMoveGen;

typedef struct _Clock {
    bool        halt;                   // Stop search immediately when set to true.
    int64       soft_stop;              // Target soft time limit to make a move.
    int64       hard_stop;              // Immediate stop time limit.
    float       extra;                  // Extra time factor based on search volatility.
    Timestamp   start;
    Ticker      ticker;
} Clock, *PClock;

typedef struct _Options {
    bool        ponder;                 // (-) Pondering mode.
    bool        infinite;               // (-) Search until the "stop" command.
    int         max_depth;              // Search X plies only.
    int         max_nodes;              // (-) Search X nodes only.
    int64       move_time;              // Search exactly X milliseconds per move.
    int64       moves_to_go;            // Number of moves to make till time control.
    int64       time_left;              // Time left for all remaining moves.
    int64       time_inc;               // Time increment after the move is made.
} Options, *POptions;

typedef struct _Engine {
    bool        log;                    // Enable logging.
    bool        fancy;                  // Represent pieces as UTF-8 characters.
    bool        uci;                    // Use UCI protocol.
    uint8       status;                 // Engine status.
    char       *log_file;               // Log file name.
    char       *book_file;              // Polyglot opening book file name.
    float       cache_size;             // Default cache size.
    Clock       clock;
    Options     options;
} Engine, *PEngine;

// Essential globals.
int node = 0;
int rootNode = 0;
Game game = {};
Position tree[1024] = {};
Evaluation eval = {};
Engine engine = {};

// Last entry of move generator array serves for utility move generation,
// ex. when converting string notations or determining a stalemate.
MoveGen moveList[MaxPly + 1] = {};
