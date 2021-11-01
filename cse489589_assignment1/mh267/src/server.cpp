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

#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "../include/server.h"
#include "../include/ip_address.h"
#include "../include/logger.h"

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
vector<client_info> client_list;

bool compare(client_info a, client_info b)
{
    if (a.PORT < b.PORT) return 1;
    else return 0;
}

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

string get_host_name(string ip)
{
    for(int i = 0; i < client_list.size(); i++) {
        if(client_list[i].IP == ip) {
            return client_list[i].host_name;
        }
    }
    return "None";

}
void add_new_client(string IP, string PORT, string client_host_name, int fd, int sock_index)
{
    if (client_list.size() ==0)
    {
        //cout<<"Empty logged in clients\n";
    }

    int client_exist = 0;
    for(int i = 0; i < client_list.size(); i++) {
        if(client_list[i].IP == IP) {
            client_exist = 1;
            client_list[i].login_status = "logged-in";
            break;
        }
    }

    if(client_exist == 0)
    {
        client_info client;
        client.IP = IP;
        client.PORT = PORT;
        client.host_name = client_host_name; //need to be updated
        client.fd = fd;
        client.socket_index = sock_index;
        client.num_msg_sent = 0;
        client.num_msg_rcv = 0;
        client.login_status = "logged-in";
        client_list.push_back(client);
    }

    for(int i = 0; i < client_list.size(); i++)
    {
        cout<<"Client info list   "<<client_list[i].IP<< " "<<client_list[i].PORT<<" "<<client_list[i].fd<< " "<<client_list[i].socket_index<<"\n";
    }
    cout<<"--------------------------------------------------\n";
}

