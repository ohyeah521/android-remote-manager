#include "Tea.h"

Tea::Tea(const vector<char>& key):key(key){}

void Tea::encrypt(vector<char>& data)
{
  if(data.size() == 0 ) return;
  char c = (char)data.size() % 8;
  if(c !=0)
  {
    data.resize( data.size() + 8 - c + 1 );
    data.at( data.size() - 1 ) = c;
  }
  else
  {
    data.resize( data.size() + 1 );
    data.at( data.size() - 1 ) = 0;
  }
  for(int i = 0; i < data.size() - 1; i+=8)
  {
    encrypt8bit((uint32_t*)data.data()+i, (uint32_t*)key.data());
  }
}

void Tea::decrypt(vector<char>& data)
{
  if(data.size() == 0 ) return;
  char c = data.at( data.size() - 1 );
  if( c != 0 )
  {
    c = 8 - c;
  }
  for(int i = 0; i < data.size() - 1; i+=8)
  {
    decrypt8bit((uint32_t*)data.data()+i, (uint32_t*)key.data());
  }
  data.resize( data.size() - 1 - c );
}

void Tea::encrypt8bit(uint32_t*v,uint32_t*k) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0, i; /*setup*/
    uint32_t delta = 0x9e3779b9; /*akeyscheduleconstant*/
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3]; /*cachekey*/
    for(i=0;i<32;i++) {
        /*basiccyclestart*/
        sum += delta;
        v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
    } /*endcycle*/
    v[0] = v0;
    v[1] = v1;
}

void Tea::decrypt8bit(uint32_t*v,uint32_t*k) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720, i; /*setup*/
    uint32_t delta = 0x9e3779b9; /*akeyscheduleconstant*/
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3]; /*cachekey*/
    for(i = 0; i < 32; i++) {
        /*basiccyclestart*/
        v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        sum -= delta;
    } /*endcycle*/
    v[0] = v0;
    v[1] = v1;
}
