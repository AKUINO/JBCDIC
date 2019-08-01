// We are not encoding JSON but URL ...
#define UBCDIC
#include "JBCDIC.h"


/**
* \fn void load_encoder()
* \brief load_encoder allows to transform the ALPHABET into a array of jbcdic_char to find in O(1) amortized the JBCDIC char from a ASCII char
*/
void JBCDIC::load_encoder(){
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


/**
* \fn void write_char(char *output, int value, int *index, int *started).
* \brief this method allows to write a char inside the output buffer.
* \param *output : the pointer to the output buffer.
* \param value : the value to be written into the buffer.
* \param index : index where the value will be written inside the buffer.
* \param *started : boolean that says if the value must be written inside the first bits or inside the last bits.
*/

void JBCDIC::write_char(uint8_t *output, int value, int *index, int *started){
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
* \fn int encode_to_jbcdic(char *input,int size_in, uint8_t *output, int size_out)
* \brief Definition of the encode_to_jbdic function. This function transforms the input char buffer into a output char buffer in jbcdic format.
* \param *input variable is a pointer to the input buffer.
* \param size_in variable is the length of the input buffer.
* \param *output variable is a pointer to the output buffer.
* \param size_out variable is the length of the output buffer.
* \return The function returns the number of byte written into the output buffer.
*/
int JBCDIC::encode_to_jbcdic(char *input,int size_in, uint8_t *output, int size_out) {

    int i, row = 0, alphabet = 0, index_output = 0, started = 0;

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
* \fn int decode_from_jbcdic(uint8_t *input, int size_in, char *output, int size_out)
* \brief Definition of the decode_from_jbcdic function. This function transforms the input jbcdic uint8_t buffer into a output char buffer in ASCII format.
* \param *input variable is a pointer to the input buffer.
* \param size_in variable is the length of the input buffer.
* \param *output variable is a pointer to the output buffer.
* \param size_out variable is the length of the output buffer.
* \return The function returns the number of byte written into the output buffer.
*/
int JBCDIC::decode_from_jbcdic(uint8_t *input, int size_in, char *output, int size_out){
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

