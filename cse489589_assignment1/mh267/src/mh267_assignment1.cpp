/**
 * @mh267_assignment1
 * @author  Mohammad Jakir Hossain <mh267@buffalo.edu>
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
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "../include/client.h"
#include "../include/server.h"

#include "../include/global.h"
#include "../include/logger.h"




using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*
		validate the command line arguments
	*/
	if(argc < 1){
		return -1;
	}
	//printf("argv1 %s \n",argv[1]);
	string runType = "";
	if(strcmp(argv[1],"s") != 0 && strcmp(argv[1],"c") != 0){
		return -1;
	}
	runType = argv[1];
	//CommandParser cp(runType);


	/*Init. Logger*/
	//string ip = get_ip();

	//cout<<"Ip address from my code = "<<ip<<"\n";
	cse4589_init_log(argv[2]);

	if(strcmp(argv[1],"s") == 0) {
	    server_main(2, argv[2]);
	}
	else {
	    client_main(3, "127.0.0.1", argv[2]);
	}

	cout<<"OK done\n";
	//cp.runShell();

	/* Clear LOGFILE*/
    //fclose(fopen(LOGFILE, "w"));
	//cse4589_print_and_log("Running in mode %s\n",cp.getRunType().c_str());
	/*Start Here*/
	
	return 0;
}
