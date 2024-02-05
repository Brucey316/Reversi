#include "driver.h"

BOARD board;
//keeps track of the number of turns taken
int turn = 0;
//keeps track of if anyone has gone more than once
int turn_offset = 0;
//keeps track of perimeter pieces [8bit row | 8bit column]
//used to effeciently check if a move can be made without checking all locations
Location* perimeter_pieces = NULL;
//size of perimeter array
int perimeter_sz = 0;

int main(int argc, char* argv[]){
    init();
    print_board();
    while(turn < MAX_TURNS_TEST3){
        printf("%s's Turn\n", ((turn%2) ^ turn_offset) ? "WHITE" : "BLACK");
        
        int row, column;
        row = column = -1;
        get_input(&row, &column);
        if (!make_move(row, column))
            continue;
        update_perimeter(row, column);
        print_board();
        turn++;
        /*
        for(int i = 0; i < perimeter_sz; i++){
            printf("Row %d Column %d\n", perimeter_pieces[i].row, perimeter_pieces[i].column);
        }
        */
        turn_offset ^= has_valid_move() ^ 1;
        //if(turn_offset) printf("\nTURN OFFSET IS ON!!!!\n\n");
    }
    
    free(perimeter_pieces);
    return 0;
}

void init(){
    //completely wipe the board
    for(int i = 0; i < ROWS; i++){
        memset(board[i], 0, SLOT_SIZE*COLUMNS);
    }
    //initialize the middle starting squares
    board[3][3] = 1;
    board[4][4] = 1;
    board[3][4] = 2;
    board[4][3] = 2;

    //set up initialization for the perimeter array
    perimeter_pieces = (Location*)calloc(12, sizeof(Location));

    int index=0;
    // Iterate over the surrounding locations
    for (int i = 2; i <= 5; ++i) {
        for (int j = 2; j <= 5; ++j) {
            // Skip the middle starting squares
            if ((i == 3 || i == 4) && (j == 3 || j == 4)) {
                continue;
            }
            // Extract the row and column information and store in location
            perimeter_pieces[index].location = (uint16_t)((i << 8) | j);
            index++;
            perimeter_sz++;
        }
    }

    // Print the results for verification
    /*
    for (int i = 0; i < 12; ++i) {
        printf("Location %d: Row = %d, Column = %d\n", i, perimeter_pieces[i].row, perimeter_pieces[i].column);
    }
    */
}

void get_input(int* row, int* column){
    printf("Enter the \"<row> <column>\" you wish to place a piece on\
            \nEnsure there is only one space\
            \n:");
    
    //set up buffer to read input
    size_t buff_sz = 10;
    char* buffer = calloc(buff_sz, 1);
    //get input from stdin
    getline(&buffer, &buff_sz, stdin);
    //check input for user input in the form of "<row> <column>"
    //make sure input was read in
    if( sscanf(buffer, "%d %d", row, column) != 2)
        *row = *column = -1;
    //printf("Read in %d %d\n", *row, *column);
    (*row) --;
    (*column) --;
    
    //free buffer
    free(buffer);
}

int make_move(int row, int column){
    Line** lines = (Line**) malloc(sizeof(Line*));
    uint8_t num_lines = 0;
    //change on has_valid_move as well
    piece_t player = ((turn%2) ^ turn_offset) + 1;
    piece_t opponent = player ^ 3;
    num_lines = is_valid_move(row, column, &lines, player, opponent);

    //if invalid move
    if(num_lines == 0){
        free(lines);
        return 0;
    }

    //if valid move
    flip_pieces(lines, num_lines);
    
    //assign spot the value
    board[row][column] = player;

    //free lines
    dealloc_lines(lines, num_lines);
    return 1;
}

int is_valid_move(int row, int column, Line*** linesp, piece_t player, piece_t opponent){
    //check if in bounds
    if( !in_bounds(row, column) )
        return 0;
    //check what piece the position is
    piece_t start = board[row][column];

    //if spot is occupied not valid move
    if (start != 0)
        return 0;
    
    //if no opponents arround
    return check_neighbors(row, column, player, opponent, linesp);
}

int in_bounds(int row, int column){
     if( row < 0 || row >= ROWS || column < 0 || column >= COLUMNS) 
        return 0;
    return 1;
}

int check_neighbors(int row, int column, piece_t player, piece_t opponent, Line*** lines){
    uint8_t sum = 0;
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
            Line* line = is_sandwich(row, column, dir[dir1], dir[dir2], player);
            //if no sandwhich found, look other directions
            if (line == NULL)
                continue;
            //printf("Has line in direction %d, %d\n", dir[dir1], dir[dir2]);
            //if sandwhich add to list
            sum ++;
            (*lines) = (Line**) realloc(*lines, sizeof(Line*) * sum);
            (*lines)[sum-1] = line;
        }
    }
    return sum;
}

Line* is_sandwich(int row, int column, int dir1, int dir2, piece_t player){
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
        Line* head = lines[line];
        //iterate through linked list and flip pieces
        while (lines[line] != NULL){
            *(lines[line]->piece) ^= 3;
            lines[line] = lines[line]->next;
        }
        lines[line] = head;
    }
}

