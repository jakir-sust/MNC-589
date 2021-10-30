/**
* @server
* @author  Swetank Kumar Saha <swetankk@buffalo.edu>, Shivang Aggarwal <shivanga@buffalo.edu>
* @version 1.0
*
* @section LICENSE
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details at
* http://www.gnu.org/copyleft/gpl.html
*
* @section DESCRIPTION
*
* This file contains the server init and main while loop for tha application.
* Uses the select() API to multiplex between network I/O and STDIN.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <string>

#include "ip_address.cpp"
using namespace std;

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256

/**
* main function
*
* @param  argc Number of arguments
* @param  argv The argument list
* @return 0 EXIT_SUCCESS
*/
struct client_info{
  string IP, PORT;
  int fd;
  int socket_index;
};
vector<client_info> client_list;

vector<string> get_vector_string(string buffer)
{
    string command = buffer;
    std::string buf;
    std::stringstream ss(command);
    std::vector<std::string> command_vec;
    while (ss >> buf)
        command_vec.push_back(buf);

    return command_vec;
}
void add_new_client(string IP, string PORT, int fd, int sock_index)
{
    if (client_list.size() ==0)
    {
        cout<<"Empty logged in clients\n";
    }

    client_info client;
    client.IP = IP;
    client.PORT = PORT;
    client.fd = fd;
    client.socket_index = sock_index;
    client_list.push_back(client);

    for(int i = 0; i < client_list.size(); i++)
    {
        cout<<"Client info list   "<<client_list[i].IP<< " "<<client_list[i].PORT<<" "<<client_list[i].fd<< " "<<client_list[i].socket_index<<"\n";
    }
}

client_info get_client_info(string IP)
{
    client_info client_with_IP;

    client_with_IP.IP = "None";
    for(int i = 0; i < client_list.size(); i++)
    {
        cout<<"Client info list   "<<client_list[i].IP<< " "<<client_list[i].PORT<<" "<<client_list[i].fd<< " "<<client_list[i].socket_index<<"\n";
        if(client_list[i].IP == IP) {
            client_with_IP = client_list[i];
            break;
        }
    }
    //cout<<"After getting client info   "<<client_with_IP.IP<<"\n";
    return client_with_IP;
}

void server_main(int argc, char *port)
{
	if(argc != 2) {
		//printf("Usage:%s [port]\n", argv[0]);
		exit(-1);
	}
	std::cout<<port<<"\n";
	
	int server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len;
	struct sockaddr_in client_addr;
	struct addrinfo hints, *res;
	fd_set master_list, watch_list;

	/* Set up hints structure */
	memset(&hints, 0, sizeof(hints));
    	hints.ai_family = AF_INET;
    	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_flags = AI_PASSIVE;

	/* Fill up address structures */
	if (getaddrinfo(NULL, port, &hints, &res) != 0)
		perror("getaddrinfo failed");
	
	/* Socket */
	server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(server_socket < 0)
		perror("Cannot create socket");
	
	/* Bind */
	if(bind(server_socket, res->ai_addr, res->ai_addrlen) < 0 )
		perror("Bind failed");

	freeaddrinfo(res);
	
	/* Listen */
	if(listen(server_socket, BACKLOG) < 0)
		perror("Unable to listen on port");
	
	/* ---------------------------------------------------------------------------- */
	
	/* Zero select FD sets */
	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);
	
	/* Register the listening socket */
	FD_SET(server_socket, &master_list);
	/* Register STDIN */
	FD_SET(STDIN, &master_list);
	
	head_socket = server_socket;
	
	while(TRUE){
		memcpy(&watch_list, &master_list, sizeof(master_list));
		
		//printf("\n[PA1-Server@CSE489/589]$ ");
		//fflush(stdout);
		
		/* select() system call. This will BLOCK */
		selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
		if(selret < 0)
			perror("select failed.");

		printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		/* Check if we have sockets/STDIN to process */
		if(selret > 0){
		    cout<<"OK\n";
			/* Loop through socket descriptors to check which ones are ready */
			for(sock_index=0; sock_index<=head_socket; sock_index+=1){
				
				if(FD_ISSET(sock_index, &watch_list)){
					
					/* Check if new command on STDIN */
					if (sock_index == STDIN){
						char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);
						
						memset(cmd, '\0', CMD_SIZE);
						if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
							exit(-1);
						
						printf("\nI got: %s\n", cmd);

						string str_cmd = string(cmd);
						str_cmd = str_cmd.substr(0, str_cmd.size()-1);
						cout<<str_cmd<<"  "<<str_cmd.size()<<"\n";

						if(str_cmd == "IP") {
						    string ip = get_ip();
        	                cout<<"Ip address from my server code = "<<ip<<"\n";
						}
						else if(str_cmd == "PORT") {
						    string ip = get_ip();
        	                cout<<"Port number "<<port<<"\n";
						}
						
						//Process PA1 commands here ...
						
						free(cmd);
					}
					/* Check if new client is requesting connection */
					else if(sock_index == server_socket){
						caddr_len = sizeof(client_addr);
						fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&caddr_len);
						if(fdaccept < 0)
							perror("Accept failed.");
						
						printf("\nRemote Host connected!\n");

						/* Add to watched socket list */
						FD_SET(fdaccept, &master_list);
						if(fdaccept > head_socket) head_socket = fdaccept;
					}
					/* Read from existing clients */
					else{
						/* Initialize buffer to receieve response */
						char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
						memset(buffer, '\0', BUFFER_SIZE);
						
						if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
							close(sock_index);
							printf("Remote Host terminated connection!\n");
							
							/* Remove from watched list */
							FD_CLR(sock_index, &master_list);
						}
						else {
							//Process incoming data from existing clients here ...

							cout<<"Enter else case\n";

                            ///*
                            vector<string> command_vec;

                            string buffer_str = string(buffer);

                            cout<<"BUFFER string ====  "<<buffer_str<<"\n";

                            command_vec = get_vector_string(buffer);
                            //if (command_vec[0] == "LOGIN") {

                            cout<< command_vec[0]<<"\n";
                            if (command_vec[0] == "LOGIN") {
                                 add_new_client(command_vec[1], command_vec[2], fdaccept, sock_index);
                            }

                            if (command_vec[0] == "SEND") {
                                cout<<"destination IP in server =  "<<command_vec[1]<< "  "<<command_vec[2]<<"\n";
                                client_info dest_client = get_client_info(command_vec[1]);

                                if(dest_client.IP == "None") cout<<"Not in the list\n";
                                else {
                                    cout<<"Dest IP found\n";
                                    if(send(dest_client.fd, buffer, strlen(buffer), 0) == strlen(buffer))
								        printf("Sending to destination Done!\n");

                                }

                            }

							printf("\nClient sent me: %s\n", buffer);
							printf("ECHOing it back to the remote host ... ");
							if(send(fdaccept, buffer, strlen(buffer), 0) == strlen(buffer))
								printf("Done!\n");
							fflush(stdout);
						}
						
						free(buffer);
					}
				}
			}
		}
	}
	
	return ;
}
