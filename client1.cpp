#ifdef __WIN32__
#include <io.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <cstring>
#include <cstdio>
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

void ser_rep(int sock, char *server_reply)
{
	while(1)
  	{
	    if(recv(sock, server_reply, 2000, 0) < 0)
	      	continue;
	    cout << "-----   " << server_reply << endl;
	    memset(server_reply, 0, sizeof(server_reply));
  	}
}

int main()
{
	mutex contr;
  int sock;
  sockaddr_in client;
  char message[1000], server_reply[2000];
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1)
  {
    cout << "Could create socket";
    return 1;
  }

  client.sin_addr.s_addr = inet_addr("127.0.0.1");
  client.sin_family = AF_INET;
  client.sin_port = htons(8888);

  if(connect(sock, (sockaddr *)& client, sizeof(client)) < 0)
  {
    cout << "Connect falied. Error";
    return 1;
  }
  cout << "Connected" << endl;
  thread showServerRep(ser_rep, sock, server_reply);
  while(1)
  {
    memset(server_reply, 0, sizeof(server_reply));
    cout << "-----   ";
    cin.getline(message, sizeof(message));
    if((send(sock, message, strlen(message), 0)) < 0)
    {
      cout << "Send falied";
      return 1;
    }
  showServerRep.join();
  }
  return 0;
}
