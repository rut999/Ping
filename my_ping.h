/**************************************************
* Author :: Rutvik Parvataneni
* Email :: parvatanenirutvik@gmail.com
*
* Implemented program in C to ping specified address
****************************************************
*/

#ifndef MY_HEADER
#define MY_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <netinet/ip_icmp.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <setjmp.h>
#include <stdbool.h>
 
 
#define MAXIMUM_TOKENS 8 

//ping packet size 
#define SIZE_PACKET     4096
#define MAXIMUM_WAIT_TIME   10

 
char sendpacket[SIZE_PACKET]; 
char receive_packet[SIZE_PACKET];
 
int sockfd; 
int data_length = 56; 
int no_sent = 0;
int no_received = 0;

struct sockaddr_in destination_address;
 
pid_t pid;
 
//using this with the help of netinet/in.h
struct sockaddr_in from;
 
// using it with the help of sys/time.h
struct timeval time_value;
 
int sent_pckg_count = 1;
 
struct my_struct *parse_sequence();


#endif