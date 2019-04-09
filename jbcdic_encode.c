#include <stdio.h>
#include <stdlib.h>

#define ROW 4
#define COLUMN 12
#define NOTFOUND 21

typedef unsigned char BYTE;

static const char UPPER_CASE[ROW][COLUMN] = {
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '-'},
    { '\'', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', '[', ']'},
    { '\"', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', '{', '}'},
    { '*',  ' ', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', ','}
};

static const char LOWER_CASE[ROW][COLUMN] = {
    { '|', '!', '*', '#', '*', '%', '&', '^', '?', '.', ';', '='},
    { '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', '(', ')'},
    { '\\', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', '<', '>'},
    { '`',  '~', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '/', ':'}
};

int main()
{
    void to_byte(char *input, BYTE *output,int size_in, int size_out);
    void ascii_to_jbcdic(char s, int *style, int *current_row, BYTE *buffer);

    char input[] = "[{TS+316123456B+3-71T+21-3H+67-2L+400C2+1134U+FA+N}{TS+316123516B+3-7T+21-35H+67L+480C2+1156U+TA+567}]";
    BYTE output[4*sizeof(input)];

    printf("char : %d\n", sizeof(input));

    to_byte(input, output, sizeof(input),0);

    int i=0;
    while(output[i] != '\0'){
        printf("%X ",output[i++]);
    }
    return 0;
}

void to_byte(char *input, BYTE *output, int size_in, int size_out) {
    void ascii_to_jbcdic(char s, int *style, int *current_row, BYTE *buffer);

    int current_row = 0, style = 1;
    int i,j;
    int index_output=0;
    BYTE buffer[3] = {0,0,0};
    int tmp = 0;
    int write = 0;

    for(i = 0; i < size_in; i++) {
        if(input[i] != '\0') {

            ascii_to_jbcdic(input[i], &style, &current_row, buffer);
            printf("iteration : %d   ----char courant : %c style : %d row : %d char : %d\n",i, input[i],buffer[0],buffer[1],buffer[2]);
            for(j = 0; j < 3; j++){
                if(buffer[j] != NOTFOUND){
                    tmp = tmp * 16 + buffer[j];
                    write++;
                }
                if( tmp > 16 || write == 2){
                    printf("index output : %d - tmp : %d\n", index_output, tmp);
                    output[index_output] = tmp;
                    index_output++;
                    tmp = 0;
                    write = 0;
                }
            }
        }
        else {
            i = size_in +1;
            if( tmp > 16 ){
                output[index_output] = tmp;
                index_output++;
            }
            else if(tmp > 0){
                output[index_output] = tmp*16 + 15;
            }
        }
    }
}

void ascii_to_jbcdic(char s, int *style, int *current_row, BYTE *buffer) {
    int i, j, tmp_style;
    int r = 0, c = 0;
    for(i = 0; i < ROW; i++) {
        for(j = 0; j < COLUMN; j++) {
            if(UPPER_CASE[i][j] == s){
                r = i;
                c = j;
                i = ROW + 1;
                j = COLUMN + 1;
                tmp_style = 1;
            }
            if(LOWER_CASE[i][j] == s){
                r = i;
                c = j;
                i = ROW + 1;
                j = COLUMN + 1;
                tmp_style = 0;
            }
        }
    }
    buffer[0] = NOTFOUND;
    if(*style != tmp_style){
        buffer[0] = *current_row;
        *style = s;
    }
    buffer[1] = NOTFOUND;
    if(*current_row != r) {
        buffer[1] = 12+r;
        *current_row = r;
    }
    buffer[2] = c;
}

