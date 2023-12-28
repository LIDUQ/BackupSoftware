#ifndef PRIMARY
#define PRIMARY

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<time.h>
#include<libgen.h>
#include<string.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>

#define BUFFER_SIZE 1024

#define COPYINCR (1024*1024*1024)

using namespace std;


class primaryBackuper
{
public:
    primaryBackuper();
    ~primaryBackuper();
  	
    bool printDir(string workdir,int depth);        //展示当前工目录结构
    bool backupRegFile(string source,string destination);	//备份普通文件，便于moveFile以及backupDir调用
	bool moveFile(string sourceFile,string targetDir);		//移动文件到指定目录，便于backupDir调用
	bool backupDir(string sourceDir,string targetDir);		//复制目录到指定目录下，便于backupDir调用
	
	bool mkDir(string dirPathName,mode_t mode=0755);		//创建新目录，默认权限755，便于backupDir调用
	bool recurMakeDir(string dirPath,mode_t mode_t=0755);	//递归创建新目录，便于mkDir调用
   
	bool moveDir(string sourceDir,string targetDir);		//移动目录到指定目录下
	bool EmptyDir(string dirName);							//对目录判空，便于rmDirOrFile调用
	bool rmDirOrFile(string dirPathName);					//删除目录或者文件
};

#endif 