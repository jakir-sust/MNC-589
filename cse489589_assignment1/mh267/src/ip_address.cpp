#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <typeinfo>
#include <ip_address.h>
#include <vector>
#include <stdlib.h>



using namespace std;

bool isNumber(const string &str)
{
    // `std::find_first_not_of` searches the string for the first character
    // that does not match any of the characters specified in its arguments
    return !str.empty() &&
        (str.find_first_not_of("[0123456789]") == std::string::npos);
}

int isValidIP(string ipAddress){
    int i = 0;
    vector<string> list;
 
    int pos = ipAddress.find(".");
 
    while (pos != string::npos)
    {
        list.push_back(ipAddress.substr(i, pos - i));
        i = ++pos;
        pos = ipAddress.find(".", pos);
    }
 
    list.push_back(ipAddress.substr(i, ipAddress.length()));

    // if the token size is not equal to four
    if (list.size() != 4) {
        return 0;
    }
 
    // validate each token
    for (int i = 0; i < list.size(); i++){
        // verify that the string is a number or not, and the numbers
        // are in the valid range
        string str = list.at(i);
        if (!isNumber(str) || atoi(str.c_str()) > 255 || atoi(str.c_str()) < 0) {
            return 0;
        }
    }
 
    return 1;

}

int isValidPort(string port){
    if(atoi(port.c_str()) > 0 && atoi(port.c_str())<65535){
        return 1;
    }
    return 0;
}
string  get_ip() {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);

            string arr = string(ifa->ifa_name);

            //std::cout <<"Type ==  "<< typeid(arr).name() << '\n';

            if ("enp2s4" == arr)
            {
                return std::string(addressBuffer);
            }

            //if (strcmp("enp2s4",ifa->ifa_name) == 0) {
              //  return std::string(addressBuffer);
           // }

        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            string arr = string(ifa->ifa_name);
             if ("enp2s4" == arr)
            {
                return std::string(addressBuffer);
            }

        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return 0;
}