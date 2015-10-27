#include "provided.h"
#include <string>
#include <vector>
#include "substituteHashTable.h"
#include <algorithm>
using namespace std;

void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
    //calculate capacity and bucket number
    unsigned int capacity=((unsigned int)s.size())/2+512;
    unsigned int maximun=16384;
    unsigned int C=min(capacity, maximun);
    unsigned int B=2*C;
    
    //initiate hash table and insert permant association
    HashTable<string, unsigned short> stringToShort(B,C);
    for (int j=0; j<256; j++) {
        string s(1,static_cast<char>(j));
        stringToShort.set(s, j, true);
    }
    
    //LZW compression
    unsigned short ID=256;
    string cur="";
    vector<unsigned short> v;
    unsigned short x;
    
    for (int i=0; i<s.size(); i++) {
        string expanded=cur;
        expanded+=s[i];
        if (stringToShort.get(expanded, x))         //expanded already in the hash table
        {
            cur=expanded;
        }
        else                                          //expanded not in, process
        {
            stringToShort.get(cur, x);
            v.push_back(x);
            stringToShort.touch(cur);
            cur="";
            string c(1,s[i]);
            stringToShort.get(c, x);
            v.push_back(x);
            if (!stringToShort.isFull())             //add expanded association if hash table not full
            {
                stringToShort.set(expanded, ID);
                ID++;
            }
            else                                    //if full, diacard one item, them add new
            {
                string key;
                stringToShort.discard(key, x);
                stringToShort.set(expanded, x);
                
            }
        }
    }
    
    if (cur.size()) {
        stringToShort.get(cur, x);
        v.push_back(x);
    }
    v.push_back(C);
    numbers=v;
}

bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
    if (numbers.size()==0)
    {
        return false;
    }
    unsigned int C=numbers.back();
    unsigned int B=2*C;
    
    //initiate hash table and insert permant association
    HashTable<unsigned short, string> shortToString(B,C);
    for (unsigned short j=0; j<256; j++) {
        string s(1,static_cast<char>(j));
        shortToString.set(j, s, true);
    }
    //LZW decompression
    unsigned short ID=256;
    string cur="";
    string output="";
    string x;
    for (int i=0; i<numbers.size()-1; i++) {
        unsigned short us=numbers[i];
        if (us<=255)
        {
            shortToString.get(us, x);
            output+=x;
            if (cur=="")
            {
                cur+=x;
            }
            else
            {
                string expanded=cur;
                expanded+=x;
                if (!shortToString.isFull())
                {
                    shortToString.set(ID, expanded);
                    ID++;
                }
                else
                {
                    unsigned short key;
                    shortToString.discard(key, x);
                    shortToString.set(key, expanded);
                }
                cur="";
            }
        }
        else
        {
            if (!shortToString.get(us, x))
            {
                return false;
            }
            else
            {
                shortToString.touch(us);
                output+=x;
                cur=x;
                
            }
        }
    }
    s=output;
    return true;
    
}



unsigned int computeHash(int key)
{
    return key;
}


unsigned int computeHash(string key)
{
    unsigned int total=0;
    for(int i=0;i<key.length();i++)
    {
        total=total+key[i]*(i+1);
    }
    return total;
}