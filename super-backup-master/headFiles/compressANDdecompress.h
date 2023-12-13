#ifndef COMPRESSANDDECOMPRESS
#define COMPRESSANDDECOMPRESS

#include"advanced.h"
#include <fstream>
#include <queue>
#include <map>
#include <vector>
#include <bitset>

class compress_worker:public ADBackuper
{

    map<unsigned char, string> codeMap;
    struct haffNode{
        unsigned long long freq;//待编码字符频率
        unsigned char uchar;//待编码字符
        string code;//编码后的二进制串
        struct haffNode* left = 0;//哈夫曼节点左孩子
        struct haffNode* right = 0;//哈夫曼节店右孩子
    };
    struct Compare{
        bool operator ()(const haffNode* a, const haffNode* b) {
            return a->freq > b->freq;
        }};
    void encode(haffNode* pn, string code);//压缩时的编码函数
    void creat_tree(haffNode* father, unsigned char uchar, string code);//解压时构建哈夫曼树
public:
    compress_worker(/* args */);
    ~compress_worker();
    //压缩函数，sourcefile为待压缩文件，targetfile为压缩后文件，压缩后文件添加后缀.co
    bool compress(string sourcefile,string targetfile);
    //解压函数，sourcefile为先前已压缩文件，targetfile为解压后文件，解压后文件去掉后缀.co
    bool decompress(string sourcefile,string targetfile);
};

#endif