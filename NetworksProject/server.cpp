// Filename: server.cpp
// Author: Erick Narvaez

#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <errno.h>		// Needed to display error messages
#include <stdlib.h>

using namespace std;
void setupSocket(const char *port){
	int status;
    struct addrinfo host_info;			//Struct that gets filled up with data by getaddrinfo()
	struct addrinfo *host_info_list;	//Pointer to linked list of host_infos

    memset(&host_info, 0, sizeof host_info);

	cout << "Setting up structs..."  << endl;

    host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
    host_info.ai_flags = AI_PASSIVE;     // IP Wildcard

    //Going to open a socket on port 5555
    status = getaddrinfo(NULL, port, &host_info, &host_info_list);
    
    if (status != 0)  
		cout << "getaddrinfo error" << gai_strerror(status) ;

    cout << "Creating a socket..."  << endl;
    int socketfd ; // The socket descripter
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socketfd == -1)  
		cout << "Socket error: " << strerror(errno) ;

    cout << "Binding socket..."  << endl;
   
    int yes = 1;
    status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) 
		cout << "Bind error: " << strerror(errno);
	
    cout << "Listening for connections..."  << endl;
    status =  listen(socketfd, 5);
    if (status == -1)  
		cout << "Listen error: " << strerror(errno);
	
    int new_sd;
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    new_sd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
        cout << "Listen error" << strerror(errno);
    else
        cout << "Connection accepted. Using new socketfd : "  <<  new_sd << endl;

    cout << "Waiting to recieve data..."  << endl;
	
	
    ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(new_sd, incomming_data_buffer,1000, 0);
    
    if (bytes_recieved == 0) 
		cout << "Host shut down." << endl;
    if (bytes_recieved == -1)
		cout << "recieve error!" << endl ;
    cout << bytes_recieved << " bytes recieved :" << endl ;
    incomming_data_buffer[bytes_recieved] = '\0';
    cout << incomming_data_buffer << endl;
	int i = atoi(incomming_data_buffer);
	char * memblock;
	cout << "Size of allocated array: " << i << endl;
	memblock = new char [i];
	
    cout << "send()ing back a message..."  << endl;
    const char *msg = "Got it. Dynamically allocated space.";
    int len;
    ssize_t bytes_sent;
    len = strlen(msg);
    //bytes_sent = send(new_sd, msg, len, 0);
	
	bytes_recieved = recv(new_sd, memblock,i,0);
	cout << memblock << endl;
	
    cout << "Stopping server..." << endl;
    freeaddrinfo(host_info_list);
}


int main(){   
	const char *port = "5555";
	setupSocket(port);
	return 0 ;
}
