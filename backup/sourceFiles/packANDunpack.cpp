#include"../headFiles/packANDunpack.h"

pack_worker::pack_worker()
{
}
pack_worker::~pack_worker()
{
}
long long int pack_worker::getDirSize(string dirname)
{
	DIR* dp;
	struct dirent *entry;
    struct stat statbuf;
	string subdir;
	long long int totalSize=0;
	if ((dp = opendir(dirname.c_str())) == NULL)
	{
      fprintf(stderr, "Cannot open dir: %s\n", dirname.c_str());
      return -1; //可能是个文件，或者目录不存在
    }   
    //先加上自身目录的大小
	lstat(dirname.c_str(), &statbuf);
    totalSize+=statbuf.st_size;
    while ((entry = readdir(dp)) != NULL)
    {
		if (strcmp(".", entry->d_name) == 0 ||strcmp("..", entry->d_name) == 0)
			continue;
		subdir=dirname+'/'+entry->d_name;
        lstat(subdir.c_str(), &statbuf);
        if (S_ISDIR(statbuf.st_mode))
         {   
            long long int subDirSize = getDirSize(subdir);
            totalSize+=subDirSize;
         }
         else
         {
             totalSize+=statbuf.st_size;
         }
     }
     closedir(dp);
     return totalSize;
}
//对目标目录或文件生成对应头结点
headblock* pack_worker::genHeader(string filename)
{
    int i;
	struct stat srcbuf;
    lstat(filename.c_str(),&srcbuf);
    headblock head;
	headblock* p=&head;
	long mtimesec,mtimensec;
	string mtimesecstring,mtimensecstring;
	char sylinkname[100];
    //存储文件名
    strcpy(head.name,filename.c_str());
	//存储文件st_mode,事实上已经包含用户权限和文件类型
    strcpy(head.mode,(to_string(srcbuf.st_mode)).c_str());
    //存储用户ID
    strcpy(head.uid,(to_string(srcbuf.st_uid)).c_str());
    //存储groupID
    strcpy(head.gid,(to_string(srcbuf.st_gid)).c_str());
	//存储目录或大小
	if(S_ISDIR(srcbuf.st_mode))
		strcpy(head.size,(to_string(getDirSize(filename))).c_str());
	else
		strcpy(head.size,(to_string(srcbuf.st_size)).c_str());
	//存储最近修改时间
	sprintf(head.mtime_sec,"%ld",srcbuf.st_mtim.tv_sec);
	sprintf(head.mtime_nsec,"%ld",srcbuf.st_mtim.tv_nsec);
	//存储文件类型
	if(S_ISREG(srcbuf.st_mode))
        head.typeflag='0';
	if(S_ISLNK(srcbuf.st_mode))
	{
		head.typeflag='1';
		if(readlink(filename.c_str(),sylinkname,srcbuf.st_size)!=-1)
		{
			cout<<"读取链接成功\n";
		}
		else perror("link");
	} 
	if(S_ISFIFO(srcbuf.st_mode))
        head.typeflag='2';
	if(S_ISDIR(srcbuf.st_mode))
        head.typeflag='3';
	//如果是软链接，存放链接路径
	strcpy(head.linkname,sylinkname);
	head.linkname[srcbuf.st_size]='\0';
	//头结点块标记
	head.fileflag='1';
	//头结点末尾填零
	char padd[218]={'\0'};
	stpcpy(head.padding,padd);
    return p;
}

//递归扫描目录时将文件写入bag中,dirname就是当前要打包的目录
bool pack_worker::addFileToBag(string dirname,int bagfile)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	headblock* head;
	cout<<"当前打包"<<dirname<<endl;

	//首先写入当前目录的头结点	
	head=genHeader(dirname);
	write(bagfile,head,BLOCKSIZE);
    int fin,fout=bagfile;			//文件描述符
	string filePath;
	char buffer[BLOCKSIZE];
	char flag[BLOCKSIZE]={'\0'};
	//打开目录并返回存储目录信息的DIR结构体
	if ((dp = opendir(dirname.c_str())) == NULL) {
		fprintf(stderr, "Can`t open directory %s\n", dirname.c_str());
		return false;
	}
	//通过先前的DIR目录结构体读取当前目录的目录文件
	//返回dirent结构体（entry）保存目录文件的信息
	while ((entry = readdir(dp)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || 
				strcmp(entry->d_name, "..") == 0 )  
				continue;	
		filePath=dirname+'/'+entry->d_name;
		lstat(filePath.c_str(), &statbuf);
		//如果是子目录，递归打包
		if (S_ISDIR(statbuf.st_mode)) {
			addFileToBag(filePath,fout);
		} 
		//如果是普通文件，创建头结点并写入文件内容
		else if(S_ISREG(statbuf.st_mode))
		{
			head=genHeader(filePath);
			write(fout,head,BLOCKSIZE);
			fin=open(filePath.c_str(),O_RDONLY);
			//文件比一个块大，才多次读入，否则一次读写即可
			if(statbuf.st_size>BLOCKSIZE){
				for(int i=0;i<(statbuf.st_size/BLOCKSIZE)-1;i++)
				{
					read(fin,buffer,BLOCKSIZE);
					write(fout,buffer,BLOCKSIZE);
				}
				//最后一次读写补零
				read(fin,buffer,statbuf.st_size%BLOCKSIZE);
				write(fout,buffer,statbuf.st_size%BLOCKSIZE);
				write(fout,flag,BLOCKSIZE-statbuf.st_size%BLOCKSIZE);	//末尾补零
			}
			else 
			{
				read(fin,buffer,statbuf.st_size);
				write(fout,buffer,statbuf.st_size);
				write(fout,flag,BLOCKSIZE-statbuf.st_size);
			}
			close(fin);
		}
		//如果是链接文件和管道文件，只保留头结点即可
		else if(S_ISLNK(statbuf.st_mode)||S_ISFIFO(statbuf.st_mode))
		{	
			head=genHeader(filePath);
			write(fout,head,BLOCKSIZE);
		}
		else
		{
			cout<<"抱歉,文件"<<filePath<<"类型暂不支持！\n";
		}
	}
	closedir(dp);
    return true;
}

