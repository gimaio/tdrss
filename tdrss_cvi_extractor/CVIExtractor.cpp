#include "Do_Table.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <sys/socket.h>
#include <stdio.h>
#include <cstring>
#include <stdexcept>
#include <iterator>
#include <sstream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fstream>

#define PORTDO 10012
#define PORTDO_OUT 10005
#define BUFSIZE 1024

using namespace std;

DO_Table* do_table;
fstream file;
int sock = 0;

struct arg_struct {
    int args_th_read_vdi;
    int args_th_write_do;
};

string readFromFileTXT(){
  string buffer;
  file.seekp (0, ios::cur);
  getline (file, buffer);
  return buffer;
}

void *socket_do_server(void *arguments){
    struct sockaddr_in clientAddr, ThisSenderInfo;
    int RetCode;

    // Be careful with the array bound, provide some checking mechanism...
    //char sendbuf[1024] = "This is a test string from sender";
    int BytesSent, nlen;
    int sock = 0;
    do_table->set_blockHeader("DS");
    do_table->set_typeOperation("V000");
    do_table->set_motQualite("0");
    do_table->set_source("B1");
    do_table->set_tempsTU("HHMMSSmsmsms");
    do_table->set_xCoord(9.99999);
    do_table->set_yCoord(9.99999);
    do_table->set_zCoord(9.99999);
    do_table->set_xPCoord(9.99999);
    do_table->set_yPCoord(9.99999);
    do_table->set_zPCoord(9.99999);
    do_table->set_separator(";");
    std::string message = "" + do_table -> to_string();
    //const char *sendbuf = message.c_str();
   int server_fd, new_socket, valread, byteSent;
    struct sockaddr_in address;
    int opt = 1;

    char buffer[1024];
    // Starting Server
    printf("DOServer> Starting Server...\n");

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
	printf("DOServer> Socket failed\n");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    address.sin_port = htons ( PORTDO_OUT );

    // Forcefully attaching socket to the port PORTDO_OUT 10005
    if (int bindResult = bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
	cout << "DOServer> Bind failed (" << bindResult << ")";
        exit(EXIT_FAILURE);
    }
    listen(server_fd, 10);


    // Loop lato SERVER - rimango in attesa di richieste di connessione
    // in caso di connessione OK procedo leggendo dalla socket
socklen_t size = sizeof (clientAddr);
new_socket = accept(server_fd, (struct sockaddr *)&clientAddr, &size);
        if (new_socket<0)
		cout << "DOServer> Error in accept(). Error> " << strerror(errno) << errno << endl;
    while(1){

        const char *sendbuf = readFromFileTXT().c_str();

        byteSent = send(new_socket , sendbuf, strlen(sendbuf) , 0);
        if (byteSent > 0)
		cout << "DOServer> Message >> sent: " << sendbuf << endl;
    	usleep(100*1000);

    }
    //return 0;
}


int extractorDO(char str_line[]){
    char* p;
    p = strtok(str_line," ");
    cout << p[0] << p[1] << endl;
    do_table->set_blockHeader("DS");
    do_table->set_typeOperation("V 000");
    do_table->set_motQualite("0");
    do_table->set_source("B1");
    do_table->set_tempsTU("HHMMSSmsmsms");
    do_table->set_xCoord(9.99999);
    do_table->set_yCoord(9.99999);
    do_table->set_zCoord(9.99999);
    do_table->set_xPCoord(9.99999);
    do_table->set_yPCoord(9.99999);
    do_table->set_zPCoord(9.99999);
    do_table->set_separator(" ");
}

void *socket_extractor(void *arguments){

    // CLIENT SIDE
    struct sockaddr_in ServerAddr, ThisSenderInfo;
    int RetCode;
    int BytesSent, nlen;

    // CNES DO example> DS N 608 1 12:18:46.467 -5577.533 00  3.842444000E+06 -5.057633500E+06  5.782768750E+05  3.311701491E-02 -4.361366481E-02  4.503619391E-03
    // CNES DO example BINARY> DS N 608 1 121846467 00  3.842444000E+06 -5.057633500E+06  5.782768750E+05  3.311701491E-02 -4.361366481E-02  4.503619391E-03

    struct sockaddr_in address;
    int  valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("DOClient> Socket creation error \n");
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );  // localhost
    serv_addr.sin_port = htons ( PORTDO );

    int connect_result = -1;
    while(1){
    const char *sendbuf = readFromFileTXT().c_str();

  	connect_result = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	    if (connect_result >= 0 || errno == 9 || errno == 106){
                    char buffer[1024] = {0};
		    valread = read( sock , buffer, 1024);
		//cout << "DOClient> " << valread << endl;
		unsigned char *pc = reinterpret_cast<unsigned char *>(buffer);
		//cout.setf (ios_base::hex, ios_base::basefield);
		cout.fill('0');
		for (int i = 0; i < 600; i++){
		if (i % 16 == 0)
		//cout << '\n';
		cout.width(1);
                cout << buffer[i];
		//cout << static_cast<int>(pc[i]) << ' ';
		}
		//cout << endl;
                    //if (valread > 0)
		    //	cout << "DOClient> Read> " << buffer << endl;
                    //extractorDO(buffer);
                    //std::string message = "" + do_table -> to_string();
    		    //const char *sendbuf = message.c_str();
		    //int byteSent = write(sock , sendbuf, strlen(sendbuf) );
		    //if (byteSent > 0)
		    //	cout << "DOClient> Message sent: " << sendbuf << endl;
	    } else {
		time_t t = time(NULL);
		tm* timePtr = localtime(&t);
		cout << "DOClient> [" << 1900 + timePtr->tm_year << "/" << 1+timePtr->tm_mon << "/" << timePtr->tm_mday << " " <<
                timePtr->tm_hour << ":" << timePtr->tm_min << ":" << timePtr->tm_sec << "]: Connection Failed (error: " << strerror(errno) << " / " << errno << ")\n";
                return 0;
	}

usleep(100*1000);

    }

    	delete do_table;
	pthread_exit(NULL);
	return NULL;

}

int main(int argc, char**argv) {
    struct arg_struct args;
    args.args_th_read_vdi = 5;
    args.args_th_write_do = 7;
    do_table = new DO_Table();

    // Prints welcome message...
    std::cout << "TDRSS DO Extractor." << std::endl;
    pthread_t th_socket_do_server;
    pthread_t th_socket_extractor;
    pthread_attr_t th_socket_extractor_attr;
    pthread_attr_t th_socket_do_server_attr;
    pthread_attr_init(&th_socket_do_server_attr);
    pthread_attr_init(&th_socket_extractor_attr);
    // Constructs the new thread and runs it. Does not block execution.
    int error_thread_do = pthread_create(&th_socket_extractor, &th_socket_extractor_attr, &socket_extractor, &args);
    int error_thread_do_server = pthread_create(&th_socket_do_server, &th_socket_do_server_attr, &socket_do_server, &args);

    if (error_thread_do==0)
    	std::cout << "Thread read DO (CLIENT): ON.\n";
    else
    	std::cout << "Thread read DO (CLIENT): ERROR.\n";

    if (error_thread_do_server==0)
    	std::cout << "Thread write DO (SERVER): ON.\n";
    else
    	std::cout << "Thread write DO (SERVER): ERROR.\n";

    std::cin.get();
    file.close();
    close (sock);
    return 0;
}
