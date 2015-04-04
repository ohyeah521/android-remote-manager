#pragma once

#include <vector>
#include <string.h>

class DataPack
{
public:
    /**
     * @brief sendDataPack
     * @param s
     * @param data
     * @return
     */
    long sendDataPack(SOCKET s, vector<char>& data, long* pSent = NULL)
    {
        send(s,SIGNATURE, sizeof(SIGNATURE), 0);
        long length = data.size();
        if(!isBigEndian())
        {
            length = endianConvert32(length);
        }
        send(s, SIGNATURE, sizeof(SIGNATURE), 0);
        send(s, &length, sizeof(length), 0);
        long sent;
        if(pSent == NULL)
        {
            pSent = &sent;
        }
        (*pSent) = 0;
        while( (*pSent) < data.size())
        {
            long send_len = send(s, data.data() + pSent, data.size() - (*pSent), 0);
            if(send_len <=0 )
            {
                break;
            }
            (*pSent) += send_len;
        }
        return (*pSent);
    }

    /**
     * @brief receiveDataPack
     * @param s
     * @return
     */
    vector<char> receiveDataPack(SOCKET s, long* pReceived = NULL, long* pTotalReceived = NULL)
    {
        vector<char> data;
        char signature[sizeof(SIGNATURE)]= {0};
        recv(s, signature, sizeof(signature), 0);
        if( memcmp(signature, SIGNATURE, sizeof(SIGNATURE)) !=0 )
        {
            return data;
        }
        long length= 0;
        recv(s, &length, sizeof(length), 0);
        if(!isBigEndian())
        {
            length = endianConvert32(length);
        }
        if(length <=0 )
        {
            return data;
        }
        data.resize(length);
        long received;
        if( pReceived ==NULL )
        {
            pReceived = &received;
        }
        (*pReceived) = 0;
        if( pTotalReceived != NULL )
        {
            (*pTotalReceived) = length;
        }
        while( (*pReceived) < length)
        {
            long receive_len = recv(s, data.data() + (*pReceived), length - (*pReceived), 0);
            if(receive_len <=0 )
            {
                break;
            }
            (*pReceived) += receive_len;
        }
        return data;
    }
private:
    bool isBigEndian()
    {
        unsigned short sign = 0XEEFF;
        return (*(char*)&sign) == 0XEE;
    }
    long endianConvert32(long n)
    {
        unsigned char *pn = (unsigned char*)&n;
        pn[0]^=pn[3]^=pn[0]^=pn[3];
        pn[1]^=pn[2]^=pn[1]^=pn[2];
        return n;
    }
private:
    static char SIGNATURE[] = {0XEE, 0XFF};
    DataPack(){}
};
