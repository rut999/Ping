/*****************************************************
* Author :: Rutvik Parvataneni
* Email :: parvatanenirutvik@gmail.com
*
* Implemented program in C to ping specified address
* Referred :: https://www.geeksforgeeks.org/ping-in-c/
*******************************************************
*/
 
#include "my_ping.h"

 
/****************************************************************************
* Compute the Internet Checksum for count bytes beginning at location address
* @param --> address
* @param --> length
* @return
* Referred :: //https://www.csee.usf.edu/~kchriste/tools/checksum.c
*****************************************************************************
*/
unsigned short checksum(unsigned short *address, int length) 
{
   int count = length;
   int sum = 0;
   unsigned short result = 0;
 
   // Here the main summing takes place
   while (count > 1) 
   {
       sum += *address++;
       count -= 2;
   }
 
   if (count == 1) {
       *(unsigned char *) (&result) = *(unsigned char *) address;
       sum += result;
   }
 
   // Fold 32-bit to 16 bits
   sum = (sum >> 16) + (sum & 0xffff);
   sum += (sum >> 16);
   result = ~sum;
   return result;
}

/*********************************
* Display ICMP Ping request result
* @param --> signal_no
**********************************
*/
void statistics(int signal_no) 
{
   if (no_sent == 0)
   {
       printf("Cannot get statistic because number of sent package is zero");
   }            
   else
   {
       printf("%d --> packets sent, %d --> answers received , %%%d --> lost\n", no_sent,no_received, ((no_sent - no_received) / no_sent) * 100);
   }      
   close(sockfd);
}
 
/*************************************************
* Packet preparation before sending to the server
* @param --> packet_number
* @return
**************************************************
*/
int pack(int packet_number) 
{
   int i;
   int packet_size;
   struct icmp *icmp;
   struct timeval *t_val;
   icmp = (struct icmp *) sendpacket;
   icmp->icmp_type = ICMP_ECHO;
   icmp->icmp_cksum = 0;
   icmp->icmp_code = 0;
   icmp->icmp_id = pid;
   icmp->icmp_seq = packet_number;   
   packet_size = 8 + data_length;
   t_val = (struct timeval *) icmp->icmp_data;
   gettimeofday(t_val, NULL);
   icmp->icmp_cksum = checksum((unsigned short *) icmp, packet_size);
   return packet_size;
}
 
/*****************************************
* Process incoming package from the server
* @param buf
* @param len
* @return
******************************************
*/
int unpack(char *buf, int len) 
{
   int i;
   int ip_hdr_len;
   struct ip *ip;
   struct icmp *icmp;
   struct timeval *tv_send;
   double rtt;
 
   ip = (struct ip *) buf;
   ip_hdr_len = ip->ip_hl << 2;
   icmp = (struct icmp *) (buf + ip_hdr_len);
   len -= ip_hdr_len;
 
   if (len <= 7) 
   {
       printf("ICMP packets\'s length is less than 8\n");
       return -1;
   }
 
   printf("\n--------------------ICMP Ping statistics-------------------\n");
   if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid)) 
   {
       tv_send = (struct timeval *) icmp->icmp_data;
       tv_sub(&time_value, tv_send);
       rtt = time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
       printf("%d byte from %s: rtt=%.3f ms\n", len, inet_ntoa(from.sin_addr), rtt);
   } else
       return -1;
}

/*************************************
* Sending packet request to the server
**************************************
*/
void send_packet()
{
   int packet_size;
 
   while (no_sent < sent_pckg_count) 
   {
       no_sent++;
       packet_size = pack(no_sent);
       if (sendto(sockfd, sendpacket, packet_size, 0, (struct sockaddr *)
               &destination_address, sizeof(destination_address)) < 0) 
        {
           perror("sendto() error");
           continue;
        }
       sleep(1);
   }
}
 
/*****************************************
* Receive incoming package from the server
******************************************
*/
void recv_packet() {
   int n;
   int fromlen;
   extern int errno;
   //alaram to display statistics
   signal(SIGALRM, statistics);
   fromlen = sizeof(from);
   while (no_received < no_sent) 
   {
       alarm(MAXIMUM_WAIT_TIME);
       if ((n = recvfrom(sockfd, receive_packet, sizeof(receive_packet), 0, (struct
               sockaddr *) &from, &fromlen)) < 0) 
        {
           if (errno == EINTR)
               continue;
           printf("recvfrom error, exiting program!");
           exit(1);
        }
       gettimeofday(&time_value, NULL);
       if (unpack(receive_packet, n) == -1)
           continue;
       no_received++;
   }
} 

