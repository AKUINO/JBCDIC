#include "JBCDIC.h"

using namespace std;



int main()
{
    char input[] = "[{ts+316123456B+3-71T+21-3H+67-2L+400C2+1134U+FA+N}{TS+316123516B+3-7T+21-35H+67L+480C2+1156U+TA+567}]";
    uint8_t output[4*sizeof(input)];
    int test = JBCDIC::encode_to_jbcdic(input, strlen(input), output, sizeof(output));
    std::cout << "char entrée : " << strlen(input) << std::endl << input << std::endl;
    int i=0;
    while(i < test){
        std::cout << (char)output[i] ;
        i++;
    }

    char test2[4*sizeof(input)];
    test = JBCDIC::decode_from_jbcdic(output, test, test2, sizeof(test2));
    std::cout << std::endl << "char sortie : " << test << std::endl;
    i=0;
    while(i < test){
        std::cout << test2[i];
        i++;
    }
    return 0;
}
