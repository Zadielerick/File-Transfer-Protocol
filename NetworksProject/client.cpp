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
#include <unistd.h>
#include <vector>

using namespace std;
struct addrinfo *host_info_list;
int socketfd;
void setupSocket(const char *ip, const char *port){
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
	socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
	if (socketfd == -1) 
		cout << "Socket error: " << strerror(errno);
	
	cout << "Connecting..." << endl;
	status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if (status == -1)
		cout << "Connect error: " << strerror(errno);
}

void sendMessage(vector<char>& msg){
	int len;
	ssize_t bytes_sent;
	

	len = msg.size();
	string temp = to_string(len);
	const char * msglen = temp.c_str();
	int len2 = strlen(msglen);
	cout << "Sending length: " << msglen << endl;
	send(socketfd,msglen,len2,0);			// Client sends length info to server
	
	bytes_sent = send(socketfd, &msg[0], len, 0);
	if(bytes_sent == 0) 
		cout << "Host shutdown" << endl;
	if(bytes_sent == -1)
		cout << "Receive Error: " << strerror(errno);
	cout << bytes_sent << " bytes sent... " << endl;
}

void closeSocket(){
	cout << "Receiving complete. Closing socket..." << endl;
	freeaddrinfo(host_info_list);
}

void sendSize(int sizeM, int packetSize, string fileName){
	string temp = to_string(sizeM);
	const char * msglen = temp.c_str();
	int len2 = strlen(msglen);
	send(socketfd,msglen,len2,0);
	temp = to_string(packetSize);
	const char * msglen2 = temp.c_str();
	len2 = strlen(msglen);
	send(socketfd,msglen2,len2,0);
	len2 = fileName.length();
	send(socketfd,fileName.c_str(),len2,0);
}

int main(int argc, char* argv[]) {
	// Gets File and checks that it opened successfully
	const int packetSize = 300;
	string fileName;
	if (argc > 1){
		fileName = argv[1];
	}
	else{
		fileName = "client.cpp";
	}
	//cout << argc <<  " is the number of arguments" << endl;
	cout << fileName << " is the fileName" << endl;
	ifstream inputFile(fileName, ifstream::binary);
	if(!(inputFile.is_open())){
		cout << "Unable to open the file!" << endl;
		exit(EXIT_FAILURE);
	}
	cout << "File opened!" << endl;
	streampos size;
	inputFile.seekg(0,inputFile.end);
	size = inputFile.tellg();
	inputFile.seekg(0, ios::beg);
    vector<char> memblock(size);
    //cout << memblock.size() << endl;

    inputFile.read(&memblock[0], size);

    //cout << inputFile.gcount() << " characters read";
    inputFile.close();
	
	const char *ip = "10.0.2.15";
	const char *port = "5555";
	
	setupSocket(ip, port);
	sendSize(size, packetSize, fileName);
	
	//Here split up into packets and send indiv
	int repeat = size/packetSize;
	//int counter = 0;
	
	
	
	if(repeat > 0){
		vector<char>::iterator first = memblock.begin();
		vector<char>::iterator last = memblock.begin() + packetSize;
		
		for(int i = 0; i < repeat; i++){
		vector<char> newVec(first, last);
		sendMessage(newVec);
		first = first + packetSize;
		last = last + packetSize;
		}
	
		int remainder = size%packetSize;
		if(remainder){
			first = first + packetSize;
			last = last + remainder;
			vector<char> newVec(first, last);
			sendMessage(newVec);
		}
	}
	else {
		vector<char>::iterator first = memblock.begin();
		vector<char>::iterator last = memblock.begin() + size;
		vector<char> newVec(first, last);
		sendMessage(newVec);
	}
	
	
	
	//cout << "Counter = " << counter << endl;
	closeSocket();
	return 0;
}
