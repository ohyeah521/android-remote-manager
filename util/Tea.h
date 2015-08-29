#pragma once
#include <stdint.h>
#include <vector>
using std::vector;

class Tea
{
public:
  Tea(const vector<char>& key);
  void encrypt(vector<char>& data);
  void decrypt(vector<char>& data);
private:
  static void encrypt8bit(uint32_t*v,uint32_t*k);
  static void decrypt8bit(uint32_t*v,uint32_t*k);

  vector<char> key;
};
