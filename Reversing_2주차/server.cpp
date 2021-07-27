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

        // ���ϰ��� = 0 �̸� close�Ǿ����� �ǹ�
        // ���ϰ��� < 0 �̸� ���� �߻��� �ǹ�
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
    fp = fopen("filelist.txt", "wb"); //���Ͽ���
    socket_recv(client_sock, buf, PACKET_SIZE); //���ϻ�����ޱ� 
    file_size = atol(buf); //char->long��ȯ 
    totalBufferNum = file_size / PACKET_SIZE + 1;
    //��ü������ = ������ü������ / �ް��ִµ�����
    while (BufferNum != totalBufferNum) {
        readBytes = socket_recv(client_sock, buf, PACKET_SIZE);
        //�����Ϳ� ��������ũ�� �ޱ� 
        BufferNum++;
        fwrite(buf, sizeof(char), readBytes, fp);
        //�����Ϳ� ��������ũ�⸸ŭ ���� 
    }
    fclose(fp);
    closesocket(client_sock);
    closesocket(skt);
    WSACleanup();
}