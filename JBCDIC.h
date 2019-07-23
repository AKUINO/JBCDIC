#ifndef JBCDIC_H
#define JBCDIC_H
/**
* \file jbcdic_encoder.c
* \brief This program allows to encode and decode JBCDIC format.
* \author Maxime Franco
* \version 2
* \date 23 juillet 2019
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
#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <string.h>

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
*/
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
};

typedef struct jbcdic_char jbcdic_char;

static jbcdic_char ASCII_JBCDIC[128 - 32] = {0};

class JBCDIC
{
    public:
        static void load_encoder();
        static void write_char(uint8_t *output, int value, int *index, int *started);
        static int encode_to_jbcdic(char *input,int size_in, uint8_t *output, int size_out);
        static int decode_from_jbcdic(uint8_t *input, int size_in, char *output, int size_out);

};

#endif // JBCDIC_H
