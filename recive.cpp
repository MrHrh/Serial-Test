#include<iostream>
#include<string.h>
#include<fcntl.h>
#include<termios.h>
#include<json/json.h>
using namespace std;

int fd;

int open_serial(char* slaveName)
{
    fd = open(slaveName,O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd == -1)
    {
        cout<<"虚拟串口打开失败！"<<endl;
        return -1;
    }

    cout<<"成功打开虚拟串口:"<<slaveName<<endl;
    return 0;
}

int create_file(string filename)
{
	int file_fd = open(filename.c_str(), O_WRONLY|O_CREAT|O_APPEND);
	if(file_fd == -1)
	{
		cout<<"创建文件失败"<<endl;
	}
	return file_fd;
}

void write_file(int file_fd, string filemessage)
{
	int retv = write(file_fd, filemessage.c_str(), strlen(filemessage.c_str()));
	if(retv < 0)
	{
		cout<<"写入文件失败"<<endl;
	}
}

void link_port()
{
	int bt = 0;
	cout<<"请设置波特率："<<endl;
	cin>>bt;
    struct termios opt;
    tcgetattr(fd,&opt);
    cfmakeraw(&opt);

    cfsetispeed(&opt,bt);
    cfsetospeed(&opt,bt);
    tcsetattr(fd, TCSANOW,&opt);
	cout<<"设置成功！"<<endl;
}

int main(int argc, char *argv[])
{
    if(open_serial(argv[1])<0)
		return 0;
	
	link_port();
   
	Json::Value val;
	Json::Reader Read;
   	char rbuf[4096] = {};
	while(1)
	{
    	if(read(fd,rbuf, 4096) > 0)
    	{	
			Read.parse(rbuf, val);
			string filename = val["filename"].asString();
			int file_fd = create_file(filename);

			string filemessage = val["filemessage"].asString();
			write_file(file_fd, filemessage);
			
			cout<<"文件传输成功："<<filename<<endl;
			close(file_fd);
		}
	}
    return 0;
}
