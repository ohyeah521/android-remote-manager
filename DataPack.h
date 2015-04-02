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
    long sendDataPack(SOCKET s, vector<char>& data)
    {
        send(s,SIGNATURE, sizeof(SIGNATURE), 0);
        long length = data.size();
        if(!isBigEndian())
        {
            length = endianConvert32(length);
        }
        send(s, SIGNATURE, sizeof(SIGNATURE), 0);
        send(s, &length, sizeof(length), 0);
        long sent = 0;
        while( sent < data.size())
        {
            long send_len = send(s, data.data() + sent, data.size() - sent, 0);
            if(send_len <=0 )
            {
                break;
            }
            sent += send_len;
        }
        return sent;
    }

    /**
     * @brief receiveDataPack
     * @param s
     * @return
     */
    vector<char> receiveDataPack(SOCKET s)
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
        long received = 0;
        while( received < length)
        {
            long receive_len = recv(s, data.data() + received, length - received, 0);
            if(receive_len <=0 )
            {
                break;
            }
            received += receive_len;
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
