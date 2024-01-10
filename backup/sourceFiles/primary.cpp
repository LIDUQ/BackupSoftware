#include"../headFiles/primary.h"

//初级备份器的构造函数
primaryBackuper::primaryBackuper()
{
}
//初级备份器的析构函数
primaryBackuper::~primaryBackuper()
{
}
//打印当前目录树结构
bool primaryBackuper::printDir(string dir, int depth)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	string dirname;
	//打开目录并返回存储目录信息的DIR结构体
	if ((dp = opendir(dir.c_str())) == NULL) {
		fprintf(stderr, "Can`t open directory %s\n", dir.c_str());
		return false;
	}
	//通过先前的DIR目录结构体读取当前目录的目录文件
	//返回dirent结构体（entry）保存目录文件的信息
	while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )  
				continue;	
		dirname=dir+'/'+entry->d_name;
		lstat(dirname.c_str(), &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {
			printf("%*s%s/\n", depth, "", entry->d_name);
			printDir(dirname, depth+4);
		} else
			printf("%*s%s\n", depth, "", entry->d_name);
	}
	closedir(dp);	
    return true;
}

//目录判空函数，便于删除和移动目录
bool primaryBackuper::EmptyDir(string dirName)
{
	DIR *dp;
	struct dirent *entry;
	struct stat *buff;
	int num=0;
	if ((dp = opendir(dirName.c_str())) == NULL) {
		fprintf(stderr, "Can`t open directory %s\n", dirName.c_str());
		return 0;
	}
    while( (entry=readdir(dp))!=NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || 
			strcmp(entry->d_name, "..") == 0 )  
			continue;	
        num++;
    }
	if(num==0)	return true;
	else return false;
}
//创建新目录函数，便于后续备份目录
bool primaryBackuper::mkDir(string dirPathName,mode_t mode)
{  
	if(dirPathName=="."||dirPathName=="/")
	{
		return true;
	}
	if(access(dirPathName.c_str(),F_OK) == 0)
	{
		return true;
	}
	else
	{
		recurMakeDir(dirPathName);
		return true;
	}
	return false;

}
//递归创建新目录，若父目录不存在，从最低级目录递归创建
bool primaryBackuper::recurMakeDir(string dirPath,mode_t mode)
{
	char path[PATH_MAX];
	strcpy(path,dirPath.c_str());
	if(dirPath=="."||dirPath=="/")
	{
		return true;
	}
	if(access(dirPath.c_str(),F_OK)==0)
	{
		return true;
	}
	else
	{
		recurMakeDir(dirname(path));
		mkdir(dirPath.c_str(),mode);
		return true;
	}
}
//删除目录函数，执行删除目录或者文件(兼容)，若目录非空会询问是否删除目录内文件
bool primaryBackuper::rmDirOrFile(string tgtName)
{
	DIR *tgtDP;
	struct dirent *tgtEntry;
	struct stat tgtBuff;
	string filename=tgtName;
	lstat(tgtName.c_str(),&tgtBuff);
	//读取tgtName文件的具体信息，若是空目录或普通文件直接删除，若是空目录递归删除
	if(S_ISDIR(tgtBuff.st_mode))
	{
		if(((tgtDP=opendir(tgtName.c_str()))==NULL))
		{
			//打开目录失败，返回失败信息
			fprintf(stderr, "无法打开目标目录，请检查后重试 %s\n", tgtName.c_str());
			return false;
		}
		//若目录为空，直接删除，否则进入对目录内文件删除的确认
		else if(EmptyDir(filename))
		{
			rmdir(filename.c_str());
			closedir(tgtDP);
			return true;
		}
		//目录非空，开始递归删除目录内文件或子目录
		else
		{
			while((tgtEntry=readdir(tgtDP))!=NULL)
			{
				if(strcmp(tgtEntry->d_name,".")==0||strcmp(tgtEntry->d_name,"..")==0)
				continue;
				//保存递归路径，便于删除
				filename=tgtName+'/'+tgtEntry->d_name;
				lstat(filename.c_str(),&tgtBuff);
				//若是子目录，进入递归删除
				if(S_ISDIR(tgtBuff.st_mode))
				{
					rmDirOrFile(filename);
				}
				//若是目录下普通文件，确认是否删除
				else if(S_ISFIFO(tgtBuff.st_mode)||S_ISLNK(tgtBuff.st_mode)||S_ISREG(tgtBuff.st_mode))
				{
					char flag;
					cout<<"确定删除当前文件？\n"
					<<filename<<endl
					<<"输入y/Y删除,任意其他键跳过删除\n";
					cin>>flag;
					if(flag=='y'||flag=='Y')
					{
						remove(filename.c_str());
					}
				}
				else
				{
					std::cout<<"抱歉，当前文件不是本功能支持的文件类型，不予删除\n";
				}
			}
			closedir(tgtDP);
		}
		if(EmptyDir(tgtName))
		{
			rmdir(tgtName.c_str());
			return true;
		}
		else
		{
			cout<<"当前目录"<<tgtName<<"非空，无法删除！\n";
			return false;
		}
	}
	else if(S_ISFIFO(tgtBuff.st_mode)||S_ISLNK(tgtBuff.st_mode)||S_ISREG(tgtBuff.st_mode))
	{
		char flag;
		cout<<"确定删除当前文件？\n"
		<<filename<<endl
		<<"输入y/Y删除,任意其他键跳过删除\n";
		cin>>flag;
		if(flag=='y'||flag=='Y')
		{
			remove(filename.c_str());
			return true;
		}
		return false;
	}
	return true;
}


