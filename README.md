# JBCDIC
BCDIC (Binary-Coded Decimal Interchange Code) adapted to support the part of ASCII necessary to represent JSON data to compress most characters into 4 bits. An escape mechanism allows UTF-8 strings to be inserted as is.

Problem: Sending JSON like data (mostly numerical) by Radio must be done in a compressed way. Compression algorithms are often complex and not "programmer friendly". We make a proposal based on the lessons of history, the BCDIC from the 1930's (never underestimate our ancestors!): https://en.wikipedia.org/wiki/BCD_(character_encoding)#IBM_704_BCD_code .

It is bewildering to see how many proposals exist for Radio Data compression (list in reference at the end of this document). We reviewed many and makes this proposal anyway: if a reader would like to compare the compression rate with real and representative data (example: https://www.epj-conferences.org/articles/epjconf/pdf/2017/31/epjconf_incape2017_01073.pdf), we would be delighted !

The reader may like to have in mind the JSON specification: https://tools.ietf.org/html/rfc7159

JBCDIC is at first a representation of JSON data (UTF8 strings excepted) in a basic set of 48 characters (completed by a second one of the same size). And thereafter, it is a compression of those characters in 4 bits chunks.

Let's take an example of data transmitted:

`[{timestamp:316123456,battery:3.71,temperature:21.3,humidity:67.2,lumens:400,co2:1134,button:false,adc:null},{timestamp:316123516,battery:3.7,temperature:21.35,humidity:67,lumens:480,co2:1156,button:true,adc:567}]`

(we suppose timestamp is the number of seconds since a given date and we are 10 years later)

The compression is done using three levels:

1) Reduction of fields names and values: the fields names are reduced to one or few upper case letters or digits ("\_" also allowed). Special values like _true_, _false_, _null_ are represented with a "+" and one upper case letter (+T,+F,+N). Remaining upper case letters can be used for any other repeating string or number in the data. Translation dictionaries must be managed at application level.

2) Reduction of punctuation: spacing is discarded, ":" is discarded between names and values, "," is discarded between members of an object and between values of an array. Values can be:
  * UTF-8 strings: an escape character starts the string (row 3, character 0) and an "invalid UTF-8" character (255, 0xFF) ends the UTF-8 string
  * `'strings'` or `"strings"`
  * `+number+` or `-number+` for positive or negative integers. For all numerical values, the final "+" is necessary only if a negative number is following (only in arrays of values)
  * `+number-decimals+` or `-number-decimals+`
  * `+number-decimals-positive exponent+` or `+number-decimals--negative exponent+`
  * other numerical structures like time or longitude could be represented using "-" as a delimiter between parts of the structure

3) The resulting string is compressed using an encoding table providing 4 bits per character and a row switching mechanism. We expect most data to be encoded using the row 0 (digits, +, - ) and only 4 bits for each of those characters.

4) When sending, the 4 last bits of the last byte can be empty: they can be filled by a chunk with a value between 12 and 15.

Our example after 1) and 2) becomes:

`[{TS+316123456B+3-71T+21-3H+67-2L+400C2+1134U+FA+N}{TS+316123516B+3-7T+21-35H+67L+480C2+1156U+TA+567}]`

103 characters remaining out of 215 characters: a compression of 52% (not counting any original spacing)

Below, the same with stars where the 4 bits compression needs "row shifts" (4 bits cost):
`*[*{*TS*+316123456*B*+3-71*T*+21-3*H*+67-2*L*+400*C*2+1134*U*+*FA*+*N*}{*TS*+316123516*B*+3-7*T*+21-35*H*+67*L*+480*C*2+1156*U*+*T*A*+567*}*]`

140 chunks of 4 bits (70 bytes) are produced for 103 characters: a compression of 42%.

A total compression from "raw" JSON (no spacing) of 66% using realistic data. 

