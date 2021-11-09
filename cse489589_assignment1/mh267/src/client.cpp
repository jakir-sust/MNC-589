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
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>

#include <cstring>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#include <sstream>
#include <iterator>
#include <unistd.h>
#include "../include/client.h"
#include "../include/server.h"
#include "../include/logger.h"
#include "../include/ip_address.h"
using namespace std;

//#define BUFFER_SIZE 2048


/**
* main function
*
* @param  argc Number of arguments
* @param  argv The argument list
* @return 0 EXIT_SUCCESS
*/
void write_file(int sockfd);
void send_file(string filename, int sockfd);

bool compare_port(client_info a, client_info b)
{

    if (atoi( a.PORT.c_str() ) < atoi( b.PORT.c_str() )) return 1;
    else return 0;
}
vector<string> get_vector_stringc(char* buffer)
{
    string command = string(buffer);
    std::string buf;
    std::stringstream ss(command);
    vector<string> command_vec;
    while (ss >> buf)
        command_vec.push_back(buf);

    return command_vec;
}

vector<string> split_string(string buffer, string delimiter){


    vector<string> command_vec;
    int start = 0;
    int end = buffer.find(delimiter);
    while (end != -1) {
        //printf("%d %d",start,end);
        command_vec.push_back(buffer.substr(start, end - start) );
        start = end + delimiter.size();
        end = buffer.find(delimiter, start);

    }
    command_vec.push_back(buffer.substr(start, end - start) );
    return command_vec ;
}
struct client_info parse_tuple(string tuple){
    struct client_info info;
    vector<string> split_str = split_string(tuple," ");
    info.IP = split_str.at(0);
    info.PORT = split_str.at(1);
    info.host_name = split_str.at(2);
    //info.login_status = split_str.at(3);
    return info;
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
    vector<struct client_info> client_list;
    int logout = 1;
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
		//else cout<<"Found in select\n";

