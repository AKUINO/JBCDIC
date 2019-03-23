# BCDIC
BCDIC (Binary-Coded Decimal Interchange Code) adapted to support the part of ASCII used by JSON (with escape mechanism toward UTF-8) to compress most characters into 4 bits

Problem: Sending JSON like data (mostly numerical) by Radio must be done in a compressed way. Compression algorithms are often complex and not "programmer friendly". We make a proposal based on the lessons of history, the BCDIC from the 1930's (never underestimate our ancestors!): https://en.wikipedia.org/wiki/BCD_(character_encoding)#IBM_704_BCD_code .

What we mean by "JSON like" is a structure allowing to represent JSON data using a fields' dictionnary translating between a "shortcut" (few uppercase letters) and a full applicative field name. This translation can be done rather late (e.g. in the application server) if the sensors are managed from there.

## Table of 48 characters in 4 rows of 16 codes

We have 2 x 48 possible characters: 26 letters, 10 digits, punctuation and escape to UTF-8
A string always starts in Upper Case, row 0.

UPPER case Table:

r|0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F
-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-
r0|0|1|2|3|4|5|6|7|8|9|+|-|.|r1|r2|r3
r1| |A|B|C|D|E|F|G|H|I|\[|\]|r0|Lower|r2|r3
r2|,|J|K|L|M|N|O|P|Q|R|\{|\}|r0|r1|Lower|r3
r3|\<UTF-8\>|\"|S|T|U|V|W|X|Y|Z|_|\'|r0|r1|r2|Lower

If a "rX" is repeated, it shifts to lower case table (and vice-versa):

lower case Table:

r|0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F
-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-
r0| \| |!|*|#|$|%|&|^|?|;|=| \~ |Upper|r1|r2|r3
r1|@|a|b|c|d|e|f|g|h|i|(|)|r0|Upper|r2|r3
r2| \\ |j|k|l|m|n|o|p|q|r|\<|\>|r0|r1|Upper|r3
r3| ^ |\`|s|t|u|v|w|x|y|z|/|:|r0|r1|r2|Upper

The \<UTF-8\> code ignores the remaining of the current byte (if any) and indicates that UTF-8 characters are following up to a byte with 0xFF (255) (something never permitted in Unicode UTF-8).

Following such a structure ensures the flexibility of JSON whilst achieving a correct compression. The choosen format is character based and 37,5% less efficient than pure binary representation (if and only if the number of bits for the binary representation is exactly provided: rarely done and produce an overhead to provide this exact length): we think it is an excellent compromise between compression, flexibility (JSON is way more flexible than fixed binary data arrays) and simplicity.

## JSON structure and compression rules

RFC-7159 (JSON) ignores all spacing (outside strings) and indicates the following delimiters:

Concept|Character|Compressed representation
-------|---------|-------------------------
begin-array|\[ left square bracket|Starts an Array(value)|Mandatory
begin-object|\{ left curly bracket|Starts an Object|Mandatory
end-array|\] right square bracket|Ends an Array(value or member)|Mandatory
end-object|\} right curly bracket|Ends an Object|Mandatory
name-separator|\: colon|After a name, Starts a value, an object or an array|Will be ommitted even if digits are not allowed in names because a "+" will be mandatory before positive numbers
value-separator|\, comma|Separates "members" in objects (then starts a name) or "values" in arrays (then starts a value, an object or an array)

In compressed representation:
* a name starts with a letter ("A" to "Z", "a" to "z", "\_" and never with a digit), followed by letters, digits or "\_" and ends with the start of a value, or end of current array, or end of current object
* a value is number, a string, an array or an object
* a numerical value starts with "+" or "-"
* a string value starts with \" or \' (and ends with the same) or with \<UTF-8\> (ends with a byte with 0xFF (255))
* an object starts with \{ and ends with \} and contains "members" (pairs of name+value)
* an array starts with \[ and ends with \] and contains "values"

## Decoding
The decoding should first read the input buffer by chunks of 4 bits. It starts in the Upper case Table, row 0.
The characters to generate are found by addressing the right row in the right table. If the UTF-8 begin character is detected, the following byte (not chunk) is the beginning of an UTF-8 string to be appended as is. This string ends with a byte containing #255 (0xFF) which never appears in a valid UTF-8 (or at the end of the input buffer).

## Encoding
A reverse table will be created from the Decoding tables. As the tables only include regular ASCII characters (no €, °, ², ³ or whatsoever), none are above #126 (0x7E) or lower than #32 (0x20). In this "reverse" table, the ASCII characters will be marked as unsupported or present in table "lower" or "UPPER", at a specific row and a specific 4 bits value (11 or lower).

If the table is different than the current one, a chunk is generated with the 4 bits value = 12+"current row" (shift to the other table)

If the row is different form the current one, a chunk is generated with the 4 bits value = 12+"requested row" (shift to the desirer row)

A chunk is generated with the desired character position (4 bits value between 0 and 11).

If the desired character is not present, a character \<UTF-8\> is outputed, the current byte is finished and the following characters are copied as-is up to the end of the string needed to be added. If other characters will be added afterward, a byte #255 (0xFF) is appended.

## Development

A small C++ library should be developped to support this format. It could also be implemented in JavaScript and Python if it proves useful for compressed transmission in other circumstances.

A distinction must be made between strings ready to be transmitted and strings that may still be processed. In the second case, we recommend to record separately the beginning position and the end position in the encoding table: this will allow to generate the right change sequence for table position when concatenating different strings together.

By the way we recommend to store the information to send in "segments" and to concatenate segments within messages just before transmission.
