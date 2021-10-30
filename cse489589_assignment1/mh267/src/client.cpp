/**
* @client
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
* This file contains the client.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
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

#include "ip_address.cpp"
using namespace std;

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256
#define MSG_SIZE 256

int connect_to_host(char *server_ip, char *server_port);

/**
* main function
*
* @param  argc Number of arguments
* @param  argv The argument list
* @return 0 EXIT_SUCCESS
*/
vector<string> get_vector_string(char* buffer)
{
    string command = string(buffer);
    std::string buf;
    std::stringstream ss(command);
    vector<string> command_vec;
    while (ss >> buf)
        command_vec.push_back(buf);

    return command_vec;
}

char* add_two_string(char *str1, char* str2)
{

    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    /* one for extra char, one for trailing zero */

    size_t len = len1 + len2 + 2;
    char *str3 = (char*) malloc(sizeof(char)*len);



    strcpy(str3, str1);
    str3[len1] = ' ';

    for (int i = 0; i < len2; i++) {
        str3[i + len1 + 1] = str2[i];
    }
    //cout<<"CAlled\n";
    str3[len1 + len2 + 1] = '\0';

   // printf("%s\n", str3);

    return str3;
}

void client_main(int argc, string ip, char *port)
{
	if(argc != 3) {
		//printf("Usage:%s [ip] [port]\n", argv[0]);
		exit(-1);
	}

	int server;

	int server_socket, head_socket;
	int fdaccept=0;
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

	/* Zero select FD sets */
	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);

	/* Register the listening socket */
	FD_SET(server_socket, &master_list);
	/* Register STDIN */
	FD_SET(STDIN, &master_list);

	head_socket = server_socket;

	char *ip_addr = &ip[0];

	while(TRUE){
		printf("\n[PA1-Client@CSE489/589]$ ");
		fflush(stdout);

		memcpy(&watch_list, &master_list, sizeof(master_list));

		int selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
		if(selret < 0)
			perror("select failed.");
		else cout<<"Found in select\n";

		for(int sock_index=0; sock_index<=head_socket; sock_index+=1){
            if(FD_ISSET(sock_index, &watch_list)){
                if (sock_index == STDIN){
                    char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
                    memset(msg, '\0', MSG_SIZE);
                    if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
                        exit(-1);

                    printf("I got: %s (size:%d chars)\n", msg, strlen(msg));


                    vector<string> command_vec;
                    command_vec = get_vector_string(msg);

                    cout<<"In client-->>>   "<<command_vec[0]<<"  "<<command_vec[0].size()<<"\n";

                    if (command_vec[0] == "IP") {
                        string ip = get_ip();
                        cout<<"Ip address from client my code = "<<ip<<"\n";
                    }
                    else if (command_vec[0] == "SEND") {
                        //char* added_string = add_two_string((char *)"First",(char *) "Second");

                        string cur_ip = get_ip();
                        char* dest_msg = &command_vec[2][0];
                        char *first2 = add_two_string((char *)"SEND", (char *) "128.205.36.33");
                        char* added_string = add_two_string(first2, dest_msg);

                        cout<<added_string<<"\n";
                        msg = added_string;

                        if(send(server, msg, strlen(msg), 0) == strlen(msg))
                            printf("sending to server Done!\n");
                        fflush(stdout);

                    }

                    if (command_vec[0] == "LOGIN") {

                        server = connect_to_host((char *)ip_addr, port);
                        fdaccept = server;
                        if(fdaccept < 0) perror("Accept failed.");
                        //printf("\nRemote Host connected!\n");

                        /* Add to watched socket list */
                        FD_SET(fdaccept, &master_list);
                        if(fdaccept > head_socket) head_socket = fdaccept;


                        printf("\nSENDing it to the remote server ... ");

                        string cur_ip = get_ip();
                        char *first2 = add_two_string((char *)"LOGIN", (char *) cur_ip.c_str());
                        char* added_string = add_two_string(first2, port);

                        cout<<added_string<<"\n";
                        msg = added_string;


                        if(send(server, msg, strlen(msg), 0) == strlen(msg))
                            printf("Sending to server Done!\n");

                        fflush(stdout);

                        /* Initialize buffer to receieve response */
                        char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);

                        if(recv(server, buffer, BUFFER_SIZE, 0) >= 0){
                            printf("Server responded: %s\n\n", buffer);
                            fflush(stdout);
                        }

                }
            }

            else {

                char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                memset(buffer, '\0', BUFFER_SIZE);

                if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
                    close(sock_index);
                    printf("Remote Host Server terminated connection!\n");
                    /* Remove from watched list */
                    FD_CLR(sock_index, &master_list);
                }
                else {
                    printf("\nServer sent me: %s\n", buffer);
                    fflush(stdout);
                }
            }

        }

	}
	}
}

int connect_to_host(char *server_ip, char* server_port)
{
	int fdsocket;
	struct addrinfo hints, *res;

	/* Set up hints structure */	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/* Fill up address structures */	
	if (getaddrinfo(server_ip, server_port, &hints, &res) != 0)
		perror("getaddrinfo failed");

	/* Socket */
	fdsocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(fdsocket < 0)
		perror("Failed to create socket");
	
	/* Connect */
	if(connect(fdsocket, res->ai_addr, res->ai_addrlen) < 0)
		perror("Connect failed");
	
	freeaddrinfo(res);

	return fdsocket;
}
