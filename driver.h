#ifndef REVERSI_H
#define REVERSI_H

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <stdint.h>

    //BOARD FINALS HELPS TO CHANGE BOARD SIZE AND DATATYPES
    #define ROWS            8
    #define COLUMNS         ROWS
    #define piece_t         uint8_t
    #define SLOT_SIZE       sizeof(piece_t)
    typedef piece_t         BOARD[ROWS][COLUMNS];

    //MAX NUMBER OF TURNS
    #define MAX_TURNS       60
    //MAX NUMBER OF TURNS FOR TEST FILES
    #define MAX_TURNS_TEST1 7
    #define MAX_TURNS_TEST3 41
    #define MEM_TEST        1

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
        piece_t* piece;
        struct Line* next;
    }Line;

    typedef union Location
    {
        uint16_t location;
        struct{
            uint8_t column;
            uint8_t row;
        };
    }Location;
    

    void init();
    void get_input(int*, int*);
    int make_move(int, int);
    int is_valid_move(int, int, Line***, piece_t, piece_t);
    int in_bounds(int, int);
    int check_neighbors(int, int, piece_t, piece_t, Line***);
    Line* is_sandwich(int, int, int, int, piece_t);
    void flip_pieces(Line**, int num_lines);
    int has_valid_move();
    void update_perimeter(int, int);
    void dealloc_lines(Line**, int);
    void print_board();
    
#endif