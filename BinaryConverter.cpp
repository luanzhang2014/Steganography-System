#include "provided.h"
#include <string>
#include <vector>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;

string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
    string bitString="";
    for (int i=0; i<numbers.size(); i++) {        //unsigned short number -> binary string
        bitString+=convertNumberToBitString(numbers[i]);
    }

    for (int i=0; i<bitString.size(); i++) {
        if (bitString[i]=='1')
            bitString[i]='\t';                    // 1 -> '\t'
        
        else
            bitString[i]=' ';                     //0 -> ' '
        
    }
    return bitString;
    
}

bool BinaryConverter::decode(const string& bitString,
                             vector<unsigned short>& numbers)
{
    //check if the bitstring has proper length
    if (bitString.size()%16!=0)
        return false;
    
    if (numbers.size()>0)
        numbers.clear();
    
    unsigned short number;
    //for every 16 bits, replace '\t' with 1 and ' ' with 0, convert it to unsigned short number
    for (int i=0; i<bitString.size(); i+=16)
    {
        string stringToConvert="";
        for (int j=i; j<i+16; j++)
        {
            if (bitString[j]=='\t')
                stringToConvert+='1';
            else if (bitString[j]==' ')
                stringToConvert+='0';
            else
                return false;
        }
        if (convertBitStringToNumber(stringToConvert, number))
            numbers.push_back(number);
        else
            return false;
    }
    return true;
}

string convertNumberToBitString(unsigned short number)
{
    string result(BITS_PER_UNSIGNED_SHORT, '0');
    for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
    {
        k--;
        if (number % 2 == 1)
            result[k] = '1';
    }
    return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
    if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
        return false;
    number = 0;
    for (size_t k = 0; k < bitString.size(); k++)
    {
        number *= 2;
        if (bitString[k] == '1')
            number++;
        else if (bitString[k] != '0')
            return false;
    }
    return true;
}
