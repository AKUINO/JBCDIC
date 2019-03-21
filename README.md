# BCDIC
BCDIC (Binary-Coded Decimal Interchange Code) adapted to support the part of ASCII used by JSON-LD (with escape mechanism toward UTF-8) to compress most characters into 4 bits

Problem: Sending JSON like data (mostly numerical) by Radio must be done in a compressed way. Compression algorithms are often complex and not "programmer friendly". We make a proposal based on the lessons of history, the BCDIC from the 1930's (never underestimate our ancestors!): https://en.wikipedia.org/wiki/BCD_(character_encoding)#IBM_704_BCD_code .

What we mean by "JSON like" is a structure allowing to represent JSON data using a fields' dictionnary translating between a "shortcut" (few uppercase letters) and a full field name.

RFC-7159 (JSON) ignores all spacing (outside strings) and indicates the following delimiters:

Concept|Character|Compressed representation
-------|---------|-------------------------
begin-array|\[ left square bracket
begin-object|\{ left curly bracket
end-array|\] right square bracket
end-object|\} right curly bracket
name-separator|\: colon|Start a value, an object or an array
value-separator|\, comma|Start a name, a value, an object or an array

In compressed representation:
* a numerical value starts with "0","-","."
* a string value starts with '"' or <UTF-8>
* a name starts with a letter ("A" to "Z", "_")
* an object starts with \{
* an array ends with \]
We have 48 characters possible: 26 letters, 10 digits, '-', '.', '"', '_', <UTF-8>, '{', '}', '[', ']', space, ','

r|0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F
-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-
r0|0|1|2|3|4|5|6|7|8|9|-|.|Lower|r1|r2|r3
r1| |A|B|C|D|E|F|G|H|I|\[|\]|r0|Lower|r2|r3
r2|/|J|K|L|M|N|O|P|Q|R|\{|\}|r0|r1|Lower|r3
r3|\<UTF-8\>|\"|S|T|U|V|W|X|Y|Z|_|,|r0|r1|r2|Lower

If a "rX" is repeated, this could change the table to shift in lower case (or vice-versa):

r|0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|F
-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-
r0|°|!|²|³|#|%| \| |=|&|?|:|;|Upper|r1|r2|r3
r1|@|a|b|c|d|e|f|g|h|i|(|)|r0|Upper|r2|r3
r2| \\ |j|k|l|m|n|o|p|q|r|\<|>|r0|r1|Upper|r3
r3| € |\'|s|t|u|v|w|x|y|z|+|=|r0|r1|r2|Upper

The \<UTF-8\> code ignores the remaining of the current byte (if any) and indicates that UTF-8 characters are following up to a byte with 0xFF (255) (something never permitted in Unicode UTF-8). 