		for(int sock_index=0; sock_index<=head_socket; sock_index+=1){
            if(FD_ISSET(sock_index, &watch_list)){
                if (sock_index == STDIN){
                    char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
                    memset(msg, '\0', MSG_SIZE);
                    if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
                        exit(-1);

                   // printf("I got: %s from STDIN \n", msg);


                    vector<string> command_vec;
                    command_vec = get_vector_stringc(msg);

                    //cout<<"In client-->>>   "<<command_vec[0]<<"  "<<command_vec[0].size()<<"\n";

                    if (command_vec[0] == "AUTHOR") {
                            cse4589_print_and_log("[AUTHOR:SUCCESS]\n");
                            cse4589_print_and_log("I, mh267, have read and understood the course academic integrity policy.\n");
							cse4589_print_and_log("I, dyadav2, have read and understood the course academic integrity policy.\n");
							cse4589_print_and_log("I, balasub5, have read and understood the course academic integrity policy.\n");
                            cse4589_print_and_log("[AUTHOR:END]\n");
                        // Need to be implemented
                    }
                    else if (command_vec[0] == "IP") {
                        string ip = get_ip();
                        //cout<<"Ip address from my server code = "<<ip<<"\n";
                        cse4589_print_and_log("[IP:SUCCESS]\n");
                        cse4589_print_and_log("IP:%s\n", ip.c_str());
                        cse4589_print_and_log("[IP:END]\n");
                    }
                    else if(command_vec[0]  == "PORT") {
                        //cout<<"Port number from my server code = "<<port<<"\n";
                        cse4589_print_and_log("[PORT:SUCCESS]\n");
                        cse4589_print_and_log("PORT:%s\n", port);
                        cse4589_print_and_log("[PORT:END]\n");
					}
                    else if (command_vec[0] == "SEND") {
                        //char* added_string = add_two_string((char *)"First",(char *) "Second");

                        if(logout) continue;
                        string whole_msg = "";

                        for (int i = 2; i<command_vec.size(); i++) {
                            whole_msg += command_vec[i];
                            if(i != command_vec.size()-1) {
                                whole_msg += " ";
                            }
                        }

                        if(whole_msg.size()>0 && whole_msg[whole_msg.size()-1] == ' ') {
                                    whole_msg = whole_msg.substr(0, whole_msg.size()-1);
                                }

                        //cout<<"MSG and len in client sending -->>>> "<<whole_msg <<" "<<whole_msg.size();



                        string cur_ip = get_ip();
                        char* dest_msg = &whole_msg[0];
                        char *first2 = add_two_string((char *)"SEND", (char *) cur_ip.c_str());
                        char *first3 = add_two_string(first2, (char *) &(command_vec[1][0]));
                        char* added_string = add_two_string(first3, dest_msg);


                        // cout<<added_string<<"\n";
                        msg = added_string;

                        int is_exist = 0;

                        for (int j=0; j< client_list.size();j++){
                            if(client_list[j].IP == command_vec[1]) {
                                is_exist = 1;
                                break;
                            }
                        }

                        if(is_exist && send(server, msg, strlen(msg), 0) == strlen(msg))
                            cse4589_print_and_log("[%s:SUCCESS]\n", command_vec[0].c_str());
                        else
                            cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                        cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());

                        fflush(stdout);

                    }
                    else if (command_vec[0] == "BROADCAST") {

                        string cur_ip = get_ip();
                        char* dest_msg = &command_vec[1][0];
                        char *first2 = add_two_string((char *)"BROADCAST", (char *) cur_ip.c_str());
                        //char *first3 = add_two_string(first2, (char *) &(command_vec[1][0]));
                        char* added_string = add_two_string(first2, dest_msg);


                        // cout<<added_string<<"\n";
                        msg = added_string;
                        //cout<<"MESSAGE SENT FROM THE CLIENT IS :"<<msg<<"\n";
                        if(send(server, msg, strlen(msg), 0) == strlen(msg))
                            cse4589_print_and_log("[%s:SUCCESS]\n", command_vec[0].c_str());
                        else
                            cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                        cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());

                        fflush(stdout);
                    }

                    else if (command_vec[0] == "LOGIN") {

                        logout = 0;

                        if(isValidIP(command_vec[1])==1 && isValidPort(command_vec[2]) == 1){
                            server = connect_to_host((char *)&command_vec[1][0], (char *)&command_vec[2][0]);
                            fdaccept = server;
                            if(fdaccept < 0) perror("Accept failed.");
                                //printf("\nRemote Host connected!\n");

                            /* Add to watched socket list */
                            FD_SET(fdaccept, &master_list);
                            if(fdaccept > head_socket) head_socket = fdaccept;


                            //printf("\nLOGIN to the remote server... ");

                            char hostname[1024];
                            hostname[1023] = '\0';
                            gethostname(hostname, 1023);

                            //cout<<"HOSTNAME --- .>>>   "<<hostname<<"\n";

                            string cur_ip = get_ip();
                            char *first2 = add_two_string((char *)"LOGIN", (char *) cur_ip.c_str());
                            char* added_string = add_two_string(first2, port);
                            msg = add_two_string(added_string, hostname);

                            if(send(server, msg, strlen(msg), 0) == strlen(msg)){
                             ////cse4589_print_and_log("[%s:SUCCESS]\n", command_vec[0].c_str());
                            }
                            else{
                                cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                                cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());
                                continue;
                            }


                            fflush(stdout);

                            /* Initialize buffer to receieve response */
                            char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                            memset(buffer, '\0', BUFFER_SIZE);

                            client_list.clear();

                            if(recv(server, buffer, BUFFER_SIZE, 0) >= 0){//process table
                                // printf("Server responded: %s\n\n", buffer);
                                string temp = string(buffer);
                                vector<string> tuples = split_string(temp,"\n");
                                //printf("Server responded: 1  %d\n\n", tuples.size());
                                for(int i=0; i < tuples.size() - 1; i++){
                                    struct client_info c = parse_tuple(tuples.at(i));
                                client_list.push_back(c);
                            }
                            fflush(stdout);
                        }

                    }
                    else {
                            cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                            cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());
                            continue;
                        }

                    }
                    else if (command_vec[0] == "REFRESH") {
                        int success = 0;
                        char* msg = add_two_string((char*) "REFRESH",(char *)get_ip().c_str());
                        if(send(server, msg, strlen(msg), 0) == strlen(msg))
                            success = 1;

                        char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);

                        if(recv(server, buffer, BUFFER_SIZE, 0) >= 0){//process table
                          // printf("Server responded: %s\n\n", buffer);
                           string temp = string(buffer);
                           vector<string> tuples = split_string(temp,"\n");
                           //printf("Server responded: 1  %d\n\n", tuples.size());
                           client_list.clear();
                           for(int i =0; i < tuples.size() - 1; i++){
                               struct client_info c = parse_tuple(tuples.at(i));
                               client_list.push_back(c);
                           }
                            fflush(stdout);
                        }
                        if(success == 0)
                            cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                        else
                            cse4589_print_and_log("[%s:SUCCESS]\n", command_vec[0].c_str());
                        cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());
                    }
                    else if(command_vec[0] == "LIST") {
                        // Need to be implemented
                        cse4589_print_and_log("[LIST:SUCCESS]\n");
                        sort(client_list.begin(), client_list.end(), compare_port);

                        for(int i = 0 ; i < client_list.size(); i++) {
                            client_info cur = client_list[i];
                            //cout<<cur.IP<<"\n";
                            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i+1, cur.host_name.c_str(), cur.IP.c_str(), atoi(cur.PORT.c_str()));
                        }

                        cse4589_print_and_log("[LIST:END]\n");
					}
                    else if (command_vec[0] == "BLOCK") {
                        // Need to be implemented
                        char* first = add_two_string((char*)"BLOCK",(char *)get_ip().c_str());
                        char* second = add_two_string(first,(char *)command_vec[1].c_str());
                        //look for blocked record in client list.
                        string cur_ip = get_ip();

                        int already_blocked = 0;

                        for(int i = 0; i < client_list.size(); i++){
                            if(client_list[i].IP == cur_ip){
                                for(int j=0; j <client_list[i].blocked_list.size();j++){
                                    struct block_info temp_block_info= client_list[i].blocked_list[j];
                                    if(temp_block_info.blocked_ip == command_vec[1]) {
                                        already_blocked = 1;
                                    }
                                }
                            }
                        }

                        if(already_blocked == 1 || isValidIP(command_vec[1]) == 0){
                             cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                             cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());
                             continue;
                        }

                        int success = 0;
                        for(int i = 0; i < client_list.size(); i++){
                            if(client_list.at(i).IP.compare(cur_ip) == 0){
                                char * msg = add_two_string(second,(char*) client_list.at(i).PORT.c_str());
                                //send
                                struct block_info temp_block_info;
                                temp_block_info.blocked_ip = command_vec[1];
                                client_list[i].blocked_list.push_back(temp_block_info);
                            }
                            else if(client_list.at(i).IP.compare(command_vec[1]) == 0){
                                char * msg = add_two_string(second,(char*) client_list.at(i).PORT.c_str());
                                if(send(server, msg, strlen(msg), 0) == strlen(msg))
                                    success = 1;
                            }
                        }
                        if(success == 0)
                            cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                        else
                            cse4589_print_and_log("[%s:SUCCESS]\n", command_vec[0].c_str());
                        cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());
                    }

                    else if (command_vec[0] == "UNBLOCK") {
                        // Need to be implemented
                        char* first = add_two_string((char*)"UNBLOCK",(char *)get_ip().c_str());
                        char* second = add_two_string(first,(char *)command_vec[1].c_str());
                        //look for blocked record in client list.
                        string cur_ip = get_ip();
                        int already_unblocked = 1;

                        for(int i = 0; i < client_list.size(); i++){
                            if(client_list[i].IP == cur_ip){
                                for(int j=0; j <client_list[i].blocked_list.size();j++){
                                    struct block_info temp_block_info= client_list[i].blocked_list[j];
                                    if(temp_block_info.blocked_ip == command_vec[1]) {
                                        client_list[i].blocked_list.erase(client_list[i].blocked_list.begin() + j);
                                        already_unblocked = 0;
                                    }
                                }
                            }
                        }

                        if(already_unblocked){
                             cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                             cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());
                             continue;
                        }

                        int success = 0;
                        for(int i = 0; i < client_list.size(); i++){
                            if(client_list.at(i).IP.compare(command_vec[1]) == 0){
                                char * msg = add_two_string(second,(char*) client_list.at(i).PORT.c_str());
                                //send
                                if(send(server, msg, strlen(msg), 0) == strlen(msg))
                                    success = 1;
                            }
                        }
                        if(success == 0)
                            cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                        else
                            cse4589_print_and_log("[%s:SUCCESS]\n", command_vec[0].c_str());
                        cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());
                    }
                    else if (command_vec[0] == "LOGOUT") {
                        logout = 1;
                        string cur_ip = get_ip();
                        char *added_string = add_two_string((char *)"LOGOUT", (char *) cur_ip.c_str());
                        msg = added_string;
                        int success = 0;
                        if(send(server, msg, strlen(msg), 0) == strlen(msg))
                            success = 1;

                        if(success == 0)
                            cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                        else
                            cse4589_print_and_log("[%s:SUCCESS]\n", command_vec[0].c_str());
                        cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());

                        fflush(stdout);

                    }
                    else if (command_vec[0] == "EXIT") {
                        string cur_ip = get_ip();
                        char *added_string = add_two_string((char *)"EXIT", (char *) cur_ip.c_str());
                        msg = added_string;
                        int success = 0;
                        if(send(server, msg, strlen(msg), 0) == strlen(msg))
                            success = 1;

                        if(success == 0)
                            cse4589_print_and_log("[%s:ERROR]\n", command_vec[0].c_str());
                        else
                            cse4589_print_and_log("[%s:SUCCESS]\n", command_vec[0].c_str());
                        cse4589_print_and_log("[%s:END]\n", command_vec[0].c_str());

                        fflush(stdout);
                        exit(0);
                    }
                    else if (command_vec[0] == "SENDFILE") {
                        string cur_ip = get_ip();
                        string dest_ip = command_vec[1];
                        string dest_port = "None";
                        string cur_port = "None";

                        for(int i=0; i<client_list.size();i++){
                            if(client_list[i].IP== dest_ip){
                                dest_port = client_list[i].PORT;
                            }
                            if(client_list[i].IP== cur_ip){
                                cur_port = client_list[i].PORT;
                            }
                        }
                        server = connect_to_host((char *)&command_vec[1][0], (char *)&dest_port[0]);
                        fdaccept = server;
                        if(fdaccept < 0) perror("Accept failed.");

                        /* Add to watched socket list */
                        FD_SET(fdaccept, &master_list);
                        if(fdaccept > head_socket) head_socket = fdaccept;

                        char hostname[1024];
                        hostname[1023] = '\0';
                        gethostname(hostname, 1023);

                        char *first2 = add_two_string((char *)"SEND_FILE", (char *) cur_ip.c_str());
                        char* added_string = add_two_string(first2, (char*)cur_port.c_str());
                        msg = added_string;
                        string cur_file = command_vec[2];
                        string filename = cur_file;

                        char data[4096];

                        send_file(filename, server);
                        //fclose(fp);

                    }
                }
                /* Check if new client is requesting connection */
                else if(sock_index == server_socket){
                    int caddr_len = sizeof(client_addr);
                    fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&caddr_len);

                    write_file(fdaccept);

                    if(fdaccept < 0)
                        perror("Accept failed.");

                    //printf("\nRemote Host connected!\n");

                    /* Add to watched socket list */
                    FD_SET(fdaccept, &master_list);
                    if(fdaccept > head_socket) head_socket = fdaccept;
                }

                // Other client is sending data through server
                else {

                    char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                    memset(buffer, '\0', BUFFER_SIZE);

                    if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
                        close(sock_index);
                        //printf("Remote Host Server terminated connection!\n");
                        /* Remove from watched list */
                        FD_CLR(sock_index, &master_list);
                    }
                    else {
                        vector<string> command_vec;
                        command_vec = get_vector_stringc(buffer);

                        if (command_vec[0] == "EVENT") {

                            vector<int> event_pos;

                            for(int i=0; i<command_vec.size();i++)
                                if(command_vec[i] == "EVENT"){
                                    event_pos.push_back(i);
                                }

                            string whole_msg = "";

                            for (int i =0 ; i<event_pos.size();i++) {
                                string sender_msg = "";

                                int cur_event_pos = event_pos[i];
                                for(int j= cur_event_pos +2; j<command_vec.size();j++){
                                    if(command_vec[j] == "EVENT") break;
                                    sender_msg += command_vec[j] +" ";
                                }
                                if(sender_msg.size()>0 && sender_msg[sender_msg.size()-1] == ' ') {
                                    sender_msg = sender_msg.substr(0, sender_msg.size()-1);
                                }

                                //cout<<"MSG and len in client receiving -->>>> "<<whole_msg <<" "<<whole_msg.size();
                                string sender_ip = command_vec[cur_event_pos+1];

                                int already_blocked = 0;
                                string cur_ip = get_ip();

                                for(int i = 0; i < client_list.size(); i++){
                                    if(client_list[i].IP == cur_ip){
                                        for(int j=0; j <client_list[i].blocked_list.size();j++){
                                            struct block_info temp_block_info= client_list[i].blocked_list[j];
                                            if(temp_block_info.blocked_ip == sender_ip) {
                                                already_blocked = 1;
                                            }
                                        }
                                    }
                                }

                                //cout<<"already_blocked   --->>>  "<<already_blocked <<" "<<sender_ip<<" "<<sender_msg<<"\n";

                                if(already_blocked == 0) {
                                //cout<<"Something wromh\n";
                                    cse4589_print_and_log("[%s:SUCCESS]\n", "RECEIVED");
                                    cse4589_print_and_log("msg from:%s\n[msg]:%s\n", sender_ip.c_str(), sender_msg.c_str());
                                    cse4589_print_and_log("[%s:END]\n", "RECEIVED");
                                }
                                //cout<<"Something wromh 2\n";


                            }



                        }

                        if (command_vec[0] == "EVENT_DONE"){
                            cse4589_print_and_log("[%s:SUCCESS]\n", "LOGIN");
                            cse4589_print_and_log("[%s:END]\n", "LOGIN");
                        }



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

void write_file(int sockfd){
    int n;
    FILE *fp;
    string cur_file = "recv_file.txt";
    char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
    memset(buffer, '\0', BUFFER_SIZE);

    char cwd[BUFFER_SIZE];
    getcwd(cwd, sizeof(cwd));

    while (1) {
        //n = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if(recv(sockfd, buffer, BUFFER_SIZE, 0) < 0) break;

        int p =0;

        char * line = strtok(strdup(buffer), "\n");
        while(line) {
           vector<string> command_vec = get_vector_stringc(line);
           if(command_vec[0] == "SENDFILE"){
                p = 1;
                break;
            }
            else if(command_vec[0] == "FILE_NAME"){
                cur_file = command_vec[1];
                string filename = cur_file;
                fp = fopen((filename.c_str()), "w");
                bzero(buffer, BUFFER_SIZE);
                line  = strtok(NULL, "\n");
                continue;
            }
            cout<<"Data received  "<<line<<"\n";
            fprintf(fp, "%s\n", line);
            line  = strtok(NULL, "\n");
        }

        bzero(buffer, BUFFER_SIZE);
        if(p==1) break;
    }
    cout<<"Receiving file done\n";
    fclose(fp);
    return;
}

void send_file(string filename, int sockfd){
    int n;
    FILE* fp = fopen(filename.c_str(), "r");
    //char data[BUFFER_SIZE] = {0};
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    //cout<<"OK in send file function\n";
    char* f_name = add_two_string((char*)"FILE_NAME",(char*) filename.c_str());

    if(send(sockfd, f_name, strlen(f_name), 0) == strlen(f_name)){
            cout<<"Sending file name done    "<<f_name<<"\n";
        }

    while ((read = getline(&line, &len, fp)) != -1) {

        char* data = (char*)string(line).c_str();

        if(send(sockfd, data, strlen(data), 0) == strlen(data)){
            cout<<"Sending done    "<<data<<"\n";
        }
        else {
            perror("[-]Error in sending file.");
            exit(1);
        }
    }

    string command = "SENDFILE";
    char* data = (char*)command.c_str();
    if(send(sockfd, data, strlen(data), 0) == strlen(data)){
        cout<<"Sending done    "<<data<<"\n";
    }
    fclose(fp);

    cout<<"Sending file done\n";
}