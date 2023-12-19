#include"../headFiles/advanced.h"
#include"../headFiles/compressANDdecompress.h"
#include"../headFiles/encryptANDdecode.h"
#include"../headFiles/packANDunpack.h"
#include"../headFiles/primary.h"

void print_function()
{
    cout<<"请输入对应数字选择功能："<<endl
    <<"0：切换工作目录"<<endl
    <<"1：备份普通文件"<<endl
    <<"2：备份链接文件"<<endl
    <<"3：备份管道文件"<<endl
    <<"4：备份目录"<<endl
    <<"5：创建空目录"<<endl
    <<"6：备份文件(兼容)"<<endl
    <<"7：移动文件(兼容)"<<endl
    <<"8：删除文件(兼容)"<<endl
    <<"9：目录打包"<<endl
    <<"10：目录解包"<<endl
    <<"11：压缩文件"<<endl
    <<"12：解压文件"<<endl
    <<"13：加密文件(只支持单个文件)"<<endl
    <<"14：解密文件"<<endl
    <<"15：退出系统"<<endl;
}
int main()
{

    int function_flag=17;
    char source_path[PATH_MAX];
    string destination_path;
	string dirPathName;
    string targetFile;      //复制文件的目标文件名
    string sourceDirOrFile;       //备份或移动的源目录或源文件
    string targetDir;       //移动文件的目标父目录或复制备份目录的父目录
	string password;
    struct stat sbuf;


    cout<<"欢迎使用本系统，本系统支持文件类型如下"<<endl<<endl;
    cout<<"1、普通文件；2、软硬连接；3、管道文件；4、目录文件"<<endl<<endl;
    cout<<"当前目录结构如下：\n";
    chdir("/home/bozai/Desktop/superbackup/testFiles");
    primaryBackuper bozai;
    bozai.printDir(".",0);

    while(function_flag!=15)
    {
        print_function();
        cin>>function_flag;
        getchar();
        switch(function_flag)
        {
            case 0://切换工作目录
            {
                cout<<"请输入想要切换的工作目录(相对路径)：\n";
                std::getline(cin,dirPathName);
                getcwd(source_path,PATH_MAX);
                cout<<source_path<<endl;
                dirPathName=string(source_path)+'/'+dirPathName;
                cout<<dirPathName<<endl;    
                if(chdir(dirPathName.c_str())==0)
                {
                    cout<<"目录切换成功,当前工作目录树如下：\n";
                    bozai.printDir(".",0);
                }
                else
                {
                    cout<<"切换失败，请检查后重新输入\n";
                }
                break;            
            }
            case 1: //备份普通文件
            {
                cout<<"请输入想要备份的文件（请输入路径+名称）"<<endl;
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"文件"<<sourceDirOrFile<<"不存在，请检查后重新输入\n";
                    break;
                }
                lstat(sourceDirOrFile.c_str(),&sbuf);
                if(!(S_ISFIFO(sbuf.st_mode)||S_ISLNK(sbuf.st_mode)||S_ISREG(sbuf.st_mode)))
                {
                    cout<<"文件"<<sourceDirOrFile<<"不是本系统支持的文件类型，请检查后重新输入\n";
                    break;
                }
                cout<<"请输入存储备份文件的目录名"<<endl;
                getline(cin,targetDir);
                //处理输入的目录以及文件名，判断是否已有重名文件，若有则须重命名
                strcpy(source_path,sourceDirOrFile.c_str());
                targetFile=targetDir+'/'+basename(source_path); 
                while(access(targetFile.c_str(),F_OK)==0)
                {
                    cout<<"当前目标目录"<<targetDir<<"下已有同名文件或目录，请为备份后的文件重命名\n";
                    getline(cin,targetFile);
                    targetFile=targetDir+'/'+targetFile; 
                }
                ADBackuper adworker;
                adworker.backupRegFile(sourceDirOrFile,targetFile);
                break;
            }
            case 2://备份链接文件
            {
                cout<<"请输入想要备份的链接（相对路径以及名称）\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"链接"<<sourceDirOrFile<<"不存在，请检查后重新输入\n";
                    break;
                }
                lstat(sourceDirOrFile.c_str(),&sbuf);
                if(!S_ISLNK(sbuf.st_mode))
                {
                    cout<<"文件"<<sourceDirOrFile<<"不是链接文件，请检查后重新输入\n";
                    break;
                }
                cout<<"请输入存储备份链接的目录名"<<endl;
                getline(cin,targetDir);
                //处理输入的目录以及文件名，判断是否已有重名文件，若有则须重命名
                strcpy(source_path,sourceDirOrFile.c_str());
                targetFile=targetDir+'/'+basename(source_path); 
                while(access(targetFile.c_str(),F_OK)==0)
                {
                    cout<<"当前目标目录"<<targetDir<<"下已有同名文件或目录，请为备份后的文件重命名\n";
                    getline(cin,targetFile);
                    targetFile=targetDir+'/'+targetFile; 
                }
                ADBackuper ADworker;
                ADworker.backupLINK(sourceDirOrFile,targetFile);
                break;
            }
            case 3://备份管道文件
            {
                cout<<"请输入想要备份的管道（相对路径以及名称）\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"管道"<<sourceDirOrFile<<"不存在，请检查后重新输入\n";
                    break;
                }
                lstat(sourceDirOrFile.c_str(),&sbuf);
                if(!S_ISFIFO(sbuf.st_mode))
                {
                    cout<<"文件"<<sourceDirOrFile<<"不是管道文件，请检查后重新输入\n";
                    break;
                }
                cout<<"请输入存储备份管道的目录名"<<endl;
                getline(cin,targetDir);
                //处理输入的目录以及文件名，判断是否已有重名文件，若有则须重命名
                strcpy(source_path,sourceDirOrFile.c_str());
                targetFile=targetDir+'/'+basename(source_path); 
                while(access(targetFile.c_str(),F_OK)==0)
                {
                    cout<<"当前目标目录"<<targetDir<<"下已有同名文件或目录，请为备份后的文件重命名\n";
                    getline(cin,targetFile);
                    targetFile=targetDir+'/'+targetFile; 
                }
                ADBackuper ADworker;
                ADworker.backupFIFO(sourceDirOrFile,targetFile);
                break;
            }
            case 4://备份目录
            {
                cout<<"请输入想要备份的目录\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"目录"<<sourceDirOrFile<<"不存在，请检查输入或创建目录后重试\n";
                    break;
                }
                stat(sourceDirOrFile.c_str(),&sbuf);
                if(!S_ISDIR(sbuf.st_mode))
                {
                    cout<<"当前输入"<<sourceDirOrFile<<"不是目录，请检查后重新输入\n";
                    break;
                }
                cout<<"请输入将要储存备份目录的父目录\n";
                getline(cin,targetDir);
                if(strcmp(sourceDirOrFile.c_str(),targetDir.c_str())==0)
                {
                    cout<<"两目录禁止同名，退出备份！\n";
                    break;
                }
                ADBackuper ADworker;
                //如果选择移入目录不存在，可直接建立
                stat(targetDir.c_str(),&sbuf);
                if(access(targetDir.c_str(),F_OK)!=0)
                {	
                    char flag;
                    cout<<"当前目标父目录不存在，是否需要直接创建新目录\n"
                    <<"若是请输入y/Y,其他任意键退出\n";
                    cin>>flag;
                    if(flag=='y'||flag=='Y'){	
                        ADworker.mkDir(targetDir);
                    }
                    else break;
                }
                else if(!S_ISDIR(sbuf.st_mode))
                {
                    cout<<"当前输入"<<targetDir<<"已有同名文件，备份失败\n";
                    break;
                }
                ADworker.ADBackupDir(sourceDirOrFile,targetDir);
                break;
            }
            case 5://创建新目录
            {
                cout<<"请输入想要创建的目录名\n"
                <<"(！不支持绝对路径下创建新目录)\n"
                <<"(但可在绝对路径已有目录下创建新目录)"<<endl;
                getline(cin,dirPathName);
                if(access(dirPathName.c_str(),F_OK)==0)
                {
                    cout<<"当前目录"<<dirPathName<<"已有同名文件或目录，请检查后重新输入\n";
                    break;
                }
                ADBackuper ADworker;
                if(ADworker.mkDir(dirPathName))
                    cout<<"创建成功\n";
                else    cout<<"创建失败\n";
                break;
            }
            case 6://备份文件（兼容上述四种文件）
            {
                cout<<"请输入想要备份的文件或目录（相对路径以及名称）\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"文件或目录"<<sourceDirOrFile<<"不存在，请检查后重新输入\n";
                    break;
                }
                lstat(sourceDirOrFile.c_str(),&sbuf);
                if(!(S_ISFIFO(sbuf.st_mode)||S_ISLNK(sbuf.st_mode)||S_ISREG(sbuf.st_mode)||S_ISDIR(sbuf.st_mode)))
                {
                    cout<<"文件"<<sourceDirOrFile<<"类型不在支持范围内，抱歉！\n";
                    break;
                }
                cout<<"请输入存储备份文件或目录的父目录名"<<endl;
                getline(cin,targetDir);
                //若备份目录时出现同名，退出备份
                if((strcmp(sourceDirOrFile.c_str(),targetDir.c_str())==0)&&(S_ISDIR(sbuf.st_mode)))
                {
                    cout<<"两目录禁止同名，退出备份！\n";
                    break;
                }
                //处理输入的目录以及文件名，判断是否已有重名文件，若有则须重命名
                strcpy(source_path,sourceDirOrFile.c_str());
                targetFile=targetDir+'/'+basename(source_path); 
                while(access(targetFile.c_str(),F_OK)==0)
                {
                    cout<<"当前目标目录"<<targetDir<<"下已有同名文件或目录，请为备份后的文件重命名\n";
                    getline(cin,targetFile);
                    targetFile=targetDir+'/'+targetFile; 
                }
                ADBackuper ADworker;
                ADworker.AllFile(sourceDirOrFile,targetDir);
                break;
            }
            case 7://移动文件（兼容性）
            {
                cout<<"请输入想要移动的文件或目录名\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"文件"<<sourceDirOrFile<<"不存在，请检查后重新输入\n";
                    break;
                }
                lstat(sourceDirOrFile.c_str(),&sbuf);
                if(!(S_ISFIFO(sbuf.st_mode)||S_ISLNK(sbuf.st_mode)||S_ISREG(sbuf.st_mode)||S_ISDIR(sbuf.st_mode)))
                {
                    cout<<"文件"<<sourceDirOrFile<<"类型不在支持范围内，抱歉！\n";
                    break;
                }
                cout<<"清输入想要移入的目标目录\n";
                getline(cin,targetDir);
                stat(sourceDirOrFile.c_str(),&sbuf);
                //若移动目录时出现同名，退出移动
                if((strcmp(sourceDirOrFile.c_str(),targetDir.c_str())==0)&&(S_ISDIR(sbuf.st_mode)))
                {
                    cout<<"两目录禁止同名，退出备份！\n";
                    break;
                }
                ADBackuper ADworker;
                ADworker.ADmoveFileOrDir(sourceDirOrFile,targetDir);
                break;
            }
            case 8://删除目录或文件
            {
                cout<<"请输入想要删除的文件或目录名（包括路径）\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"文件或目录"<<sourceDirOrFile<<"不存在，请检查后重新输入\n";
                    break;
                }
                lstat(sourceDirOrFile.c_str(),&sbuf);
                if(!(S_ISDIR(sbuf.st_mode)||S_ISFIFO(sbuf.st_mode)||S_ISLNK(sbuf.st_mode)||S_ISREG(sbuf.st_mode)))
                {
                    cout<<"文件或目录"<<sourceDirOrFile<<"不是本系统支持的文件类型，请检查后重新输入\n";
                    break;
                }
                ADBackuper ADworker;
                ADworker.rmDirOrFile(sourceDirOrFile);
                break;
            }
            case 9://打包文件
            {
                cout<<"请输入要打包的目录：\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK)||bozai.EmptyDir(sourceDirOrFile))
                {
                    cout<<"当前目录不存在或当前目录为空，请检查后重新输入\n";
                }
                else
                {
                    cout<<"请为打包后的文件命名(自动添加后缀)：\n";
                    getline(cin,targetFile);
                    pack_worker paworker;
                    paworker.packDir(sourceDirOrFile,targetFile);
                }
                break;
            }
            case 10://解包文件
            {
                cout<<"请输入需要解包的文件(包括路径和后缀)：\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"当前目录不存在，请检查后重新输入\n";
                }
                else
                {
                    pack_worker paworker;
                    cout<<"请输入解包所得目录的父目录：\n";
                    getline(cin,targetDir);
                    if(access(targetDir.c_str(),F_OK))
                    {	
                        char flag;
                        cout<<"当前目标父目录不存在，是否需要直接创建新目录\n"
                        <<"若是请输入y/Y,其他任意键退出\n";
                        cin>>flag;
                        if(flag=='y'||flag=='Y')
                        {	
                            paworker.mkDir(targetDir);
                        }
                        else 
                            break;
                    }
                    paworker.unpackBag(sourceDirOrFile,targetDir);
                }
                break;
            }
            case 11://压缩文件
            {
                cout<<"请输入需要压缩的文件：\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"当前文件不存在，请检查后后重新输入\n";
                }
                else
                {
                    cout<<"请为压缩后文件命名(自动添加后缀)：\n";
                    getline(cin,targetFile);
                    compress_worker cpworker;
                    cpworker.compress(sourceDirOrFile,targetFile);
                }
                break;
            }
            case 12://解压文件
            {
                cout<<"请输入需要解压的文件(包含后缀)：\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"当前文件不存在，请检查后后重新输入\n";
                }
                else
                {
                    cout<<"请输入存放解压后文件的父目录：\n";
                    getline(cin,targetDir);
                    if(access(targetDir.c_str(),F_OK))
                    {	
                        char flag;
                        cout<<"当前目标父目录不存在，是否需要直接创建新目录\n"
                        <<"若是请输入y/Y,其他任意键退出\n";
                        cin>>flag;
                        if(flag=='y'||flag=='Y')
                        {	
                            bozai.mkDir(targetDir);
                        }
                        else break;
                    }
                    compress_worker cpworker;
                    if(cpworker.decompress(sourceDirOrFile,targetDir))
                    {
                        cout<<"解压成功!\n";
                    }
                }
                break;
            }
            case 13://加密备份
            {
                cout<<"请输入需要加密的文件\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"当前文件不存在，请检查后后重新输入\n";
                }
                else
                {
                    string md5,md5md5;
                    encrypt_worker worker;
                    password=worker.getPSWD();
                    md5=worker.encode(password);
                    md5md5=worker.encode(md5);
                    cout<<"请输入存放加密文件的目录：\n";
                    getline(cin,targetDir);
                    if(access(targetDir.c_str(),F_OK))
                    {	
                    char flag;
                    cout<<"当前目标父目录不存在，是否需要直接创建新目录\n"
                    <<"若是请输入y/Y,其他任意键退出\n";
                    cin>>flag;
                    if(flag=='y'||flag=='Y')
                    {	
                        bozai.mkDir(targetDir);
                    }
                    else break;
                    }
                    worker.encyptFile(md5,md5md5,sourceDirOrFile,targetDir);
                }
                break;
            }
            case 14://解密文件
            {
                cout<<"请输入需要解密的文件\n";
                getline(cin,sourceDirOrFile);
                if(access(sourceDirOrFile.c_str(),F_OK))
                {
                    cout<<"当前文件不存在，请检查后后重新输入\n";
                }
                else
                {
                    string md5,md5md5;
                    encrypt_worker worker;
                    password=worker.getPSWD();
                    md5=worker.encode(password);
                    md5md5=worker.encode(md5);
                    cout<<"请输入存放解密文件的目录：\n";
                    getline(cin,targetDir);
                    if(access(targetDir.c_str(),F_OK))
                    {	
                    char flag;
                    cout<<"当前目标父目录不存在，是否需要直接创建新目录\n"
                    <<"若是请输入y/Y,其他任意键退出\n";
                    cin>>flag;
                    if(flag=='y'||flag=='Y')
                    {	
                        bozai.mkDir(targetDir);
                    }
                    else break;
                    }
                    worker.decryptFile(md5,md5md5,sourceDirOrFile,targetDir);
                }
                break;
            }
            case 15://退出系统
            {
                cout<<"exit successfully!"<<endl;
                break;
            }
            default:
            {    
                cout<<"输入错误，请重新输入！\n"<<endl;
                break;
            }
            cout<<"按任意键继续使用\n";
        }
    }
    return 0;
}