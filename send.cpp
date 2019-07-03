#include<iostream>
#include<json/json.h>
#include<string.h>
#include<fcntl.h>
#include<termios.h>
using namespace std;

int fd;

int open_serial(char* slaveName)
{
    fd = open(slaveName ,O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(fd == -1)
    {
        cout<<"打开虚拟串口失败!"<<endl;
        return -1;
    }

    cout<<"成功打开虚拟串口："<<slaveName<<endl;
    return 0;
}

void link_port()
{
	int bt = 0;
	cout<<"请设置波特率："<<endl;
	cin>>bt;
    struct termios option;
    tcgetattr(fd,&option);
    cfmakeraw(&option);
    cfsetispeed(&option,bt);
    cfsetospeed(&option,bt);
    tcsetattr(fd, TCSANOW,&option);
	cout<<"设置成功！"<<endl;
}

int main(int argc, char*argv[])
{

    if(open_serial(argv[1]) <0)
    {
        return -1;
    }
	
	link_port();

	int file_fd = open(argv[2],O_RDONLY);
	if(file_fd < 0)
	{
		cout<<"文件不存在！"<<endl;
		return 0;
	}

	while(1)
	{
		char sbuf[4096] = {};
		int file_len = read(file_fd, sbuf, 4096);

		if(file_len == 0)
		{
			cout<<"成功传输文件:"<<argv[2]<<endl;
			break;
		}

		Json::Value val;
		val["filename"] = argv[2];
		val["filemessage"] = sbuf;

		char json_file[4096] = {};
		strcpy(json_file, val.toStyledString().c_str());
		int length = strlen(json_file);

    	
    	if(write(fd, json_file, length) == -1)
    	{
        	cout<<"往串口中写文件时，出现了一些问题。。。"<<endl;
			break;
    	}
	}
	close(file_fd);
	return 0;
}
