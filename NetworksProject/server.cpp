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

using namespace std;
struct addrinfo *host_info_list;	//Pointer to linked list of host_infos
int socketfd;
int new_sd;
vector<char> memblockMain;
bool accessed = false;
int fragmentationSize;
int totalSize;
//ofstream outputFile ("outputTest.jpg", ofstream::binary|ofstream::app);
//const int buff = 50;

void setupSocket(const char *port){
	int status;
    struct addrinfo host_info;			//Struct that gets filled up with data by getaddrinfo()
	

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
   
    int yes = 1;
    status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) 
		cout << "Bind error: " << strerror(errno);
	
    cout << "Listening for connections..."  << endl;
    status =  listen(socketfd, 5);
    if (status == -1)  
		cout << "Listen error: " << strerror(errno);
	
    
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    new_sd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
        cout << "Listen error" << strerror(errno);
    else
        cout << "Connection accepted. Using new socketfd : "  <<  new_sd << endl;
}
void acceptMessage(){
	//cout << "Waiting to recieve data..."  << endl;
	
    ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(new_sd, incomming_data_buffer,1000, 0);
    cout << incomming_data_buffer << " bytes gonna be received" << endl;
    if (bytes_recieved == 0) 
		cout << "Host shut down." << endl;
    if (bytes_recieved == -1)
		cout << "recieve error!" << endl ;
    
    incomming_data_buffer[bytes_recieved] = '\0';
    //cout << incomming_data_buffer << endl;
	int i = atoi(incomming_data_buffer);
	cout << i << " is the other value" << endl;
	vector<char> memblock(i);
		
	bytes_recieved = recv(new_sd,&memblock[0],i,0);
	//vector<char>::iterator it = memblock.end();

	cout << bytes_recieved << " bytes recieved :" << endl;
	cout << memblock.size() << " is the size of the memblock" << endl;
	cout << &memblock[0] << endl;
	//outputFile.write((char*)&memblock,memblock.size());
	//~ vector<char> temp;
	//~ temp.reserve(memblockMain.size()+ memblock.size());
	//~ temp.insert(temp.end(), memblockMain.begin(), memblockMain.end() );
	//~ temp.insert(temp.end(), memblock.begin(), memblock.end() );
	
	//memblockMain = temp;
	memblockMain.insert(memblockMain.end(), memblock.begin(), memblock.end());
	cout << "WOOOOOOO" << endl;
	//move(memblock.begin(), memblock.end(), back_inserter(memblockMain));	
	//memblock.erase(memblock.begin(), memblock.end());
}

void setMemSize(){
	ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(new_sd, incomming_data_buffer,1000, 0);
    
    if (bytes_recieved == 0) 
		cout << "Host shut down." << endl;
    if (bytes_recieved == -1)
		cout << "recieve error!" << endl ;
    //cout << bytes_recieved << " bytes recieved :" << endl ;
    incomming_data_buffer[bytes_recieved] = '\0';
    cout << incomming_data_buffer << endl;
	totalSize = atoi(incomming_data_buffer);
	cout << totalSize << " TOTAL SIZE" << endl;
	//cout << "Size of allocated array: " << totalSize << endl;
	//memblockMain.resize(totalSize); 			//= new char [totalSize];
	cout << memblockMain.size() << " is the size of the memblockMain" << endl;
	//~ if (memblockMain == nullptr)
		//~ cout << "WTF" << endl;
	char incomming_data_buffer_s[100];
	bytes_recieved = recv(new_sd, incomming_data_buffer_s,100, 0);
	fragmentationSize = atoi(incomming_data_buffer_s);
	//cout << "Packet Size: " << fragmentationSize << endl;
}

void closeSocket(){
	//cout << memblockMain << "WHAT" << endl;
	//~ ofstream myfile;
	//~ myfile.open ("serverOutput.jpg", ios::out | ios::binary| ios::ate);
	//~ myfile << memblockMain;
	ofstream outputFile ("outputTest.jpg", ofstream::binary);
	outputFile.write(&memblockMain[0],totalSize);
	outputFile.close();
	
	
	//myfile.close();
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
