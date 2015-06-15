// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

// Returns row number in 0..7 range for the given square.
#define row(square) ((square) >> 3)
#define ROW row

// Returns column number in 0..7 range for the given square.
#define col(square) ((square) & 7)
#define COL col

// Returns relative rank for the square in 0..7 range. For example E2 is rank 1
// for white and rank 6 for black.
#define rank(color, square) (row(square) ^ ((color) * 7))
#define RANK rank

// Returns 0..63 square number for the given row/column coordinate.
#define square(row, col) (((row) << 3) + (col))
#define SQUARE square

// Returns distance between current and root node.
#define PLY() (node - rootNode)

// Checks whether the move is among good moves captured so far and returns its history value.
#define good(move) game.history[piece(move)][to(move)]

#define fixed_depth() (engine.options.max_depth > 0)
#define fixed_time() (engine.options.move_time > 0)
#define varying_time() (engine.options.move_time == 0)

// Flips the square verically for white (ex. E2 becomes E7).
//------------------------------------------------------------------------------
int flip(uint8 color, int square) {
    return color == White ? square ^ 56 : square;
}

// Returns a bitmask with light or dark squares set matching the color of the square.
//------------------------------------------------------------------------------
Bitmask same(int square)  {
    return any(bit[square] & maskDark) ? maskDark : ~maskDark;
}

//------------------------------------------------------------------------------
int uncache(int score, int ply) {
    if (score > Checkmate - MaxPly && score <= Checkmate) {
        return score - ply;
    } else if (score >= -Checkmate && score < -Checkmate + MaxPly) {
        return score + ply;
    }

    return score;
}

Timestamp now() {
    Timestamp timestamp;

    gettimeofday(&timestamp, NULL);

    return timestamp;
}

// Returns time since start in microseconds (one millionth of a second).
//------------------------------------------------------------------------------
uint64 since(Timestamp checkpoint) {
    Timestamp elapsed, timestamp = now();

    timersub(&timestamp, &checkpoint, &elapsed);

    return elapsed.tv_sec * 1000000 + elapsed.tv_usec;
}


// Returns nodes per second search speed for the given time duration.
//------------------------------------------------------------------------------
uint64 nps(uint64 duration) {
    uint64 nodes = (uint64)(game.nodes + game.qnodes) * 1000000;

    if (duration) {
        return nodes / duration;
    }

    return nodes;
}

// Formats time duration in milliseconds in human readable form (MM:SS.XXX).
//------------------------------------------------------------------------------
char *ms(uint64 duration, char *buffer) {
    uint64 mm = duration / 1000000 / 60;
    uint64 ss = duration / 1000000 % 60;
    uint64 xx = duration - mm * 100000 * 60 - ss * 1000000;
    sprintf(buffer, "%02llu:%02llu.%03llu", mm, ss, xx / 1000);

    return buffer;
}

// // The generation of random numbers is too important to be left to chance.
// // Returns pseudo-random integer in [0, limit] range. It panics if limit <= 0.
// func Random(limit int) int {
//     rand.Seed(time.Now().Unix())
//     return rand.Intn(limit)
// }
//
// func C(color uint8) string {
//     return [2]string{"white", "black"}[color]
// }
//
// func Summary(metrics map[string]interface{}) {
//     phase = metrics["Phase"].(int)
//     tally = metrics["PST"].(Score)
//     material = metrics["Imbalance"].(Score)
//     final = metrics["Final"].(Score)
//     units = float32(onePawn)
//
//     fmt.Println()
//     fmt.Printf("Metric              MidGame        |        EndGame        | Blended\n")
//     fmt.Printf("                W      B     W-B   |    W      B     W-B   |  (%d)  \n", phase)
//     fmt.Printf("-----------------------------------+-----------------------+--------\n")
//     fmt.Printf("%-12s    -      -    %5.2f  |    -      -    %5.2f  >  %5.2f\n", "PST",
//         float32(tally.midgame)/units, float32(tally.endgame)/units, float32(tally.blended(phase))/units)
//     fmt.Printf("%-12s    -      -    %5.2f  |    -      -    %5.2f  >  %5.2f\n", "Imbalance",
//         float32(material.midgame)/units, float32(material.endgame)/units, float32(material.blended(phase))/units)
//
//     for _, tag = range([]string{"Tempo", "Center", "Threats", "Pawns", "Passers", "Mobility", "+Pieces", "-Knights", "-Bishops", "-Rooks", "-Queens", "+King", "-Cover", "-Safety"}) {
//         white = metrics[tag].(Total).white
//         black = metrics[tag].(Total).black
//
//         var score Score
//         score.add(white).subblack)
//
//         if tag[0:1] == "+" {
//             tag = tag[1:]
//         } else if tag[0:1] == "-" {
//             tag = "  " + tag[1:]
//         }
//
//         fmt.Printf("%-12s  %5.2f  %5.2f  %5.2f  |  %5.2f  %5.2f  %5.2f  >  %5.2f\n", tag,
//             float32(white.midgame)/units, float32(black.midgame)/units, float32(score.midgame)/units,
//             float32(white.endgame)/units, float32(black.endgame)/units, float32(score.endgame)/units,
//             float32(score.blended(phase))/units)
//     }
//     fmt.Printf("%-12s    -      -    %5.2f  |    -      -    %5.2f  >  %5.2f\n\n", "Final Score",
//         float32(final.midgame)/units, float32(final.endgame)/units, float32(final.blended(phase))/units)
// }
//
// // Logging wrapper around fmt.Printf() that could be turned on as needed. Typical
// // usage is Log(); defer Log() in tests.
// func Log(args ...interface{}) {
//     switch len(args) {
//     case 0:
//         // Calling Log() with no arguments flips the logging setting.
//         engine.log = !engine.log
//         engine.fancy = !engine.fancy
//     case 1:
//         switch args[0].(type) {
//         case bool:
//             engine.log = args[0].(bool)
//             engine.fancy = args[0].(bool)
//         default:
//             if engine.log {
//                 fmt.Println(args...)
//             }
//         }
//     default:
//         if engine.log {
//             fmt.Printf(args[0].(string), args[1:]...)
//         }
//     }
// }
