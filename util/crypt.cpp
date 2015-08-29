#include "crypt.h"

crypt::crypt()
{
}

void crypt::encrypt(char* buf, int len)
{
    for(int i=0;i<len; ++i)
    {
        buf[i] ^= 0X88;
    }
}
void crypt::decrypt(char* buf, int len)
{
    encrypt(buf,len);
}