Notes:
* Application dictionary of names and values is essential to achieve a good performance.
* Acronyms may have to be chosen taking into account the rows in the encoding table (to avoid row shifts). For instance, using "RP" (record period) instead of "TS" for the timestamp would remove a byte from the resulting string.
* One may see that with a maximum data packet size of 53 bytes in LoRa: we would not send two sets of those example measures in one message.
* Timestamp is often represented using 4 bytes (binary representation): we are using 5 bytes for `+316123456`
* Small floating points numbers like temperature are often represented by 4 bytes in in binary format (single precision: 7 significant digits, well explained here http://www.cse.hcmut.edu.vn/~hungnq/courses/501120/docthem/Single%20precision%20floating-point%20format%20-%20Wikipedia.pdf ). We are using 2 bytes for `21.5`, 3 bytes for `-12.34` and 4.5 for `-123.4567` (6 bytes for `-1234567---4` which means _-1234567e-4_) : BCD is very competitive compared to binary and keeps the flexibility of a character based representation of data (grammar driven encoding/decoding)
* One more byte can also be saved by assuming a message is always an array of objects (measures sets): the opening and closing brackets can be assumed.

## Table of 48 characters in 4 rows of 16 codes

We have 2 x 48 possible characters: 26 letters, 10 digits, punctuation and escape to UTF-8
A string always starts in Upper Case, row 0.

UPPER case Table:

row|0|1|2|3|4|5|6|7|8|9|A (10)|B (11)|C (12)|D (13)|E (14)|F (15)
---|-|-|-|-|-|-|-|-|-|-|------|------|------|------|------|------
r0|0|1|2|3|4|5|6|7|8|9|+|-|Lower|r1|r2|r3
r1|\'|A|B|C|D|E|F|G|H|I|\[|\]|r0|Lower|r2|r3
r2|\"|J|K|L|M|N|O|P|Q|R|\{|\}|r0|r1|Lower|r3
r3|\<UTF-8\>| |S|T|U|V|W|X|Y|Z|_|,|r0|r1|r2|Lower

If a "rX" is repeated, it shifts to lower case table (and vice-versa):

lower case Table:

row|0|1|2|3|4|5|6|7|8|9|A (10)|B (11)|C (12)|D (13)|E (14)|F (15)
---|-|-|-|-|-|-|-|-|-|-|------|------|------|------|------|------
r0| \| |!|*|#|$|%|&|^|?|.|;|=|Upper|r1|r2|r3
r1|@|a|b|c|d|e|f|g|h|i|(|)|r0|Upper|r2|r3
r2| \\ |j|k|l|m|n|o|p|q|r|\<|\>|r0|r1|Upper|r3
r3| \` | \~ |s|t|u|v|w|x|y|z|/|:|r0|r1|r2|Upper

The \<UTF-8\> code ignores the remaining of the current byte (if any) and indicates that UTF-8 characters are following up to a byte with 0xFF (255) (something never permitted in Unicode UTF-8).

The example above can be compressed. We have row shifts between 0 and 3 (°'²³):
`'[²{³TS°+316123456'B°+3-71³T°+21-3'H°+67-2²L°+400'C°2+1134³U°+'FA°+²N}{³TS°+316123516'B°+3-7³T°+21-35'H°+67²L°+480'C°2+1156³U°+³T'A°+567²}']`

The characters are represented as an hexadecimal digit indicating their position (0 to 11, 0 to 0xB) in each row:
`'A²A³32°A316123456'2°A3B71³3°A21B3'8°A67B2²3°A400'3°2B1134³4°A'61°A²5B
A³32°A316123516'2°A3B7³3°A21B35'8°A67²3°A480'3°2A1156³4°A³3'1°A567²B'B`

The row shifts from 0 to 3 are coded using characters 0xC to 0xF:
`DAEAF32CA316123456D2CA3B71F3CA21B3D8CA67B2E3CA400D3C2B1134F4CAD61CAE5B
AF32CA316123516D2CA3B7F3CA21B35D8CA67E3CA480D3C2A1156F4CAF3D1CA567EBDB`

Corresponding bytes:
`DA EA F3 2C A3 16 12 34 56 D2 CA 3B 71 F3 CA 21 B3 D8 CA6 7B 2E 3C A4 00 D3 C2 B1 13 4F 4C AD 61 CA E5 BA F3 2C A3 16 12 35 16 D2 CA 3B 7F 3C A2 1B 35 D8 CA 67 E3 CA 48 0D 3C 2A 11 56 F4 CA F3 D1 CA 56 7E BD BF`
("F" added to the end (shift to row 3) to fill the last byte)

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
* a numerical value starts with "+" or "-" providing it signs. Dashes ("-") further in the number indicates the decimal point (".") and then the positive exponent and then the negative exponent. A numerical value must be ended by a "+" only if it is directly followed by a negative numerical value.
* a string value starts with \" or \' (and ends with the same) or with \<UTF-8\> (ends with a byte with 0xFF (255))
* an object starts with \{ and ends with \} and contains "members" (pairs of name+value)
* an array starts with \[ and ends with \] and contains "values"

## Decoding
The decoding should first read the input buffer by chunks of 4 bits. It starts in the Upper case Table, row 0.
The characters to generate are found by addressing the right row in the right table. If the UTF-8 begin character is detected, the following byte (not chunk) is the beginning of an UTF-8 string to be appended as is. This string ends with a byte containing #255 (0xFF) which never appears in a valid UTF-8 (or at the end of the input buffer).

Our first aim will be to start from a string of encoded bytes and obtain the result of 4bits decompression (JSON agnostic): a simple C function receiving a string and returning another (larger normally!). Example, going from:
`DA EA F3 2C A3 16 12 34 56 D2 CA 3B 71 F3 CA 21 B3 D8 CA6 7B 2E 3C A4 00 D3 C2 B1 13 4F 4C AD 61 CA E5 BA F3 2C A3 16 12 35 16 D2 CA 3B 7F 3C A2 1B 35 D8 CA 67 E3 CA 48 0D 3C 2A 11 56 F4 CA F3 D1 CA 56 7E BD BF`

