#include <string>
#include <vector>

#ifndef SERVER_H_
#define SERVER_H_
using namespace std;

struct client_info{
  string IP, PORT, host_name;

  int fd;
  int socket_index;
  int num_msg_sent;
  int num_msg_rcv;
  int login_status;
  vector<string> blocked_list;
};

vector<string> get_vector_string(string buffer);
void add_new_client(string IP, string PORT, int fd, int sock_index);
client_info get_client_info(string IP);
void server_main(int argc, char *port);

#endif