#include "driver.h"

BOARD board;
int turn = 0;

int main(int argc, char* argv[]){
    init();
    print_board();
    make_move(3,0);
    print_board();
    turn++;
    make_move(0,0);
    print_board();
    return 0;
}

void init(){
    //set board to all values
    for(int i = 0; i < ROWS; i++){
        memset(board[i], i%3, SLOT_SIZE*COLUMNS);
    }
}

int make_move(int row, int column){
    Line** lines = (Line**) malloc(sizeof(Line*));
    u_int8_t num_lines = 0;
    num_lines = is_valid_move(row, column, &lines);
    
    //if invalid move
    if(num_lines == 0){
        free(lines);
        return 0;
    }

    //if valid move
    flip_pieces(lines, num_lines);
    board[row][column] = turn%2+1;
    for(int line = 0; line < num_lines; line++){
        Line* last = NULL;
        while(lines[line] != NULL){
            last = lines[line];
            free(last);
            lines[line] = lines[line]->next;
        }
        free(lines[line]);
    }
    free(lines);
    return 1;
}

int is_valid_move(int row, int column, Line*** linesp){
    //check if in bounds
    if( !in_bounds(row, column) )
        return 1;

    //check what piece the position is
    SLOT_DT start = board[row][column];
    SLOT_DT player = turn % 2 + 1;
    SLOT_DT opponenet = player ^ 3;

    //if spot is occupied not valid move
    if (start != 0)
        return 0;
    
    Line** lines = *linesp;
    //if no opponents arround
    return check_neighbors(row, column, opponenet, &lines);
}

int in_bounds(int row, int column){
     if( row < 0 || row >= ROWS || column < 0 || column >= COLUMNS) 
        return 0;
    return 1;
}

int check_neighbors(int row, int column, SLOT_DT opponent, Line*** lines_p){
    Line** lines = *lines_p;
    u_int8_t sum = 0;
    int dir[] = {-1,0,1};
    for(int dir1 = 0; dir1 < 3; dir1 ++){
        for(int dir2 = 0; dir2 < 3; dir2++){
            int nrow = row + dir[dir1];
            int ncol = column + dir[dir2];
            //check if slot is in bounds
            if (! in_bounds(nrow, ncol)) 
                continue;
            //if not an opponent, dont look for sandwhich
            if(board[nrow][ncol] != opponent)
                continue;
            //check for sandwhich
            Line* line = is_sandwich(row, column, dir[dir1], dir[dir2]);
            //if no sandwhich found, look other directions
            if (line == NULL)
                continue;
            //if sandwhich add to list
            sum ++;
            lines = realloc(lines, sizeof(Line*) * sum);
            lines[sum-1] = line;
        }
    }
    return sum;
}

Line* is_sandwich(int row, int column, int dir1, int dir2){
    SLOT_DT player = turn % 2 + 1;
    Line* head = NULL;
    Line* tail = NULL;

    while (1){
        row += dir1;
        column += dir2;
        //if out of bounds, free list
        if(!in_bounds(row,column)) 
            break;
        //if free space hit, clear board
        if (board[row][column] == 0)
            break;
        //if own teamate is hit, return whatever sandwhich occured (may be NULL)
        if (board[row][column] == player)
            return head;
        //allocate & expand linked list if opponent is spotted
        Line* line = (Line*) malloc(sizeof(Line));
        line->next = NULL;
        line->piece = &(board[row][column]);
        if (head == NULL){
            head = line;
            tail = line;
        }
        else{
            tail->next = line;
            tail = line;
        }
    }
    //free list if not being used
    Line* last = NULL;
    while(head != NULL){
        last = head;
        head = head->next;
        free(last);
    }
    return NULL;
}

void flip_pieces(Line** lines, int num_lines){
    //iterate through 
    for(int line = 0; line < num_lines; line++){
        //iterate through linked list and flip pieces
        while (lines[line] != NULL){
            *lines[line]->piece ^= 3;
            lines[line] = lines[line]->next;
        }
    }
}

void print_board(){
    for(int row = 0; row < ROWS*2+1; row++){
        if(row%2 == 1)
            printf("%d ", row/2+1);
        else 
            printf("  ");
        for(int column = 0; column < COLUMNS*2+1; column++){
            //print the top
            if(row == 0){
                if(column == 0)
                    printf(TOP_LEFT);
                else if(column == COLUMNS*2)
                    printf(TOP_RIGHT);
                else if (column % 2 == 0)
                    printf(TOP_INTER);
                else
                    printf(TOP);
                continue;
            }
            //print the bottom
            else if(row == ROWS*2){
                if(column == 0)
                    printf(BOTTOM_LEFT);
                else if(column == COLUMNS*2)
                    printf(BOTTOM_RIGHT);
                else if (column % 2 == 0)
                    printf(BOTTOM_INTER);
                else
                    printf(BOTTOM);
                continue;
            }
            else if(row % 2 == 1){
                 if (column == 0)
                    printf(LEFT);
                else if (column == COLUMNS*2)
                    printf(RIGHT);
                else if (column % 2 == 0)
                    printf(LEFT);
                else
                    printf("%s", board[row/2][column/2] == 0 ? " " : board[row/2][column/2] == 1 ? WHITE_CIRCLE : BLACK_CIRCLE);
            }
            else{
                if (column == 0)
                    printf(LEFT_INTER);
                else if (column == COLUMNS*2)
                    printf(RIGHT_INTER);
                else if (column % 2 == 0)
                    printf(INTERSECTION);
                else
                    printf(BOTTOM);
            }
        }
        printf("\n");
    }
    printf("  ");
    for(int column=0; column < ROWS; column++){
        printf(" %d", column+1);
    }
    printf("\n");
    
}