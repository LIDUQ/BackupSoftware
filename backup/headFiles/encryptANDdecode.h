#ifndef ENCRYPTANDDECODE
#define ENCRYPTANDDECODE

#include"advanced.h"
#include <vector>
#include <cstdlib>
#include <bitset>

#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476
//md5所用常量数据
extern const char str16[];

extern const unsigned int T[];

extern const unsigned int s[] ;

/*
该类实现了对文件的加密和解密功能，其中使用了MD5算法进行密码处理，然后再使用AES算法进行加密和解密。
*/
class MD5
{
private:
     unsigned int tempA, tempB, tempC, tempD, strlength;
public:
    MD5();
    ~MD5();
    unsigned int F(unsigned int b, unsigned int c, unsigned int d);
    // G函数
    unsigned int G(unsigned int b, unsigned int c, unsigned int d);
    // H函数
    unsigned int H(unsigned int b, unsigned int c, unsigned int d);
    // I函数
    unsigned int I(unsigned int b, unsigned int c, unsigned int d);
    // 移位操作函数
    unsigned int shift(unsigned int a, unsigned int n);
    // 编码函数
    string encode(string src);
    // 循环压缩
    void iterateFunc(unsigned int* X, int size = 16);
    // 填充字符串
    vector<unsigned int> padding(string src);
    // 整理输出
    string format(unsigned int num);

};
//aes所用数据
typedef bitset<8> byte;
typedef bitset<32> word;
 
const int Nr =10;  // AES-128需要 10 轮加密
const int Nk=4;   // Nk 表示输入密钥的 word 个数
 
extern const byte S_Box[16][16];
 
extern const byte Inv_S_Box[16][16];
 
// 轮常数，密钥扩展中用到。（AES-128只需要10轮）
extern const word Rcon[10] ;
class AES
{
public:
    AES(/* args */);
    ~AES();
    void charToByte(byte out[16], const char s[16]);//char字符转换为二进制存入byte
    void divideToByte(byte out[16], bitset<128>& data);//连续128存入一个数组
    bitset<128> mergeByte(byte in[16]);//16个byte合成128位
    void SubBytes(byte mtx[4*4]);//S盒变换函数
    void ShiftRows(byte mtx[4*4]);//行变换函数
    byte GFMul(byte a, byte b);//有限域上的乘法
    void MixColumns(byte mtx[4*4]);//列变换
    void AddRoundKey(byte mtx[4*4], word k[4]);//轮密钥加变换
    void InvSubBytes(byte mtx[4*4]);//逆黑盒变换
    void InvShiftRows(byte mtx[4*4]);//逆行变换
    void InvMixColumns(byte mtx[4*4]);//逆列变换
    word Word(byte& k1, byte& k2, byte& k3, byte& k4);//4个byte转换为一个Word
    word RotWord(word& rw);//按字节循环
    word SubWord(word& sw);//对Word中的每个字节进行S盒变换
    void KeyExpansion(byte key[4*Nk], word w[4*(Nr+1)]);//密钥扩展函数
    void encrypt(byte in[4*4], word w[4*(Nr+1)]);//加密
    void decrypt(byte in[4*4], word w[4*(Nr+1)]);//解密
};

class encrypt_worker:   public  ADBackuper,public MD5,public AES   
{  

public:
    encrypt_worker(/* args */);
    ~encrypt_worker();
    string getPSWD();//密码输入用星号遮掩
    bool encyptFile(string md5ofpwd,string md5md5ofpwd,string sourcefile,string targetdir);//对sourcefile进行aes加密，targetdir为加密文件存放目录
    bool decryptFile(string md5ofpwd,string md5md5ofpwd,string sourcefile,string targetdir);//对sourcefile进行aes解密，targetdir为解密文件存放目录
};


#endif