to:
`[{TS+316123456B+3-71T+21-3H+67-2L+400C2+1134U+FA+N}{TS+316123516B+3-7T+21-35H+67L+480C2+1156U+TA+567}]`

A second function will be necessary later to transform this result in a JSON-LD compatible format (typically for transmission to a Web application):
`[{'TS':316123456,'B':3.71,'T':21.3,'H':67.2,'L':400,'C2':1134,'U':false,'A':null}[{'TS':316123516,'B':3.7,'T':21.35,'H':67,'L':480,'C2':1156,'U':true,'A':567}`

## Encoding
A reverse table will be created from the Decoding tables. As the tables only include regular ASCII characters (no €, °, ², ³ or whatsoever), none are above #126 (0x7E) or lower than #32 (0x20). In this "reverse" table, the ASCII characters will be marked as unsupported or present in table "lower" or "UPPER", at a specific row and a specific 4 bits value (11 or lower).

If the table is different than the current one, a chunk is generated with the 4 bits value = 12+"current row" (shift to the other table)

If the row is different from the current one, a chunk is generated with the 4 bits value = 12+"requested row" (shift to the desirer row)

A chunk is generated with the desired character position (4 bits value between 0 and 11).

If the desired character is not present, a character \<UTF-8\> is outputed, the current byte is finished and the following characters are copied as-is up to the end of the string needed to be added. If other characters will be added afterward, a byte #255 (0xFF) is appended.

If the last byte is only half filled, a chunk between 12 and 15 (0xC to 0xF) can be appended.

We need a function taking a string and returning its compressed version (which may be a bit larger if it is not usual JSON data). Example, going from:
`[{TS+316123456B+3-71T+21-3H+67-2L+400C2+1134U+FA+N}{TS+316123516B+3-7T+21-35H+67L+480C2+1156U+TA+567}]`

to the following bytes (hexadecimal):
`DA EA F3 2C A3 16 12 34 56 D2 CA 3B 71 F3 CA 21 B3 D8 CA6 7B 2E 3C A4 00 D3 C2 B1 13 4F 4C AD 61 CA E5 BA F3 2C A3 16 12 35 16 D2 CA 3B 7F 3C A2 1B 35 D8 CA 67 E3 CA 48 0D 3C 2A 11 56 F4 CA F3 D1 CA 56 7E BD BF`

as a C string:
`\xDA\xEA\xF3\x2C\xA3\x16\x12\x34\x56\xD2\xCA\x3B\x71\xF3\xCA\x21\xB3\xD8\xCA6\x7B\x2E\x3C\xA4\x00\xD3\xC2\xB1\x13\x4F\x4C\xAD\x61\xCA\xE5\xBA\xF3\x2C\xA3\x16\x12\x35\x16\xD2\xCA\x3B\x7F\x3C\xA2\x1B\x35\xD8\xCA\x67\xE3\xCA\x48\x0D\x3C\x2A\x11\x56\xF4\xCA\xF3\xD1\xCA\x56\x7E\xBD\xBF`


## Development

A small C++ library should be developed to support this format. It could also be implemented in JavaScript and Python if it proves useful for compressed transmission in other circumstances.

Concatenation of strings is not easy (the current row/table selection must be known for the beginning and the end of the string): we recommend to store the data as regular strings and to encode/decode them in one block when transmitting.

# Other existing formats :

A performance comparison (including parameterization effort) should be done with existing formats like:
* ABCL, Binary encoded structure definition language: https://docs.allthingstalk.com/dl/AllThingsTalk_Binary_Conversion_Language_1_0_0.pdf
* BER from ASN.1
* BSON: http://bsonspec.org/spec.html
* CayenneLPP which is used by LoRaWAN and supported by existing libraries ( https://mydevices.com/cayenne/docs/lora/#lora-cayenne-low-power-payload , https://github.com/myDevicesIoT/cayenne-docs/blob/master/docs/LORA.md
* CBOR: https://tools.ietf.org/html/rfc7049
* HYBRID: http://henrich.poehls.com/papers/2017_PoehlsPetschkuhn_IoT_signature_encoding_CAMAD.pdf
* JSON-B: http://mathmesh.com/Documents/draft-hallambaker-jsonbcd.html
* MessagePack: https://msgpack.org/
* NDN TLV (Type-Length-Value) encoding: https://named-data.net/doc/NDN-packet-spec/0.3/tlv.html
* RAKE: https://www.eurasip.org/Proceedings/Eusipco/Eusipco2017/papers/1570344021.pdf
* SenseML: https://tools.ietf.org/html/rfc8428
* Smile, Jackson Efficient JSON-compatible binary format: https://github.com/FasterXML/smile-format-specification/blob/master/smile-specification.md
* UBJSON is a TLV (Type-Length-Value) encoding and wants to replace BSON, BJSON, etc.

N.B. The format we propose is not a TLV as it remains "chunk" based and not binary.

# Possible application examples
OBSS: https://www.sciencedirect.com/science/article/pii/S0360132316300476
