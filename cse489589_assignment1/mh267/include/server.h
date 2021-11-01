#include <string>
#include <vector>
#include <queue>

#ifndef SERVER_H_
#define SERVER_H_
using namespace std;

struct buffer_info{
    string sender_ip;
    string sender_msg;
};

struct block_info{
    string blocked_ip;
    string blocked_host_name;
    string blocked_port;
};

struct client_info{
  string IP;
  string PORT;
  string host_name;
  int fd;
  int socket_index;
  int num_msg_sent;
  int num_msg_rcv;
  string login_status;

  vector<block_info> blocked_list;
  queue<buffer_info> buffer_msg;
};

vector<string> get_vector_string(string buffer);
void add_new_client(string IP, string PORT, int fd, int sock_index);
client_info get_client_info(string IP);
void server_main(int argc, char *port);

#endif