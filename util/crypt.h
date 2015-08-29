#ifndef CRYPT_H
#define CRYPT_H

class crypt
{
public:
    crypt();
    void encrypt(char* buf, int len);
    void decrypt(char* buf, int len);
};

#endif // CRYPT_H
