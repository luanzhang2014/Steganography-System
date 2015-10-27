#include "provided.h"
#include <string>
#include <iostream>
using namespace std;

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut)
{
    //check if hostIn is empty
    if (hostIn=="") {
        return false;
    }
    
    vector<unsigned short> v;
    Compressor::compress(msg, v);        //compress
    string s=BinaryConverter::encode(v);        //encode
    
    //split the hostIn lines by '\n' and remove the ' ' and '\t' at the end of each line
    vector<string> lines;
    string line = hostIn;
    int cur = 0;
    int start = 0;    //mark the start of each new line
    for (int i=0; i<hostIn.size(); i++)
    {
        if (hostIn[i]=='\n')
        {
            cur--;
            while ((line[cur]==' ' || line[cur]== '\t') && cur>0 )
            {
                cur--;
            }
            lines.push_back(line.substr(start,(cur-start+1)));
            start = ++cur;
        }
        else if (hostIn[i]=='\r')       //ignore '\r'
        {
            ;
        }
        else
        {
            line[cur++] = hostIn[i];
        }
    }
    if (start != cur)   //In case there is no '\n' at the end of the hostIn
    {
        cur--;
        while ((line[cur]==' ' || line[cur]== '\t') && cur>=start )
        {
            cur--;
        }
        if(cur >= start){
            lines.push_back(line.substr(start,(cur-start+1)));
        }
    }
    
    vector<string> subLines;
    int L=s.size();
    int N=lines.size();
    
    //divide the encoded string.
    //the first L%N subline should have one more char than the left lines
    for (int i=0; i<L%N; i++) {
        string subLine="";
        for (int j=i; j<i+L/N+1; j++) {
            subLine+=s[j];
        }
        lines[i] += subLine;
    }
    for(int i=L%N; i<N; i++)
    {
        string subLine="";
        if (L>N)
        {
            for (int j=i*(L/N); j<(i+1)*(L/N); j++)
            {
                subLine+=s[L%N+j];
            }
        }
        lines[i] += subLine;
    }
    
    //add sublines to corresponding lines and combine complete lines together
    hostOut="";
    for (int i=0; i<lines.size(); i++) {
        hostOut+=lines[i];
        hostOut+='\n';
    }
    
    return true;
}

bool Steg::reveal(const string& hostIn, string& msg)
{
    vector<string> lines;
    string line="";
    //split the hostIn into several lines according to '\n'
    for (int i=0; i<hostIn.size(); i++)
    {
        if (hostIn[i]=='\n')
        {
            //gather only the ' ' and '\t' at the end of a line
            int j;
            for (j=i-1; hostIn[j]=='\t' || hostIn[j]==' '; j--)
            {
                ;
            }
            for (int k=j+1; k<i; k++)
            {
                line+=hostIn[k];
            }
            lines.push_back(line);
            line="";
        }
        
    }
    //put the lines together
    string s="";
    for (int i=0; i<lines.size(); i++) {
        s+=lines[i];
    }
    //decode the string
    vector<unsigned short> v;
    if (!BinaryConverter::decode(s, v)) {
        return false;
    }
    //decompress the message
    if (!Compressor::decompress(v, msg)) {
        return false;
    }
    return true;
    
    
}
