#ifndef REVERSI_H
#define REVERSI_H

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    //BOARD FINALS HELPS TO CHANGE BOARD SIZE AND DATATYPES
    #define ROWS            8
    #define COLUMNS         ROWS
    #define SLOT_DT         u_int8_t
    #define SLOT_SIZE       sizeof(SLOT_DT)
    typedef SLOT_DT         BOARD[ROWS][COLUMNS];

    //Interior Cross Sections
    #define INTERSECTION    "\u253C"

    //Edges
    #define TOP             "\u2501"
    #define LEFT            "\u2503"
    #define BOTTOM          TOP
    #define RIGHT           LEFT

    //Edge Intersections
    #define TOP_INTER       "\u252C"
    #define LEFT_INTER      "\u251C"
    #define BOTTOM_INTER    "\u2534"
    #define RIGHT_INTER     "\u2524"

    //Corners
    #define TOP_LEFT        "\u250C"
    #define TOP_RIGHT       "\u2510"
    #define BOTTOM_LEFT     "\u2514"
    #define BOTTOM_RIGHT    "\u2518"

    //Player pieces
    #define WHITE_CIRCLE    "\u25CF"
    #define BLACK_CIRCLE    "\u25CB"

    //Board coloring
    #define TEXT_FONT       "\033[38;5;0;48;5;250m"
    #define CLEAR_FONT      "\033[0m"

    typedef struct Line{
        SLOT_DT* piece;
        struct Line* next;
    }Line;

    void init();
    int make_move(int, int);
    int is_valid_move(int, int, Line***);
    int in_bounds(int, int);
    int check_neighbors(int, int, SLOT_DT, Line***);
    Line* is_sandwich(int, int, int, int);
    void flip_pieces(Line**, int num_lines);
    void print_board();
    
#endif