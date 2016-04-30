// Filename: client.cpp
// Author: Erick Narvaez

#include <iostream> //Input/Output from console
#include <cstring>			// Needed for memset()
#include <string>			// Needed for strings used
#include <sys/socket.h>		// Needed for socket functions
#include <netdb.h>			// Needed for socket functions
#include <errno.h>			// Needed to display error messages
#include <fstream> 			// Needed to input files to send
#include <stdlib.h>			// Needed for exit, EXIT_FAILURE

using namespace std;
struct addrinfo *host_info_list;
int setupSocket(const char *ip, const char *port){
	int status;
	struct addrinfo host_info;

	memset(&host_info, 0, sizeof host_info);
	cout << "Setting up structs..." << endl;

	host_info.ai_family = AF_UNSPEC;		//Both IPv4 and IPv6 work
	host_info.ai_socktype = SOCK_STREAM;	//SOCK_STREAM=TCP , SOCK_DGRAM=UDP
	
	// Fill up list of host_infos with address information
	status = getaddrinfo(ip, port, &host_info, &host_info_list);
	if(status!=0)
		cout << "getaddrinfo error " << gai_strerror(status);
	
	cout << "Creating a socket..." << endl;
	int socketfd;
	socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
	if (socketfd == -1) 
		cout << "Socket error: " << strerror(errno);
	
	cout << "Connecting..." << endl;
	status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if (status == -1)
		cout << "Connect error: " << strerror(errno);
	return socketfd;
}

void sendMessage(int socketfd, char * msg){
	int len;
	ssize_t bytes_sent;
	ssize_t bytes_recieved;
	char incoming_data_buffer[1000];
	char incoming_data_buffer_s[1000];

	len = strlen(msg);
	string temp = to_string(len);
	const char * msglen = temp.c_str();
	int len2 = strlen(msglen);
	cout << "Sending length" << endl;
	send(socketfd,msglen,len2,0);			// Client sends length info to server
	cout<< "Sending message..." << endl;
	
	//bytes_recieved = recv(socketfd,incoming_data_buffer_s, 1000, 0);	//Client waits for server confirmation
	//cout << incoming_data_buffer_s << endl;

	bytes_sent = send(socketfd, msg, len, 0);
	cout << bytes_sent << " bytes sent : " << endl;
	
	//cout << "Waiting to receive data..." << endl;
	
	//bytes_recieved = recv(socketfd, incoming_data_buffer, 1000, 0);
	
	//~ if(bytes_recieved == 0) 
		//~ cout << "Host shutdown" << endl;
	//~ if(bytes_recieved == -1)
		//~ cout << "Receive Error: " << strerror(errno);
	//~ cout << bytes_recieved << " bytes received : " << endl;
	//~ incoming_data_buffer[bytes_recieved] = '\0';
	//cout << incoming_data_buffer << endl;
}

void closeSocket(){
	cout << "Receiving complete. Closing socket..." << endl;
	freeaddrinfo(host_info_list);
}
int main() {
	// Gets File and checks that it opened successfully
	ifstream inputFile("client.cpp", ios::in|ios::binary|ios::ate);
	if(!(inputFile.is_open())){
		cout << "Unable to open the file!" << endl;
		exit(EXIT_FAILURE);
	}
	cout << "File opened!" << endl;
	streampos size;
	char * memblock;
	size = inputFile.tellg();
    memblock = new char [size];
    
	inputFile.seekg (0, ios::beg);
    inputFile.read (memblock, size);
    inputFile.close();
	
	const char *ip = "10.0.2.15";
	const char *port = "5555";
	
	int socketfd = setupSocket(ip, port);
	sendMessage(socketfd, memblock);
	closeSocket();
	return 0;
}