//备份器的普通文件复制函数实现,sourcefile为源文件名，targetfile为目标文件名
bool primaryBackuper::backupRegFile(string sourceFile,string targetFile){
    
    int fin,fout;			//文件描述符
	void *src,*dst;			//无类型指针，便于后续使用
	size_t	copysz;			//复制的长度，限定在1GB
	struct	stat sbuf;		//存储源文件具体信息的结构体
	off_t	fsz=0;			//复制的初始位置(文件大小)
	struct timespec times[2]; //存储源文件的时间信息

	//若以只读方式打开源文件失败则返回	
	if((fin=open(sourceFile.c_str(),O_RDONLY))<0)	
	{
		return false;
	}
    //若创建目标文件失败直接返回，0644为初始权限,创建文件支持携带路径
	//注意此处采用O_TRUNC，则已有文件时直接覆盖
	if((fout=open(targetFile.c_str(),O_RDWR|O_CREAT|O_TRUNC,0644))<0)
	{
		//退出要关闭文件描述符
		close(fin);
		return false;
	}
	//若获取源文件状态信息失败，关闭文件并返回
	if(fstat(fin,&sbuf)<0)
	{
		close(fin);
		close(fout);
		return false;
	}
	//若无法给目标文件分配与源文件同样大小的空间则关闭文件并返回
	if(ftruncate(fout,sbuf.st_size)<0)
	{
		close(fin);
		close(fout);
		return false;
	}
    //通过内存映射的方式实现文件复制
    while(fsz<sbuf.st_size)
	{
		//当复制的大小大于1GB时，限定映射内存1GB
		if((sbuf.st_size-fsz)>COPYINCR)
			copysz=COPYINCR;
		else
			copysz=sbuf.st_size-fsz;
		if((src=mmap(0,copysz,PROT_READ,MAP_SHARED,fin,fsz))==MAP_FAILED)
		{
			close(fin);
			close(fout);
			return false;
		}
    if((dst=mmap(0,copysz,PROT_READ|PROT_WRITE,MAP_SHARED,fout,fsz))==MAP_FAILED)
		{
			close(fin);
			close(fout);
			return false;
		}
		memcpy(dst,src,copysz);     //完成内存映射后直接复制
		munmap(src, copysz);        //复制完成，撤销内存映射
		munmap(dst, copysz); 
		
		//修该备份后的元数据
		times[0]=sbuf.st_atim;
		times[1]=sbuf.st_mtim;
		fchmod(fout, sbuf.st_mode);
    	chown(targetFile.c_str(),sbuf.st_uid,sbuf.st_gid);
    	futimens(fout,times);
		fsz+=copysz;
	}
	//退出要关闭文件描述符
	close(fin);
	close(fout);
	return true;
}
//递归复制目录函数，便于备份目录时调用
//此处sourceDir相当于子目录，targetDir相当于父目录
bool primaryBackuper::backupDir(string sourceDir,string targetDir)
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
			cout<<filename<<endl;
			cout<<tgtPath<<endl;
			if(!backupDir(filename,tgtPath))
			{
				cout<<"递归复制失败"<<endl;
				return false;
			}
		}
		else if(S_ISFIFO(srcBuff.st_mode)||S_ISLNK(srcBuff.st_mode)||S_ISREG(srcBuff.st_mode))
		{
			if(!backupRegFile(filename,tgtPath+'/'+srcDirEntry->d_name))
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