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

void handler(int client1, int client2, int &size_of_mess, char *mess, mutex &detach)
{
  do
  {
    detach.lock();
    size_of_mess = recv(client1, mess, 2000, 0);
    write(client2, mess, strlen(mess));
    memset(mess, 0, sizeof(mess));
    detach.unlock();
  }
  while(size_of_mess > 0);
}

int main()
{
  mutex cont;
  char client_message[2000];
  int socket_desc, c, size_of_mess1, size_of_mess2, client1_sock, client2_sock;
  sockaddr_in server, client1, client2;
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if(socket_desc == -1)
  {
    cout << "Error: socket dosen't return descriptor" << endl;
    return 1;
  }
  cout << "Socket cteated" << endl;

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(8888);

  if(bind(socket_desc, (sockaddr *)&server, sizeof(server)) < 0)
  {
    cout << "Error of bing" << endl;
    return 1;
  }
  cout << "Bind done" << endl << "Waiting for connections" << endl;

  listen(socket_desc, 3);
  c = sizeof(sockaddr_in);
  client1_sock = accept(socket_desc, (sockaddr*)&client1, (socklen_t *)&client1);

  cout << "Wait for second client" << endl;
  listen(socket_desc, 3);
  client2_sock = accept(socket_desc, (sockaddr*)&client2, (socklen_t *)&client2);

  if((client1_sock||client2_sock) == -1)
  {
    cout << "Accept falied" << endl;
    return 1;
  }
  cout << "Accept succesful" << endl;

  //(int client1, int client2, char *mess, mutex &detach)
  thread handling_conections1(handler, client1_sock, client2_sock, ref(size_of_mess1), client_message, ref(cont));
  thread handling_conections2(handler, client2_sock, client1_sock, ref(size_of_mess2), client_message, ref(cont));
  handling_conections1.join();
  handling_conections2.join();

  // One main thread !!!!!!!!!!!!!!!!!!!!
  // do
  // {
  //   size_of_mess1 = recv(client1_sock, client_message, 2000, 0);
  //   write(client2_sock, client_message, strlen(client_message));
  //   size_of_mess2 = recv(client2_sock, client_message, 2000, 0);
  //   write(client1_sock, client_message, strlen(client_message));
  //   memset(client_message, 0, sizeof(client_message));
  // }
  // while((size_of_mess1||size_of_mess2) > 0);
  if((size_of_mess1||size_of_mess2) == 0)
  {
    cout << "Clients disconnected" << endl;
    fflush(stdout);
  }
  else if((size_of_mess1||size_of_mess2) == -1)
  {
    cout << "Error of recv" << endl;
    return 1;
  }
  return 0;
}