/*****************************************
* Most of the commands use this structure
* token --> tokens of the command
* count --> number of tokens
* 
******************************************
*/
struct my_struct {
   char *token[MAXIMUM_TOKENS]; 
   int count; 
} cmd_inst;
 
/***************************************
* Check for empty string, space, or tabs
**************************************** 
*/
int is_empty(const char *p) 
{
   if (strcmp(p, "") == 0) 
   {
       return 1;
   }
   while (*p != '\0') 
   {
       if (!isspace((unsigned char) *p))
           return 0;
       p++;
   }
   return 1;
} 
 
/***********************************
* This is where the execution begins
* @param argc
* @param argv
* @return
************************************
*/
main(int argc, char *argv[]) 
{
   struct protoent *protocol;
   int seen_quit = 0;
   if ((protocol = getprotobyname("icmp")) == NULL) 
   {
       perror("getprotobyname");
       exit(1);
   }   
   while (seen_quit == 0) 
   {
       struct hostent *host;
       int size = 50 * 1024;
       unsigned long inaddr = 0l;
       int waittime = MAXIMUM_WAIT_TIME;
       struct my_struct *command;
       
 
       no_sent = 0;
       no_received = 0;
 
       printf("--> ");
       fflush(stdout);
       // Read the input from the user
       char str[256];
       if (fgets(str, 256, stdin) == NULL) 
       {
           printf("\n");
           continue;
       }       
       if (is_empty(str) == 0) 
       {
           command = parse_sequence(str);
       } else
           continue;
 
        //storing the first and second arguments given by the user
       char *first_command = command->token[0];
       char *second_command = command->token[1];
       
 
       if (strcmp(first_command, "exit") == 0) 
       {
           seen_quit = 1;
           continue;
       }
       //Prompt the correct usage to the user if invalid input given
       if (second_command == NULL || (sent_pckg_count = atoi(second_command)) == 0) 
       {
           printf("Invalid command usage: <Host/IP> <Number of Ping>\n");
           continue;
       }
       printf("Number of package sent request: %d\n", sent_pckg_count);
 
       if ((sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0) 
       {
           perror("socket error");
           exit(1);
       }
 
       setuid(getuid());
       setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
       bzero(&destination_address, sizeof(destination_address));
       // DNS reverse lookup
       destination_address.sin_family = AF_INET;
 
       if (inaddr = inet_addr(first_command) == INADDR_NONE) 
       {
           //checking the hostname input given by the user
           if ((host = gethostbyname(first_command)) == NULL) 
           {
               perror("gethostbyname error, Program Exit");
               exit(1);
           }
           memcpy((char *) &destination_address.sin_addr, host->h_addr, host->h_length);
       } else
           destination_address.sin_addr.s_addr = inet_addr(first_command);
       pid = getpid();
       printf("PING %s(%s): %d bytes data in ICMP packets.\n", first_command, inet_ntoa
               (destination_address.sin_addr), data_length);
       send_packet();
       recv_packet();
       statistics(SIGALRM);
       wait(2);
       receive_packet[SIZE_PACKET];
       sendpacket[SIZE_PACKET];
       fflush(stdout);
   }
   return 0;
 
}
 
void tv_sub(struct timeval *out, struct timeval *in) 
{
   if ((out->tv_usec -= in->tv_usec) < 0) 
   {
       --out->tv_sec;
       out->tv_usec += 1000000;
   }
   out->tv_sec -= in->tv_sec;
}
 
/*******************************************************************
* The parse_sequence function 
* - used to parse the char line from standard input
* - into my_struct to pass arguments to sys calls
* @param 
*******************************************************************
*/
struct my_struct *parse_sequence(char *line_inp) {
   int i, t;
   struct my_struct *c = &cmd_inst;
   memset(c, 0, sizeof(struct my_struct));
 
   t = 0;
 
   i = 0;
   while (isspace(line_inp[i]))
       i++;
   c->token[t] = &line_inp[i];
 
   while (line_inp[i] != '\0' && t < MAXIMUM_TOKENS - 1) 
   {
       t++;
 
       while (!isspace(line_inp[i]) && line_inp[i] != '\0')
           i++;
 
       while (isspace(line_inp[i])) 
       {
           line_inp[i] = '\0';
           i++;
       }
 
       c->token[t] = &line_inp[i];
   }
   c->count = t + 1;
   c->token[t] = NULL;
 
   return c;
}
 
 

