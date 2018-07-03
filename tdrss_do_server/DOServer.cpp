#include "Do_Table.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <stdio.h>
#include <cstring>
#include <stdexcept>
#include <iterator>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fstream>
#include <inttypes.h>
#include "crc22.h"

#define PG 0x5079AB  /* Polinomio generatre CRC22 x22 + x20 + x14 + x13 + x12 + x11 + x8 + x7 + x5 + x3 + x + 1 */

using namespace std;

DO_Table* do_table;
int server_fd, new_socket, byteSent;
FILE *file;
int PORTCVI;
ifstream in;
int pox = 0;
int len = 42;
char *buffer = new char [len];
char *pre_buffer = new char [18+len+540];
char *crc_buffer = new char [594];
static uint32_t table[256];
typedef uint8_t crc;

uint32_t
rc_crc22(uint32_t crc, const char *buf, size_t len)
{
	static uint32_t table[256];
	static int have_table = 0;
	uint32_t rem;
	uint8_t octet;
	int i, j;
	const char *p, *q;

	/* This check is not thread safe; there is no mutex. */
	if (have_table == 0) {
		/* Calculate CRC table. */
		for (i = 0; i < 256; i++) {
			rem = i;  /* remainder from polynomial division */
			for (j = 0; j < 8; j++) {
				if (rem & 1) {
					rem >>= 1;
					rem ^= PG;
				} else
					rem >>= 1;
			}
			table[i] = rem;
		}
		have_table = 1;
	}

	crc = ~crc;
	q = buf + len;
	for (p = buf; p < q; p++) {
		octet = *p;  /* Cast to unsigned octet. */
		crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
	}
	return ~crc;
}

struct arg_struct {
    int args_th_read_vdi;
    int args_th_write_do;
};



int readFromFileBinary(){

	if (in.eof()) return 0;
    	in.seekg (0, ios::end);
	pox+=len;
	in.seekg (-pox, ios::end);

	in.read (buffer, len);

	pre_buffer[0]=0x62;
	pre_buffer[1]=0x76;
	pre_buffer[2]=0x27;
	pre_buffer[3]=0xE2;
	pre_buffer[4]=0x10;
	pre_buffer[5]=0x0E;
	pre_buffer[6]=0x01;
	pre_buffer[7]=0x01;
	pre_buffer[8]=0xE2;
	pre_buffer[9]=0x01;
	pre_buffer[10]=0x02;
	pre_buffer[11]=0x84;
	pre_buffer[12]=0xFF;
	pre_buffer[13]=0xFF;
	pre_buffer[14]=0xFF;
	pre_buffer[15]=0xFF;
	pre_buffer[16]=0xFF;
	pre_buffer[17]=0xFF;

	for (int i = 0;i<42;i++){
	   pre_buffer[i+18]=buffer[i];
	}

	pre_buffer[596]=0xFF;

        for (int i = 0;i<14;i++){
	   crc_buffer[i]=pre_buffer[i+3];
	}

        for (int i = 14;i<594;i++){
	   crc_buffer[i]=pre_buffer[i+3];
	}
	//uint32_t crc22 = rc_crc22(0, crc_buffer, 594);
        //cout << "++++" << hex << crc22 << endl;
	crc32::generate_table(table);
	uint32_t crc22 = crc32::update(table,0,buffer, 594);
	//cout << "++++" << hex << crc22 << endl;
	int elem = crc22 >> 16 & 0x0000FF;
        //cout << "++++ elem" << elem << endl;
        elem = elem | 0xC0;
	pre_buffer[597]=elem;
        //cout << "++++ elem + C0 > " << elem << endl;
	pre_buffer[598]=crc22 >> 8 & 0x0000FF;
	pre_buffer[599]=crc22 & 0x0000FF;


	return 1;

}

void *socket_read_cvi(void *arguments){

	struct sockaddr_in clientAddr;
	int BytesSent;
	int sock = 0;

	int server_fd, new_socket, byteSent;
	struct sockaddr_in address;

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
	address.sin_port = htons ( PORTCVI );

	// Forcefully attaching socket to the port PORTCVI
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
	unsigned char *pc = reinterpret_cast<unsigned char *>(pre_buffer);
	unsigned char *pcrc = reinterpret_cast<unsigned char *>(crc_buffer);
    	while(1){
		int noEOF = readFromFileBinary();
		if (!noEOF) {
			cout << "File Finished." << endl;
			return 0;
		}
		byteSent = send(new_socket , pre_buffer, 18+len+540 , MSG_NOSIGNAL);
		//cout << "DOServer Message sent (0x" << byteSent << " Bytes)> ";
		if (byteSent==0x258){
		cout.setf (ios_base::hex, ios_base::basefield);
		cout.fill('0');
		for (int i = 0; i < 18+len+540; i++){
		if (i % 16 == 0)
		cout << '\n';
		cout.width(1);
		cout << static_cast<int>(pc[i]);
		}
		cout << '\n';
		}
	    	usleep(100*1000);
    	}

}

int main(int argc, char *argv[]) {

    struct arg_struct args;
    if (argv[0] != NULL)
       PORTCVI=atoi(argv[1]);
    char *filePath;
    if (argv[2] != NULL)
       filePath=argv[2];
    else{
       std::cout << "Insert second parameters!!!" <<  std::endl;
       in.close();
       close(server_fd);
       return 0;
      }
    args.args_th_read_vdi = 5;
    args.args_th_write_do = 7;
    do_table = new DO_Table();
    std::cout << "TDRSS DO Server (SIM)> Open FIle> " << filePath <<  std::endl;
    // /home/bssuser/TM_KIT_DRIVERS/DO_SERVER/sortie_dodo_v608.dat
    //in.open("test_files/sortie_dodo_v608.dat", ios::binary);
    in.open(filePath, ios::binary);


    readFromFileBinary();
    // Prints welcome message...
    std::cout << "TDRSS DO Server (SIM). On port> " << PORTCVI << std::endl;
    pthread_t th_socket_read_cvi;
    pthread_t th_socket_extractor;
    pthread_attr_t th_socket_extractor_attr;
    pthread_attr_t th_socket_read_cvi_attr;
    pthread_attr_init(&th_socket_read_cvi_attr);
    pthread_attr_init(&th_socket_extractor_attr);
    // Constructs the new thread and runs it. Does not block execution.
    int error_thread_cvi = pthread_create(&th_socket_read_cvi, &th_socket_read_cvi_attr, &socket_read_cvi, &args);
    if (error_thread_cvi==0)
    	std::cout << "Thread read from CVI: ON.\n";
    else
	std::cout << "Thread read from CVI: ERROR.\n";

    std::cin.get();
    in.close();
    close(server_fd);
    return 0;
}
