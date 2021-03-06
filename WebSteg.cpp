#include "provided.h"
#include "http.h"
#include <string>
using namespace std;

bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
    string page;
    if (HTTP().get(url, page))
        if (Steg::hide(page, msg, host))
            return true;
    
    return false;
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
    string page;
    if (HTTP().get(url, page))
        if (Steg::reveal(page, msg))
            return true;
    return false;
}
