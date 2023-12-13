#include"../headFiles/encryptANDdecode.h"

encrypt_worker::encrypt_worker(/* args */)
{
}
encrypt_worker::~encrypt_worker()
{
}
string encrypt_worker::getPSWD()
{
    string password,password1;
    int flag = 1; 
    system("stty -echo");
    while (flag)
    {
        cout<<"请输入密码(不显示明文)：\n";
        getline(cin,password);
        cout<<"请再次输入密码进行确认:\n";
        getline(cin,password1);
        if(password1==password)
        flag=0;
        else
        {
            cout<<"两次输入密码不符，请重新输入!\n";
        }
    }
    system("stty echo");
    return password;
}
bool encrypt_worker::encyptFile(string md5ofpwd,string md5ofmd5odpwd,string sourcefile,string targetdir)
{
    char keyStr[16];
    //最后将密码的MD5的MD5写入文件末尾，以便检验密码是否正确

    for(int i=0;i<16;i++)
    {
        keyStr[i]=char(strtol(md5ofpwd.substr(2*i,2).c_str(),NULL,16));
    }
    //用于处理最后一字节
    struct stat srcbuf;
    int count,i,offset;
	
	byte key[16]; 
	charToByte(key, keyStr);
	// 密钥扩展
	word w[4*(Nr+1)];
	KeyExpansion(key, w);
	bitset<128> data;
	byte plain[16];
    data.reset();
 
	int fin,fout;
    string targetfile;
    char sourcpath[PATH_MAX];
    strcpy(sourcpath,sourcefile.c_str());
    targetfile=targetdir+'/'+basename(sourcpath)+".en";
    cout<<targetfile;

	fin=open(sourcefile.c_str(),O_RDONLY);
    stat(sourcefile.c_str(),&srcbuf);
    count=srcbuf.st_size/16;
    offset=srcbuf.st_size%16;
	fout=open(targetfile.c_str(),O_RDWR|O_CREAT|O_TRUNC|O_APPEND,0644);
	for(i=0;i<count;i++)
    {
        read(fin,(char*)&data, sizeof(data));
        divideToByte(plain, data);
		encrypt(plain, w);
		data = mergeByte(plain);
		write(fout,(char*)&data, sizeof(data));
		data.reset();  // 置0
    }
    //最后一块加密时补充8个字节16
    if(offset==0)
    {
        memset((char*)&data,16,16);
        divideToByte(plain, data);
		encrypt(plain, w);
		data = mergeByte(plain);
		write(fout,(char*)&data, sizeof(data));
    }
    //否则按照offset填充
    else
    {
        read(fin,(char*)&data,offset);
        memset((char*)&data+offset,16-offset,16-offset);
        divideToByte(plain, data);
		encrypt(plain, w);
		data = mergeByte(plain);
		write(fout,(char*)&data, sizeof(data));
    }
    char buffer[32];
    strcpy(buffer,md5ofmd5odpwd.c_str());
    write(fout,buffer,32);

	close(fin);
	close(fout);
    return true;
	
}

bool encrypt_worker::decryptFile(string md5ofpwd,string md5md5ofpwd, string sourcefile,string targetdir)
{
    //验证文件后缀
    if (sourcefile.substr(sourcefile.find_last_of(".") + 1) != "en")
    {
        cout<<"文件格式不符，解密失败！\n";
        return false;
    }    
    int count,i;	
    int fin,fout;
    string targetfile;
    char source[PATH_MAX];
    struct stat srcbuf;
    //首先验证密码正确性
    string filetail;
    char buffer[323];
    fin=open(sourcefile.c_str(),O_RDONLY);
    lseek(fin,-32,SEEK_END);
    read(fin,buffer,32);
    buffer[32]='\0';
    close(fin);
    close(fout);
    filetail=buffer;
    if(md5md5ofpwd!=filetail)
    {
        cout<<"密码错误，退出解密！\n";
        return false;
    }
    else
        cout<<"密码正确，开始解密文件！\n";
    //密码正确则将密码的MD5值转化为密钥
    char keyStr[16];;
    for(int i=0;i<16;i++)
    {
        keyStr[i]=char(strtol(md5ofpwd.substr(2*i,2).c_str(),NULL,16));
    }
    byte key[16]; 
	charToByte(key, keyStr);
	// 密钥扩展
	word w[4*(Nr+1)];
	KeyExpansion(key, w);
	bitset<128> data;
    bitset<8> off;
	byte plain[16];

    strcpy(source,sourcefile.substr(0,sourcefile.find_last_of('.')).c_str());
    targetfile=targetdir+'/'+basename(source);
    //16字节一组，开始解密
	fin=open(sourcefile.c_str(),O_RDONLY);
	fout=open(targetfile.c_str(),O_RDWR|O_CREAT|O_TRUNC|O_APPEND,0644);
    stat(sourcefile.c_str(),&srcbuf);
    count=srcbuf.st_size/16-2;
    for(i=0;i<count-1;i++)
    {
        read(fin,(char*)&data, sizeof(data));
        divideToByte(plain, data );
		decrypt(plain, w);
		data = mergeByte(plain);
		write(fout,(char*)&data, sizeof(data));
		data.reset();  // 置0
    }
    //处理最后16字节
    read(fin,(char*)&data, sizeof(data));
    divideToByte(plain, data );
	decrypt(plain, w);
	data = mergeByte(plain);
	write(fout,(char*)&data,16-plain[0].to_ulong());
	data.reset();  // 置0
    
    close(fin);
    close(fout);
    cout<<"文件解密完成，请查看\n";
    return true;

}