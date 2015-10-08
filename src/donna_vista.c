// Copyright (c) 2014-2015 by Michael Dvorkin. All Rights Reserved.
// Use of this source code is governed by a MIT-style license that can
// be found in the LICENSE file.

#include "donna_vista.h"

// Visual Studio Express
// https://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx

int main(int argc, char *argv[]) {
    init();
    new_engine(
        "log", false, "fancy", true, "cache", 256.0, "depth", 12, "bookfile", "donna.bin",
        NULL // <-- va_args() were designed at the time when everything had to be null terminated.
    );
    repl();

    // char buffer[512];
    // Position *p;
    // new_game();
    // p = start();
    // printf("%s\n", position_string(p, buffer));

    // setup("Ke1,Qd2,e2,e3", "Ke7,Qd8,Be8,Ne5,h5");
    // p = start();
    // printf("%s\n", position_string(p, buffer));

    // setup("Ke1,Qa3,Rd1,Rh1,Bc4,Bc5,Nf3,a2,d4,f2,g2,h2", "Kg8,Qb6,Ra8,Rf8,Bg7,Bg4,Nc3,a7,b7,c6,f7,g5,h7");
    // p = start();
    // printf("%s\n=> %d\n", position_string(p, buffer), evaluate(p));
    // exit(1);

    // setup("Kg1,Rb7,Na4,a2,b2,f4,g2,g3", "M30,Kh7,Rd2,Bb6,a7,c5,c4,g7,h6");
    // p = start();
    // printf("%s\n", position_string(p, buffer));
    // think();

    // setup("Kh1,Qe2,Ra1,Rf1,Bb2,Be4,Nc3,a3,b4,e5,f4,g2,h2", "M21,Kg8,Qe7,Rc8,Rd8,Bb6,Bb7,Ng4,a6,b5,e6,f7,g7,h7");
    // p = start();
    // printf("%s\n", position_string(p, buffer));
    // think();

    // setup("Ke1,Qa3,Rd1,Rh1,Bc4,Bc5,Nf3,a2,d4,f2,g2,h2", "M16,Kg8,Qb6,Ra8,Rf8,Bg4,Bg7,Nc3,a7,b7,c6,f7,g6,h7");
    // p = start();
    // printf("%s\n", position_string(p, buffer));
    // think();

    // setup("M17,Kg1,Qc2,Ra1,Re1,Bc1,Bg2,Ng5,a2,b2,c3,d4,f2,g3,h2", "Kg8,Qd6,Ra8,Rf8,Bc8,Nd5,Ng6,a7,b6,c4,e6,f7,g7,h7");
    // p = start();
    // printf("%s\n", position_string(p, buffer));
    // think();

    // setup("M30,Kg1,Qe5,Bb2,Ng3,c3,d4,e6,g2,h2", "Kg7,Qe7,Nb3,Nf6,a7,b6,c4,d5,g6,h7");
    // p = start();
    // printf("%s\n", position_string(p, buffer));
    // think();

    // Depth: 5 => 0.262194s
    // Nodes: 4865609
    //
    // Depth: 6 => 6.054049s
    // Nodes: 119060324

    // // Windows: For interval taking, use GetTickCount(). It returns milliseconds since startup.
    // Timestamp checkpoint = now();
    //
    // // Some code you want to time, for example:
    // int depth = 6;
    // new_game();
    // p = start();
    // int64 nodes = perft(p, depth);
    //
    // uint64 elapsed = since(checkpoint);
    // uint64 nps = nodes * 1000000 / elapsed; // Elapsed is in microseconds.
    //
    // printf("Time elapsed: %llu.%06llu\n", elapsed / 1000000, elapsed % 1000000);
    // printf("depth %d -> %lld nodes, nps %lldK\n", depth, nodes, nps / 1000);
}