//打包函数实现，sourcedir为打包源目录，targetbag为打包输出文件，后缀名为bo
bool pack_worker::packDir(string sourcedir,string targetbag)
{
	DIR *dp;
	string dirPath;
	int fout;
	headblock* head;
	char flag[BLOCKSIZE]={'\0'};
	targetbag+=".bo";
	if((fout=open(targetbag.c_str(),O_RDWR|O_CREAT|O_TRUNC|O_APPEND,0644))<0)
	{
		return false;
	}
	//打开目录并返回存储目录信息的DIR结构体
	if ((dp = opendir(sourcedir.c_str())) == NULL) {
		fprintf(stderr, "Can`t open directory %s\n", sourcedir.c_str());
		return false;
	}
	if(addFileToBag(sourcedir,fout))
	{
		write(fout,flag,BLOCKSIZE);
		close(fout);
		cout<<"打包成功！\n";
		return true;
	}
	else
	{
		close(fout);
		cout<<"打包失败!\n";
		return false; 
	}
}

//具体拆包函数，sourcebag为源包的文件描述符，targetdir为解包目标目录
bool pack_worker::turnBagToFile(int sourcebag,string targetdir)
{	
	timespec times[2];
	string filepath;
	int fout;
	char buffer[BLOCKSIZE];
	headblock head[BLOCKSIZE];
	while(read(sourcebag,head,BLOCKSIZE))
	{
		//不是头结点不在此读
		if(head->fileflag!='1')
			continue;
		times[0].tv_sec=time_t(atol(head->mtime_sec));
		times[0].tv_nsec=atol(head->mtime_nsec);
		times[1]=times[0];
		filepath=targetdir+'/'+head->name;	
		//读到普通文件的头结点，生成对应普通文件
		if(head->typeflag=='0')
		{
			if((fout=open(filepath.c_str(),O_RDWR|O_CREAT|O_TRUNC|O_APPEND,0644))<0)
			{
				return false;
			}
			if(atol(head->size)>BLOCKSIZE)
			{
				for(int i=0;i<(atoi(head->size)/BLOCKSIZE)-1;i++)
				{
					read(sourcebag,buffer,BLOCKSIZE);
					write(fout,buffer,BLOCKSIZE);
				}
				//最后一次读写
				read(sourcebag,buffer,atoi(head->size)%BLOCKSIZE);
				write(fout,buffer,atoi(head->size)%BLOCKSIZE);
				read(sourcebag,buffer,BLOCKSIZE-(atoi(head->size)%BLOCKSIZE));
			}
			else
			{
				read(sourcebag,buffer,atoi(head->size));
				write(fout,buffer,atoi(head->size));
				read(sourcebag,buffer,BLOCKSIZE-atoi(head->size));
			}
			close(fout);
		}
		//若是软连接的头文件，直接生成对应头结点的软链接
		else if(head->typeflag=='1')
		{
			if(symlink(head->linkname , filepath.c_str())<0)
			{
				perror("link");
				return false;
			}
		}
		//若是管道文件，生成对应管道文件
		else if(head->typeflag=='2')
		{
			if(mkfifo(filepath.c_str(),atoi(head->mode))<0)
			{
				perror("pipe");
				return false;
			}
		}
		else
		{
			if(mkDir(filepath.c_str(),atoi(head->mode)))
			cout<<"成功创建目录"<<filepath<<endl;
			else cout<<"创建失败\n";
		}
		//对应头结点生成对应文件后，设置文件元数据
		chmod(filepath.c_str(),atoi(head->mode));
		chown(filepath.c_str(),atoi(head->uid),atoi(head->gid));
		utimensat(AT_FDCWD,filepath.c_str(),times,AT_SYMLINK_NOFOLLOW);
	}
	return true;
}
//解包函数，将打包后的.bo文件复原为目录
bool pack_worker::unpackBag(string sourcebag,string targetdir)
{
	//先简单校验是否为.bo文件
	int len;
	int fin;
	len=strlen(sourcebag.c_str());
	if(sourcebag[len-2]!='b'||sourcebag[len-1]!='o')
	{
		cout<<"当前解包文件格式不符，退出解包！\n";
		return false;
	}
	else
	{
		fin=open(sourcebag.c_str(),O_RDONLY);
		if(turnBagToFile(fin,targetdir))
		{
			cout<<"解包成功！\n";
			close(fin);
			return true;
		}	
		else
			cout<<"解包失败！\n";
		close(fin);
		return false;
	}
}