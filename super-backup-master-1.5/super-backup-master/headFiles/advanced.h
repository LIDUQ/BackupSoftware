#ifndef ADVANCED
#define ADVANCED

#include"primary.h"

class ADBackuper: public primaryBackuper
{

public:
    ADBackuper();
    ~ADBackuper();
   
    bool AllFile(string sourceFile,string targetFile);//整合备份四种文件，管道，链接，普通，目录
    bool backupLINK(string sourcefile,string targetfile);//备份软链接
    bool backupFIFO(string sourcefile,string targetfile);//备份管道
    bool ADBackupDir(string sourceDir,string targetDir);//包含四种文件类型的目录备份功能
    bool ADmoveFileOrDir(string source,string target);//兼容性移动文件或目录
};
#endif