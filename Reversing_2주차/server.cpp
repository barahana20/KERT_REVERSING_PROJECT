#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <cassert>

using namespace std;

#define PACKET_SIZE 1024
SOCKET skt, client_sock;
int socket_recv(int socket, char* buffer, int size)
{
    int total_received;
    int received;

    assert(buffer);
    assert(size > 0);

    total_received = 0;

    while (size)
    {
        received = recv(socket, buffer, size, 0);

        // 리턴값이 = 0 이면 close되었음을 의미
        // 리턴값이 < 0 이면 오류 발생을 의미
        if (received <= 0) break;

        total_received += received;
        size -= received;

        buffer += received;
    }

    return received > 0 ? total_received : received;
}
int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    skt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4444);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(skt, (SOCKADDR*)&addr, sizeof(addr));
    listen(skt, SOMAXCONN);

    SOCKADDR_IN client = {};
    int client_size = sizeof(client);
    ZeroMemory(&client, client_size);
    client_sock = accept(skt, (SOCKADDR*)&client, &client_size);

    int totalBufferNum;
    int BufferNum = 0;
    int readBytes;
    long file_size;
    char buf[PACKET_SIZE];

    FILE* fp;
    fp = fopen("filelist.txt", "wb"); //파일열고
    socket_recv(client_sock, buf, PACKET_SIZE); //파일사이즈받기 
    file_size = atol(buf); //char->long변환 
    totalBufferNum = file_size / PACKET_SIZE + 1;
    //전체사이즈 = 파일전체사이즈 / 받고있는데이터
    while (BufferNum != totalBufferNum) {
        readBytes = socket_recv(client_sock, buf, PACKET_SIZE);
        //데이터와 데이터의크기 받기 
        BufferNum++;
        fwrite(buf, sizeof(char), readBytes, fp);
        //데이터와 데이터의크기만큼 쓰기 
    }
    fclose(fp);
    closesocket(client_sock);
    closesocket(skt);
    WSACleanup();
}