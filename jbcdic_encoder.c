/**
* \file jbcdic_encoder.c
* \brief This program allows to encode and decode JBCDIC format.
* \author Maxime Franco
* \version 1
* \date 16 avril 2019
*
*    This program allows to encode and decode JBCDIC format
*    Copyright (C) 2019  Maxime Franco
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
* Definition of ROW : constant int value 4 because the different alphabets contain 4 ROW.
*/

#define ROW 4
/**
* Definition of COLUMN : constant int value 12 because all alphabets contain 12 column
*/
#define COLUMN 12

/**
* Definition of NB_ALPHABET : constant int value 2 because there are 2 type of alphabet.
*/
#define NB_ALPHABET 2

/**
* Definition of CHAR_AlPHABET : constant int value 48 because there are 48 chars by alphabet
*/
#define CHAR_ALPHABET 48

/**
* Definition of CHAR_MIN : constant int value 32 because the minimal ASCII char accepted is 32.
*/
#define CHAR_MIN 32

/**
* Definition of CHAR_MAX : constant int value 127 because the minimal ASCII char accepted is 127.
* /
#define CHAR_MAX 127


/**
* Definition of the alphabet.
*/
static const char ALPHABET[NB_ALPHABET][ROW][COLUMN] = {
    {
        { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '-'},
        { '\'', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', '[', ']'},
        { '\"', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', '{', '}'},
        { 0x7F,  ' ', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', ','}
    },
    {
        { '|', '!', '*', '#', '*', '%', '&', '^', '?', '.', ';', '='},
        { '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', '(', ')'},
        { '\\', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', '<', '>'},
        { '`',  '~', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '/', ':'}
    }
};

/**
 * \struct jbcdic_char
 * \brief this structure allows to represent a jbcdic char with all coordinate to make the link with the ALPHABET.
 *
 * this structure allows to represent a jbcdic char with all coordinate to make the link with the ALPHABET.
 */

struct jbcdic_char{
    uint8_t alphabet:1;
    uint8_t row:2;
    uint8_t column:4;
    uint8_t :0;
} pack;

typedef struct jbcdic_char jbcdic_char;

static jbcdic_char ASCII_JBCDIC[128 - 32] = {0};


int main()
{

    char input[] = "[{ts+316123456B+3-71T+21-3H+67-2L+400C2+1134U+FA+N}{TS+316123516B+3-7T+21-35H+67L+480C2+1156U+TA+567}]";
    uint8_t output[4*sizeof(input)];
    int test = encode_to_jbcdic(input, strlen(input), output, sizeof(output));
    printf("char entrée : %d\n", strlen(input));
    int i=0;
    while(i < test){
        printf("%X ",output[i]);
        i++;
    }

    char test2[4*sizeof(input)];
    test = decode_from_jbcdic(output, test, test2, sizeof(test2));
    printf("char sortie : %d\n", test);
    i=0;
    while(i < test){
        printf("%c",test2[i]);
        i++;
    }
    return 0;
}


/**
* \fn int encode_to_jbcdic(char *input,int size_in, uint8_t *output, int size_out)
* \brief Definition of the encode_to_jbdic function. This function transforms the input char buffer into a output char buffer in jbcdic format.
* \param *input variable is a pointer to the input buffer.
* \param size_in variable is the length of the input buffer.
* \param *output variable is a pointer to the output buffer.
* \param size_out variable is the length of the output buffer.
* \return The function returns the number of byte written into the output buffer.
*/
int encode_to_jbcdic(char *input,int size_in, uint8_t *output, int size_out) {

    int i, j, k, row = 0, alphabet = 0, index_output = 0, started = 0;

    // loading encoder
    load_encoder();

    for(i = 0; i < size_in && input[i]; i++){
        if(index_output < size_out && input[i] >= CHAR_MIN && input[i] <= CHAR_MAX) {
            int index = input[i] - CHAR_MIN;

            // encodage changement alphabet
            if(ASCII_JBCDIC[index].alphabet != alphabet){
                write_char(output, (COLUMN + row), &index_output, &started);
                alphabet = ASCII_JBCDIC[index].alphabet;
            }

            //encodage changement row
            if(ASCII_JBCDIC[index].row != row){
                write_char(output, COLUMN + ASCII_JBCDIC[index].row, &index_output, &started);
                row = ASCII_JBCDIC[index].row;
            }

            //encodage character
            write_char(output, ASCII_JBCDIC[index].column, &index_output, &started);

        }
        else{
            return -1;
        }
    }
    if(started == 1){
        output[index_output] += 15;
        index_output++;
        started = 0;
    }
    return index_output;
}

/**
* \fn void write_char(char *output, int value, int *index, int *started).
* \brief this method allows to write a char inside the output buffer.
* \param *output : the pointer to the output buffer.
* \param value : the value to be written into the buffer.
* \param index : index where the value will be written inside the buffer.
* \param *started : boolean that says if the value must be written inside the first bits or inside the last bits.
*/

void write_char(char *output, int value, int *index, int *started){
    if(*started == 0){
        output[*index] = value << 4;
        *started = 1;
    }
    else{
        output[*index] += value;
        *started = 0;
        *index = *index + 1;
    }
}


/**
* \fn int decode_from_jbcdic(uint8_t *input, int size_in, char *output, int size_out)
* \brief Definition of the decode_from_jbcdic function. This function transforms the input jbcdic uint8_t buffer into a output char buffer in ASCII format.
* \param *input variable is a pointer to the input buffer.
* \param size_in variable is the length of the input buffer.
* \param *output variable is a pointer to the output buffer.
* \param size_out variable is the length of the output buffer.
* \return The function returns the number of byte written into the output buffer.
*/
int decode_from_jbcdic(uint8_t *input, int size_in, char *output, int size_out){
    int i, j, row = 0, alphabet = 0, index_output = 0;

    for(i = 0; i < size_in; i++) {
        uint8_t char_jbcdic[2] = {input[i] >> 4, input[i] & 0x0F};
        for(j = 0; j < 2; j++) {
            if(index_output < size_out) {
                if(char_jbcdic[j] >= COLUMN && char_jbcdic[j] == COLUMN + row){
                    alphabet = !alphabet;
                }
                else if(char_jbcdic[j] >= COLUMN){
                    row = char_jbcdic[j] - COLUMN;
                }
                else{
                    output[index_output] = ALPHABET[alphabet][row][char_jbcdic[j]];
                    index_output++;
                }
            }
            else{
                return -1;
            }
        }
    }
    return index_output;
}


/**
* \fn void load_encoder()
* \brief load_encoder allows to transform the ALPHABET into a array of jbcdic_char to find in O(1) amortized the JBCDIC char from a ASCII char
*/
void load_encoder(){
    int i,j,k;
    if (ASCII_JBCDIC[0].row == 0) {
        for(i = 0; i < NB_ALPHABET; i++){
            for(j = 0; j < ROW; j++){
                for(k = 0; k < COLUMN; k++){
                    unsigned int index = ALPHABET[i][j][k] - CHAR_MIN;
                    ASCII_JBCDIC[index].alphabet = i;
                    ASCII_JBCDIC[index].row = j;
                    ASCII_JBCDIC[index].column = k;
                }
            }
        }
    }
}

