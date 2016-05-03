// Filename: server.cpp
// Author: Erick Narvaez

#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <errno.h>		// Needed to display error messages
#include <stdlib.h>		
#include <fstream>
#include <vector>
#include <string>


using namespace std;
struct addrinfo *host_info_list;	// Pointer to linked list of host_infos
int socketfd;						// Socket File descriptor for initial connection
int new_sd;							// New Socket File descriptor for new accept call
vector<char> memblockMain;			// Main vector where file will be stored
int fragmentationSize;				// Size of the packets
int totalSize;						// Total size of the file
string fileName;

void setupSocket(const char *port){
	int status;
    struct addrinfo host_info;			// Struct that gets filled up with data by getaddrinfo()
	

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
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socketfd == -1)  
		cout << "Socket error: " << strerror(errno) ;

    cout << "Binding socket..."  << endl;
   // Now binding socket which assigns address to socket
    int yes = 1;
    status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) 
		cout << "Bind error: " << strerror(errno);
	
	// Waits for connection
    cout << "Listening for connections..."  << endl;
    status =  listen(socketfd, 5);
    if (status == -1)  
		cout << "Listen error: " << strerror(errno);
	
    // Connected! Now accept connection
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    new_sd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
        cout << "Listen error" << strerror(errno);
    else
        cout << "Connection accepted. Using new socketfd : "  <<  new_sd << endl;
    cout << "Waiting to recieve data..."  << endl;
}
void acceptMessage(){
    ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(new_sd, incomming_data_buffer,1000, 0);
    
    if (bytes_recieved == 0) 
		cout << "Host shut down." << endl;
    if (bytes_recieved == -1)
		cout << "recieve error!" << endl ;
    
    incomming_data_buffer[bytes_recieved] = '\0';
    
	int i = atoi(incomming_data_buffer);
	
	vector<char> memblock(i);
		
	bytes_recieved = recv(new_sd,&memblock[0],i,0);
	cout << bytes_recieved << " bytes recieved..." << endl;
	
	//Append the received data to the main vector
	memblockMain.insert(memblockMain.end(), memblock.begin(), memblock.end());
}

// Priliminary exchange the size of the file being sent to server so we can 
// know how much to expect.
void setMemSize(){
	ssize_t bytes_recieved;
    char incomming_data_buffer[100];
    bytes_recieved = recv(new_sd, incomming_data_buffer,100, 0);
    
    if (bytes_recieved == 0) 
		cout << "Host shut down." << endl;
    if (bytes_recieved == -1)
		cout << "recieve error!" << endl ;
   
    incomming_data_buffer[bytes_recieved] = '\0';
    //cout << incomming_data_buffer << endl;
	totalSize = atoi(incomming_data_buffer);
	//cout << totalSize << " is the total size of the incoming file..." << endl;
	
	//cout << memblockMain.size() << " is the size of the memblockMain" << endl;
	
	char incomming_data_buffer_s[100];
	bytes_recieved = recv(new_sd, incomming_data_buffer_s,100, 0);
	fragmentationSize = atoi(incomming_data_buffer_s);
	cout << "Packet Size: " << fragmentationSize << endl;
	char incomming_data_buffer_f[100];
	bytes_recieved = recv(new_sd, incomming_data_buffer_f, 100, 0);
	incomming_data_buffer_f[bytes_recieved] = '\0';
	fileName = incomming_data_buffer_f;
	cout << "The filename is: " << fileName << endl;
	
}

void closeSocket(){
	fileName = "OutputFiles/" + fileName;
	ofstream outputFile (fileName, ofstream::binary);
	outputFile.write(&memblockMain[0],totalSize);
	outputFile.close();
	
	cout << "Stopping server..." << endl;
    freeaddrinfo(host_info_list);
}

int main(){   
	const char *port = "5555";
	setupSocket(port);
	setMemSize();
	//
	int repeat = totalSize/fragmentationSize + 1;
	for (int i = 0; i < repeat; i++){
		acceptMessage();
	}
	closeSocket();
	return 0 ;
}
