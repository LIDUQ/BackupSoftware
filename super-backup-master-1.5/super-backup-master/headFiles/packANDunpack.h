#ifndef PACKUNPACK
#define PACKUNPACK

#include"advanced.h"


#define BLOCKSIZE 512
typedef struct{
        char    name[100];
        char    mode[32];
        char    uid[8];
        char    gid[8];
        char    size[12];
        char    mtime_sec[16];
        char    mtime_nsec[16];
        char    typeflag;
        char    linkname[100];
        char    fileflag;
        char    padding[218];
} headblock;


class pack_worker:public ADBackuper
{

public:
    pack_worker();
    ~pack_worker();
    long long int getDirSize(string sourcedir);             //获取目录大小，便于计入头结点
    bool packDir(string sourcedir,string targetbag);        //打包接口函数
    bool addFileToBag(string filename,int bag);             //递归向打包文件写入文件
    headblock* genHeader(string sourcefile);                //在每次添加文件前为文件生成头结点

    bool unpackBag(string sourcebag,string targetdir);      //解包函数

    bool turnBagToFile(int sourcebag,string targetdir);     //递归拆包 

};

#endif