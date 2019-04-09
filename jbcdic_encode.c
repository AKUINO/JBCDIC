#include <stdio.h>
#include <stdlib.h>

/*
* Definition of ROW : constant int value 4 because the different alphabets contain 4 ROW.
* Definition of COLUMN : constant int value 12 because all alphabets contain 12 column
* Definition of NOTFOUND : constant int value 21. Value set in the char when there is no char to add, catch if the encode_to_jbcdic function.
*/

#define ROW 4
#define COLUMN 12
#define NOTFOUND 21

/*
* Definition of BYTE as a unsigned char.
*/

typedef unsigned char BYTE;


/*
* Definition of the UPPER_CASE alphabet.
*/

static const char UPPER_CASE[ROW][COLUMN] = {
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '-'},
    { '\'', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', '[', ']'},
    { '\"', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', '{', '}'},
    { '*',  ' ', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', ','}
};

/*
* Definition of the LOWER_CASE alphabet.
*/

static const char LOWER_CASE[ROW][COLUMN] = {
    { '|', '!', '*', '#', '*', '%', '&', '^', '?', '.', ';', '='},
    { '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', '(', ')'},
    { '\\', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', '<', '>'},
    { '`',  '~', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '/', ':'}
};

int main()
{
    int encode_to_jbcdic(char *input, BYTE *output,int size_in, int size_out);
    void ascii_to_jbcdic(char s, int *style, int *current_row, BYTE *buffer);

    char input[] = "[{TS+316123456B+3-71T+21-3H+67-2L+400C2+1134U+FA+N}{TS+316123516B+3-7T+21-35H+67L+480C2+1156U+TA+567}]";
    BYTE output[4*sizeof(input)];

    printf("char : %d\n", sizeof(input));

    int test = encode_to_jbcdic(input, output, sizeof(input),4*sizeof(input));
    printf("char ecrit : %d\n", test);

    int i=0;
    while(output[i] != '\0'){
        printf("%X ",output[i++]);
    }
    return 0;
}

/*
* Definition of the encode_to_jbdic function. This function transforms the input char buffer into a output char buffer in jbcdic format.
*                                             The function returns the number of byte written into the output buffer.
* The char *input variable is a pointer to the input buffer.
* The char *output variable is a pointer to the output buffer.
* The int size_in variable is the length of the input buffer.
* The int size_out variable is the length of the output buffer.
*/

int encode_to_jbcdic(char *input, BYTE *output, int size_in, int size_out) {
    void ascii_to_jbcdic(char s, int *style, int *current_row, BYTE *buffer);

    int current_row = 0, style = 1;
    int i,j;
    int index_output=0;
    BYTE buffer[3] = {0,0,0};
    int tmp = 0;
    int write = 0;

    for(i = 0; i < size_in; i++) {
        if(index_output <=size_out){
            if(input[i] != '\0') {
                ascii_to_jbcdic(input[i], &style, &current_row, buffer);
                for(j = 0; j < 3; j++){
                    if(buffer[j] != NOTFOUND){
                        tmp = tmp * 16 + buffer[j];
                        write++;
                    }
                    if( tmp > 16 || write == 2){
                        output[index_output] = tmp;
                        index_output++;
                        tmp = 0;
                        write = 0;
                    }
                }
            }
            else {
                i = size_in +1;
                if( tmp > 16 || write == 2){
                    output[index_output] = tmp;
                    index_output++;
                    write = 0;
                }
                else if(tmp > 0 || write == 1){
                    output[index_output] = tmp*16 + 15;
                    index_output++;
                }
            }
        }
    }
    return index_output;
}

/*
* Definition of the ascii_to_jbcdic function. This function receives a input char, the current type of alphabet(lower case or upper case) and the current row in the alphabet.
*                                             This function returns the input char to the jbcdic format and may modifies the style or th current_row if they're changed.
* The char s is the input character.
* The *style is the pointer to the current style.
* The *current_row is the pointer to the current row.
* The *buffer is the pointer to the output buffer.
*/

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
