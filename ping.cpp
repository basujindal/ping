/*
 *    This is a ping imitation program 
 *    It will send an ICMP ECHO packet to the server of 
 *    your choice and listen for an ICMP REPLY packet.
 *    Usage sudo ./ping DESTINATION_IP
 *    eg. sudo ./ping 8.8.8.8
 *    This program requires root access due to SOCK_RAW
 *    If the IP address is not reachable, the program will timeout after 4 seconds 
 */
#include <stdio.h>
#include <arpa/inet.h>
#include <linux/icmp.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <iostream>
#include <chrono> 
 
using namespace std;

// function to calculate ICMP packet checksum
unsigned short in_cksum(unsigned short *addr, int len)
{
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;
    /*
     * The algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
    /* mop up an odd byte, if necessary */
    if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }
    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);       /* add hi 16 to low 16 */
    sum += (sum >> 16);               /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return (answer);
}


int main(int argc, char** argv)
{

    if(argc == 1){
      cout << "Please enter the destination IP address." << endl;
      return 0;
    }
    else if(argc > 2){
      cout << "Too many arguments! Please enter the destination IP address only." << endl;
      return 0;
    }
    else{
      // checking if input IP address is valid
      struct sockaddr_in sa;
      int result = inet_pton(AF_INET, argv[1], &(sa.sin_addr));
      if(result == 0){
        cout << "Bad hostname" << endl;
        return 0;
      } 
    }

    unsigned short in_cksum(unsigned short *, int);
    char dst_addr[20];
    strcpy(dst_addr,argv[1]);
    struct iphdr* ip_reply;
    struct icmphdr* icmp;
    struct sockaddr_in connection;
    char* buffer;
    int sockfd, addrlen, siz;
     
    
    // allocate all necessary memory
    icmp = (struct icmphdr*)malloc(sizeof(struct icmphdr));
    buffer = (char*)malloc(sizeof(struct icmphdr));

    // using SOCK_RAW because ICMP is different from both UDP or TCP
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
     
    /*
     *  here the icmp packet is created
     *  also the ip checksum is generated
     */
    icmp->type           = ICMP_ECHO;
    icmp->code           = 0;
    icmp->un.echo.id     = random();
    icmp->un.echo.sequence   = 0;
    icmp-> checksum      = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr));
     
    connection.sin_family = AF_INET;
    connection.sin_addr.s_addr = inet_addr(dst_addr);
     
    //  now the packet is sent
    auto start = chrono::high_resolution_clock::now(); 
    sendto(sockfd, (char*)icmp, sizeof(struct icmphdr), 0, (struct sockaddr *)&connection, sizeof(struct sockaddr));
    
    //  now we listen for responses with 4 sec timeout
    addrlen = sizeof(connection);
    struct timeval read_timeout;
    read_timeout.tv_sec = 4;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
    siz = recvfrom(sockfd, buffer, sizeof(struct icmphdr), 0, (struct sockaddr *)&connection, (socklen_t*)&addrlen) ;
      
    cout << siz <<  endl;
    if(siz == -1){
        cout << "Request timed out or host unreacheable" << endl;
        return 0;

      }
    auto stop = chrono::high_resolution_clock::now(); 
    float duration = chrono::duration_cast<chrono::microseconds>(stop - start).count(); 
    cout << "Reply from " << dst_addr << " RTT = " << duration/1000 << " ms" << endl; 
 
    free(buffer);
    close(sockfd);
    return 0;
}