int has_valid_move(){
    int has_moves = 0;
    piece_t player = ((turn%2) ^ turn_offset) + 1;
    piece_t opponent = player ^ 3;
    int i;
    for(i = 0; i < perimeter_sz && !has_moves; i++){
        Line** lines = (Line**) malloc(sizeof(Line*));
        //printf("Checking player %d: %d %d\n", player, perimeter_pieces[i].row, perimeter_pieces[i].column);
        has_moves = is_valid_move(perimeter_pieces[i].row, perimeter_pieces[i].column, &lines, player, opponent);
        if(lines == NULL){
            free(lines);
            continue;
        }
        dealloc_lines(lines, has_moves);
    }
    /*if(has_moves)
        printf("Move Found (%d): %d %d\n", has_moves, perimeter_pieces[i].row, perimeter_pieces[i].column);

    else
        printf("No Move Found\n");*/
    return has_moves > 0;
}

void update_perimeter(int row, int column){
    Location* piece = NULL;
    //Find item in list
    for(int i = 0; i < perimeter_sz && piece == NULL; i++){
        if(perimeter_pieces[i].row == row && perimeter_pieces[i].column == column)
            piece = &(perimeter_pieces[i]);
    }
    if(piece == NULL){
        printf("FATAL ERROR: PIECE (%d, %d) NOT FOUND IN LIST\n", row, column);
        exit(1);
    }
    int dir[] = {-1,0,1};
    for(int dir1 = 0; dir1 < 3; dir1 ++){
        for(int dir2 = 0; dir2 < 3; dir2++){
            //dont check piece placed
            if(dir1 == 1 && dir2 == 1) continue;
            //assign new row/column variables
            int nrow = row+dir[dir1];
            int ncolumn = column+dir[dir2];
            //check if in bounds
            if(! in_bounds(nrow, ncolumn)) continue;
            //check if place is unoccupied
            if(board[nrow][ncolumn] != 0) continue;
            //check if piece is already in list
            int i = 0;
            for(i = 0; i < perimeter_sz; i++){
                //printf("inspecting: %d, %d\n", perimeter_pieces[i].row, perimeter_pieces[i].column);
                if( ((nrow << 8) | ncolumn) == perimeter_pieces[i].location)
                    break;
            }
            //if broken forloop early, check new direction
            if (i != perimeter_sz){ 
                //printf("matched %d %d with %d %d\n", nrow, ncolumn, perimeter_pieces[i].row, perimeter_pieces[i].column);
                continue;
            }
            //printf("Adding: %d %d\n", nrow, ncolumn);
            //First adjacent space can replace the piece just placed
            if(piece != NULL){
                piece->location = (uint16_t) ((nrow << 8) | ncolumn);
                piece = NULL;
            }
            //Other adjacent spaces can be placed at the end of the list
            else{
                perimeter_sz++;
                //printf("Perimeter_sz: %d\n", perimeter_sz);
                perimeter_pieces = (Location*) realloc(perimeter_pieces, sizeof(Location) * perimeter_sz);
                perimeter_pieces[perimeter_sz-1].location = (uint16_t) ((nrow << 8) | ncolumn);
            }
        }
    }
    //If adjacent spaces already corrected, exit
    if(piece == NULL) return;

    //If no adjacent space, shift array down
    Location* last = NULL;
    int found = 0;
    for(int i = 0; i < perimeter_sz; i++){
        if(memcmp(piece, &(perimeter_pieces[i]), sizeof(Location)) == 0){
            found = 1;
            last = &(perimeter_pieces[i]);
            continue;
        }
        else if(!found) continue;
        //update previous location with current location
        last->location = perimeter_pieces[i].location;
        //set pointer for current location as last location for next shift
        last = &(perimeter_pieces[i]);
    }
    //adjust the size counter
    perimeter_sz--;
    //deallocate necessary memory
    perimeter_pieces = (Location*) realloc(perimeter_pieces, sizeof(Location) * perimeter_sz);
}

void dealloc_lines(Line** lines, int num_lines){
    //deallocate the line array
    for(int line = 0; line < num_lines; line++){
        while(lines[line] != NULL){
            Line* last = lines[line];
            lines[line] = lines[line]->next;
            free(last);
        }
    }
    free(lines);
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
            //print the rows that hold the pieces
            else if(row % 2 == 1){
                 if (column == 0)
                    printf(LEFT);
                else if (column == COLUMNS*2)
                    printf(RIGHT);
                else if (column % 2 == 0)
                    printf(LEFT);
                else
                    printf("%s", board[row/2][column/2] == 0 ? " " : board[row/2][column/2] == 1 ? BLACK_CIRCLE : WHITE_CIRCLE);
            }
            //print the rows that hold the border lines in between
            else{
                //print the left edge
                if (column == 0)
                    printf(LEFT_INTER);
                //print the right edge
                else if (column == COLUMNS*2)
                    printf(RIGHT_INTER);
                //print the middle intersections
                else if (column % 2 == 0)
                    printf(INTERSECTION);
                //print the border under the pieces
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