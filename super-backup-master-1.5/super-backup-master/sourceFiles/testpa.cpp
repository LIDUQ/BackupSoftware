#include"../headFiles/packANDunpack.h"


int main()
{
//    int name=100;
//    string name1=to_string(name);
//     cout<<atoi(name1.c_str())<<name1;
    // string sourcedir,targetfile;
    // cout<<"打包目录\n";
    // cin>>sourcedir;
    // cout<<"目标文件名\n";
    // cin>>targetfile;
    pack_worker ADworker;
    // if(ADworker.packDir(sourcedir,targetfile))
    //     cout<<"打包成功！\n";
    ADworker.unpackBag("test3/test1.bo","test2");
    
    // string workdir;
    // primaryBackuper worker;
    // cin>>workdir;
    // worker.printDir(workdir,4);
   
    return  0;
}