#include <vector>
#include <string>
#ifndef CLIENT_H_
#define CLIENT_H_

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256
#define MSG_SIZE 256
using namespace std;

int connect_to_host(char *server_ip, char *server_port);
vector<string> get_vector_stringc(char* buffer);
char* add_two_string(char *str1, char* str2);
void client_main(int argc, string ip, char *port);
int connect_to_host(char *server_ip, char* server_port);


#endif