client_info get_client_info(string IP)
{
    client_info client_with_IP;

    client_with_IP.IP = "None";
    for(int i = 0; i < client_list.size(); i++)
    {
        //cout<<"Client info list   "<<client_list[i].IP<< " "<<client_list[i].PORT<<" "<<client_list[i].fd<< " "<<client_list[i].socket_index<<"\n";
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
	std::cout<<"Port number in server ==   "<<port<<"\n";
	
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
		
		printf("\n[PA1-Server@CSE489/589]$ ");
		fflush(stdout);
		
		/* select() system call. This will BLOCK */
		selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
		if(selret < 0)
			perror("select failed.");

		//printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		/* Check if we have sockets/STDIN to process */
		if(selret > 0){
		    //cout<<"OK\n";
			/* Loop through socket descriptors to check which ones are ready */
			for(sock_index=0; sock_index<=head_socket; sock_index+=1){
				
				if(FD_ISSET(sock_index, &watch_list)){
					
					/* Check if new command on STDIN */
					if (sock_index == STDIN){
						char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);
						
						memset(cmd, '\0', CMD_SIZE);
						if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
							exit(-1);
						
						printf("\nServer  got the message : %s\n", cmd);

						string str_cmd = string(cmd);
						//str_cmd = str_cmd.substr(0, str_cmd.size()-1);

						vector<string> command_vec;
                        command_vec = get_vector_string(str_cmd);
						//cout<<str_cmd<<"  "<<str_cmd.size()<<"\n";

						if(command_vec[0] == "AUTHOR") {
                            cse4589_print_and_log("[AUTHOR:SUCCESS]\n");
                            cse4589_print_and_log("I, mh267, have read and understood the course academic integrity policy.\n");
							cse4589_print_and_log("I, dyadav2, have read and understood the course academic integrity policy.\n");
							cse4589_print_and_log("I, balasub5, have read and understood the course academic integrity policy.\n");
                            cse4589_print_and_log("[AUTHOR:END]\n");
						}

						else if(command_vec[0]  == "IP") {
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
						else if(command_vec[0]  == "LIST") {
						    // Need to be implemented
						    cse4589_print_and_log("[LIST:SUCCESS]\n");

						    sort(client_list.begin(), client_list.end(), compare);

						    for(int i = 0 ; i < client_list.size(); i++) {
						        client_info cur = client_list[i];
						        //cout<<cur.IP<<"\n";
                                cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i+1, cur.host_name.c_str(), cur.IP.c_str(), atoi(cur.PORT.c_str()));
                            }

						    cse4589_print_and_log("[LIST:END]\n");
						}
						else if(command_vec[0]  == "STATISTICS") {
						    // Need to be implemented
						    cse4589_print_and_log("[STATISTICS:SUCCESS]\n");

						    sort(client_list.begin(), client_list.end(), compare);

						    for(int i = 0 ; i < client_list.size(); i++) {
						        client_info cur = client_list[i];
						        //cout<<cur.IP<<"\n";
						        cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", i+1, cur.host_name.c_str(), cur.num_msg_sent, cur.num_msg_rcv, cur.login_status.c_str());
						    }

						    cse4589_print_and_log("[STATISTICS:END]\n");
						}
						else if(command_vec[0]  == "BLOCKED") {
						    // Need to be implemented
						    string blocker_ip = command_vec[1];
						    for(int i = 0 ; i < client_list.size(); i++) {
                                client_info cur = client_list[i];
                                if (blocker_ip != cur.IP) continue;

                                for (int j =0 ; j<cur.blocked_list.size(); j++) {
                                    struct block_info block_dest;
                                    block_dest = cur.blocked_list[j];
                                    cse4589_print_and_log("[BLOCKED:SUCCESS]\n");
                                    cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",j+1, block_dest.blocked_host_name.c_str(),
                                                            block_dest.blocked_ip.c_str(), atoi(block_dest.blocked_port.c_str()));
                                    cse4589_print_and_log("[BLOCKED:END]\n");
                                }

						    }
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

							//cout<<"Enter else case\n";

                            ///*
                            vector<string> command_vec;

                            string buffer_str = string(buffer);

                            cout<<"Client sent to me BUFFER string ====  "<<buffer_str<<"\n";

                            command_vec = get_vector_string(buffer);
                            //if (command_vec[0] == "LOGIN") {

                            cout<< command_vec[0]<<"\n";
                            if (command_vec[0] == "LOGIN") {
                                 string client_ip = command_vec[1];
                                 string client_port = command_vec[2];
                                 string client_host_name = command_vec[3];
                                 add_new_client(client_ip, client_port, client_host_name, fdaccept, sock_index);

                                 //printf("ECHOing it back to the remote host ... ");
                                // struct
                                 //if(send(sock_index, &client_list, strlen(client_list), 0) == strlen(client_list))
                                 //    printf("Done!\n");
                                 //fflush(stdout);
								char client_data[2048];

								for(int i = 0 ; i < client_list.size(); i++) {
									//cout<<"Inside For";
						        	if(client_list[i].login_status =="logged-in"){
										//cout<<"Inside If";
										strcat(client_data, client_list[i].host_name.c_str());
										strcat(client_data," ");
										strcat(client_data, client_list[i].IP.c_str());
										strcat(client_data," ");
										strcat(client_data, client_list[i].PORT.c_str());
										strcat(client_data,"\n");
									}
								}

								string send_list_to_client = "";
                                 for(int i = 0 ; i < client_list.size(); i++) {
                                     if(client_list[i].login_status =="logged-in"){
                                        client_info cur = client_list[i];
                                        send_list_to_client += cur.IP + " ";
                                        send_list_to_client += cur.PORT + " ";
                                        send_list_to_client += cur.host_name + "\n";
                                     }
                                 }

								cout<<"Client list in server \n"<<send_list_to_client<<"\n";
								char* client_list_data = (char*) send_list_to_client.c_str();

								if(send(sock_index, client_list_data, strlen(client_list_data), 0) == strlen(client_list_data))
                                     printf("Done SENDING THE DAMN LIST!\n");
                                 fflush(stdout);


                                 for(int i = 0 ; i < client_list.size(); i++) {

                                    client_info cur = client_list[i];
                                    if (client_ip != cur.IP) continue;
                                    string send_buffer_msg_to_client = "";

                                    while(!client_list[i].buffer_msg.empty()){
                                        struct buffer_info cur_buf_msg = client_list[i].buffer_msg.front();
                                        client_list[i].buffer_msg.pop();
                                        //send_list_to_client += "BUFFER_IP ";
                                        send_buffer_msg_to_client += "EVENT ";
                                        send_buffer_msg_to_client += cur_buf_msg.sender_ip + " ";
                                        send_buffer_msg_to_client += cur_buf_msg.sender_msg + " ";

                                        cse4589_print_and_log("[%s:SUCCESS]\n", "RELAYED");
                                        cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", cur_buf_msg.sender_ip.c_str(),
                                                                client_ip.c_str(), cur_buf_msg.sender_msg.c_str());
                                        cse4589_print_and_log("[%s:END]\n", "RELAYED");
										client_list[i].num_msg_rcv += 1;

                                    }
                                    char * client_list_data = (char*) send_buffer_msg_to_client.c_str();

                                    if(send_buffer_msg_to_client.size()>0)
                                    if(send(sock_index, client_list_data, strlen(client_list_data), 0) == strlen(client_list_data))
                                         printf("Sending msg from Buffer done-->> %s\n", client_list_data);

                                 }

								//cout<<client_data;
                            }

                            else if (command_vec[0] == "SEND") {
                                cout<<"destination IP in server =  "<<command_vec[1]<< "  "<<command_vec[2]<<" "<<command_vec[3]<<"\n";
                                client_info dest_client = get_client_info(command_vec[2]);

                                string sender_client = command_vec[1];
                                string receiver_client = command_vec[2];
                                string sender_msg = command_vec[3];

								int block_flag = 0;


                                if(dest_client.IP == "None") cout<<"Destination IP not in the list\n";
                                else {
									cout<<"Dest IP found\n";
									//Logic to check if destination client is blocked
									struct client_info destination = get_client_info(receiver_client);
									vector<block_info>::iterator b;
									for (b = destination.blocked_list.begin(); b!=destination.blocked_list.end(); ++b){
										if(b->blocked_ip == sender_client){
											block_flag = 1;
										}
									}
									if(block_flag == 0){
										//Logic for updating STATS for Sending Client
									for(int i = 0 ; i < client_list.size(); i++) {
										if(client_list[i].IP == sender_client){
											client_list[i].num_msg_sent += 1;
										}
									}
									
                                    if(dest_client.login_status == "logged-out") {
                                        cout<<"Client logged out\n";
                                        for(int i = 0 ; i < client_list.size(); i++) {
                                            client_info cur = client_list[i];
                                            if (receiver_client != cur.IP) continue;
                                            struct buffer_info buffer_msg;
                                            buffer_msg.sender_ip = sender_client;
                                            buffer_msg.sender_msg = sender_msg;

                                            client_list[i].buffer_msg.push(buffer_msg);

                                        }
                                    }
                                    else {
                                        string msg_client = "EVENT " + sender_client + " " + sender_msg;
                                        char * msg_to_client = (char*) msg_client.c_str();
                                        if(send(dest_client.fd, msg_to_client, strlen(msg_to_client), 0) == strlen(msg_to_client)) {
                                            //printf("Sending to destination Done! %d %d %d\n", dest_client.fd, fdaccept, sock_index);
                                            cse4589_print_and_log("[%s:SUCCESS]\n", "RELAYED");
                                            cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", sender_client.c_str(), receiver_client.c_str(), sender_msg.c_str());
                                            cse4589_print_and_log("[%s:END]\n", "RELAYED");
                                        }
										//Logic for updating STATS for Receiving Client
										for(int i = 0 ; i < client_list.size(); i++) {
											if(client_list[i].IP == receiver_client){
												client_list[i].num_msg_rcv += 1;
											}
										}
									}

									}
									
                                    

                                }

                            }

                             else if (command_vec[0] == "BROADCAST") {
                                 // Need to be implemented
                             }
                             else if (command_vec[0] == "BLOCK") {
                                 // Need to be implemented
                                 string sender_ip = command_vec[1];

                                 cout<<"In block server "<<command_vec[0]<<" "<<command_vec[1]<<" "<<command_vec[2]<<"\n";

                                 string blocked_ip = command_vec[2];
                                 string blocked_port = command_vec[3];

                                 string blocked_host_name = get_host_name(blocked_ip);

                                 for(int i = 0 ; i < client_list.size(); i++) {
                                     client_info cur = client_list[i];
                                     if (sender_ip != cur.IP) continue;

                                     struct block_info block_dest;
                                     block_dest.blocked_ip = blocked_ip;
                                     block_dest.blocked_host_name = blocked_host_name;
                                     block_dest.blocked_port = blocked_port;
                                     client_list[i].blocked_list.push_back(block_dest);

                                     cout<<"In blocked  "<< block_dest.blocked_ip <<" "<<block_dest.blocked_host_name<<" "<<block_dest.blocked_port<<"\n";
                                 }
                             }

                             else if (command_vec[0] == "UNBLOCK") {
                                 // Need to be implemented
                             }
                             else if (command_vec[0] == "REFRESH") {
                                 // Need to be implemented
                             }

                             else if (command_vec[0] == "LOGOUT") {
                                 // Need to be implemented

                                 string client_ip = command_vec[1];
                                 for(int i = 0 ; i < client_list.size(); i++) {
                                    client_info cur = client_list[i];
                                    if (client_ip != cur.IP) continue;
                                    client_list[i].login_status = "logged-out";
                                 }
                             }

                             else if (command_vec[0] == "LIST") {
                                 // Need to be implemented
                             }
                             else if (command_vec[0] == "EXIT") {
                                 // Need to be implemented
                             }

                             //if(send(sock_index, buffer, strlen(buffer), 0) == strlen(buffer))
                              //       printf("common sending to client Done!\n");
                             fflush(stdout);

							//printf("\nClient sent me: %s\n", buffer);

						}
						
						free(buffer);
					}
				}
			}
		}
	}
	
	return ;
}
