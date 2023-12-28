#include"../headFiles/advanced.h"
ADBackuper::ADBackuper(/* args */)
{
}
ADBackuper::~ADBackuper()
{
}

//备份软链接文件,sourcefile为源文件名，targetfile为目标文件名
bool ADBackuper::backupLINK(string sourcefile,string targetfile)
{
    char buf[BUFFER_SIZE];
    struct timespec times[2];
    struct stat statbuf;
    if(stat(sourcefile.c_str(),&statbuf)!=0){
        perror("link");
        return -1;
    }
    readlink(sourcefile.c_str(), buf, sizeof(buf));//读取源链接
    if(symlink(buf , targetfile.c_str())<0)//复制源链接
    {
        perror("link");
        return false;
    }
    //保留元数据
    times[0]=statbuf.st_atim;
    times[1]=statbuf.st_mtim;
    chown(targetfile.c_str(),statbuf.st_uid,statbuf.st_gid);
    utimensat(AT_FDCWD,targetfile.c_str(),times,AT_SYMLINK_NOFOLLOW);
   
    return true;
}

//备份管道文件，sourcefile为源文件名，targetfile为目标文件名
bool ADBackuper::backupFIFO(string sourcefile,string targetfile)
{
    struct stat statbuf;
    struct timespec times[2];
    if(stat(sourcefile.c_str(),&statbuf)) {
        perror("pipe");
        return false;
    }
    if(mkfifo(targetfile.c_str(),statbuf.st_mode)!=0)
    {
        perror("pipe");
        return false;
    }
    //保留元数据
    times[0]=statbuf.st_atim;
    times[1]=statbuf.st_mtim;
    chown(targetfile.c_str(),statbuf.st_uid,statbuf.st_gid);
    utimensat(AT_FDCWD,targetfile.c_str(),times,AT_SYMLINK_NOFOLLOW);
   
    return true;
}

//兼容递归备份目录，sourceDir为源目录，targetDir为目标父目录
bool ADBackuper::ADBackupDir(string sourceDir,string targetDir)
{
    DIR *srcDir;
	struct dirent *srcDirEntry;
	struct stat srcBuff;
	string srcPath=sourceDir;
	string tgtPath=targetDir;
	string filename;
	char srcpath[PATH_MAX],tgtpath[PATH_MAX];
	struct timespec times[2]; //存储源目录的时间信息
	strcpy(srcpath,srcPath.c_str());
	//在目标目录下创建同名子目录，并转换到同名子目录下，此时srcPath与tgtPath同级
	tgtPath=targetDir+'/'+basename(srcpath);
	//判断目标父目录下是否已有重目录，若有则须重命名
    strcpy(tgtpath,sourceDir.c_str());
    while(access(tgtPath.c_str(),F_OK)==0)
    {
        cout<<"当前目标目录"<<targetDir<<"下已有同名文件或目录，请为备份后的目录重命名\n";
        getline(cin,tgtPath);
        tgtPath=targetDir+'/'+tgtPath; 
    }
	mkDir(tgtPath,srcBuff.st_mode);
	if ((srcDir = opendir(sourceDir.c_str())) == NULL) 
	{
		//打开目录失败，返回失败信息
		fprintf(stderr, "无法打开源目录，请检查后重试 %s\n", targetDir.c_str());
		return false;
	}
	while ((srcDirEntry = readdir(srcDir)) != NULL) 
	{
		filename=srcPath+'/'+srcDirEntry->d_name;
		lstat(filename.c_str(),&srcBuff);
		if (strcmp(srcDirEntry->d_name, ".") == 0 || strcmp(srcDirEntry->d_name, "..") == 0)
			continue;
		if(S_ISDIR(srcBuff.st_mode))
		{
			if(!ADBackupDir(filename,tgtPath))
			{
				cout<<"递归复制失败"<<endl;
				return false;
			}
		}
		else if(S_ISREG(srcBuff.st_mode))
		{
			if(!backupRegFile(filename,tgtPath+'/'+srcDirEntry->d_name))
			{
				cout<<"复制文件失败"<<endl;
				return false;
			}
		}
        else if(S_ISFIFO(srcBuff.st_mode))
        {
            if(!backupFIFO(filename,tgtPath+'/'+srcDirEntry->d_name))
			{
				cout<<"复制文件失败"<<endl;
				return false;
			}
        }
        else if(S_ISLNK(srcBuff.st_mode))
        {
            if(!backupLINK(filename,tgtPath+'/'+srcDirEntry->d_name))
			{
				cout<<"复制文件失败"<<endl;
				return false;
			}
        }
		else
		{
			cout<<"该文件不是本系统支持的文件类型，不予复制\n";
		}
	}
	closedir(srcDir);
	//修该备份后的目录文件元数据
	stat(sourceDir.c_str(),&srcBuff);
	times[0]=srcBuff.st_atim;
	times[1]=srcBuff.st_mtim;
	chmod(tgtPath.c_str(),srcBuff.st_mode);
	chown(tgtPath.c_str(),srcBuff.st_uid,srcBuff.st_gid);
	utimensat(AT_FDCWD,tgtPath.c_str(),times,0);
	return true;
}

//兼容性备份，sourcefile为源文件或目录，targetfile为目标父目录
bool ADBackuper::AllFile(string sourcefile,string targetdir)
{
    struct stat srcbuf;
    char path[PATH_MAX];
    strcpy(path,sourcefile.c_str());
    string targetfilename=targetdir+'/'+basename(path);
    stat(sourcefile.c_str(),&srcbuf);
    if(S_ISDIR(srcbuf.st_mode))
    {
        if(ADBackupDir(sourcefile,targetdir))
        {
            cout<<sourcefile<<"备份成功！\n";
            return true;
        }
        return false;
    }
    else if(S_ISREG(srcbuf.st_mode))
    {
        if(backupRegFile(sourcefile,targetfilename))
        {
            cout<<sourcefile<<"备份成功！\n";
            return true;
        }
        return false;
    }
    else if (S_ISLNK(srcbuf.st_mode))
    {
        if(backupLINK(sourcefile,targetfilename))
        {
            cout<<sourcefile<<"备份成功！\n";
            return true;
        }
        return false;
    }
    else if (S_ISFIFO(srcbuf.st_mode))
    {
        if(backupFIFO(sourcefile,targetfilename))
        {
            cout<<sourcefile<<"备份成功！\n";
            return true;
        }
        return false;
    }
    else
    {
        cout<<"非常抱歉！当前系统不支持该文件类型\n";
        return false;
    }
}

//支持特殊文件类型的文件或目录移动函数
bool ADBackuper::ADmoveFileOrDir(string source,string target)
{
    if(AllFile(source,target))
    {
        cout<<"备份成功!\n";
        if(rmDirOrFile(source))
        {
            cout<<"移动成功\n";
            return true;
        }
        return false;
    }
    else 
    {
        cout<<"移动失败！\n";
        return false;
    }
}
