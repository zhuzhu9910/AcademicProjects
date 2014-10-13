/**************************************************************************
 * simpletun.c                                                            *
 *                                                                        *
 * A simplistic, simple-minded, naive tunnelling program using tun/tap    *
 * interfaces and TCP. Handles (badly) IPv4 for tun, ARP and IPv4 for     *
 * tap. DO NOT USE THIS PROGRAM FOR SERIOUS PURPOSES.                     *
 *                                                                        *
 * You have been warned.                                                  *
 *                                                                        *
 * (C) 2009 Davide Brini.                                                 *
 *                                                                        *
 * DISCLAIMER AND WARNING: this is all work in progress. The code is      *
 * ugly, the algorithms are naive, error checking and input validation    *
 * are very basic, and of course there can be bugs. If that's not enough, *
 * the program has not been thoroughly tested, so it might even fail at   *
 * the few simple things it should be supposed to do right.               *
 * Needless to say, I take no responsibility whatsoever for what the      *
 * program might do. The program has been written mostly for learning     *
 * purposes, and can be used in the hope that is useful, but everything   *
 * is to be taken "as is" and without any kind of warranty, implicit or   *
 * explicit. See the file LICENSE for further details.                    *
 *************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h> 
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>
#include <netinet/ip.h>
 
#include "vpnutilities.h"
#include "servctl.h"

//#define SINGLE_TEST

int debug;
char *progname;
struct Session *slisthead = 0;

/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device. The caller     *
 *            needs to reserve enough space in *dev.                      *
 **************************************************************************/
int tun_alloc(char *dev, int flags) {

  struct ifreq ifr;
  int fd, err;

  if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
    perror("Opening /dev/net/tun");
    return fd;
  }

  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags = flags;

  if (*dev) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
    perror("ioctl(TUNSETIFF)");
    close(fd);
    return err;
  }

  strcpy(dev, ifr.ifr_name);

  return fd;
}

/**************************************************************************
 * cread: read routine that checks for errors and exits if an error is    *
 *        returned.                                                       *
 **************************************************************************/
int cread(int fd, char *buf, int n){
  
  int nread;

  if((nread=read(fd, buf, n))<0){
    perror("Reading data");
    exit(1);
  }
  return nread;
}

/**************************************************************************
 * cwrite: write routine that checks for errors and exits if an error is  *
 *         returned.                                                      *
 **************************************************************************/
int cwrite(int fd, char *buf, int n){
  
  int nwrite;

  if((nwrite=write(fd, buf, n))<0){
    perror("Writing data");
    exit(1);
  }
  return nwrite;
}

/**************************************************************************
 * read_n: ensures we read exactly n bytes, and puts those into "buf".    *
 *         (unless EOF, of course)                                        *
 **************************************************************************/
int read_n(int fd, char *buf, int n) {

  int nread, left = n;

  while(left > 0) {
    if ((nread = cread(fd, buf, left))==0){
      return 0 ;      
    }else {
      left -= nread;
      buf += nread;
    }
  }
  return n;  
}

/**************************************************************************
 * do_debug: prints debugging stuff (doh!)                                *
 **************************************************************************/
void do_debug(char *msg, ...){
  
  va_list argp;
  
  if(debug){
	va_start(argp, msg);
	vfprintf(stderr, msg, argp);
	va_end(argp);
  }
}

/**************************************************************************
 * my_err: prints custom error messages on stderr.                        *
 **************************************************************************/
void my_err(char *msg, ...) {

  va_list argp;
  
  va_start(argp, msg);
  vfprintf(stderr, msg, argp);
  va_end(argp);
}

/**************************************************************************
 * usage: prints usage and exits.                                         *
 **************************************************************************/
void usage(void) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "%s -i <ifacename> [-s|-c <serverIP>] [-p <port>] [-u|-a] [-d]\n", progname);
  fprintf(stderr, "%s -h\n", progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "-i <ifacename>: Name of interface to use (mandatory)\n");
  fprintf(stderr, "-s|-c <serverIP>: run in server mode (-s), or specify server address (-c <serverIP>) (mandatory)\n");
  fprintf(stderr, "-p <port>: port to listen on (if run in server mode) or to connect to (in client mode), default 55555\n");
  fprintf(stderr, "-u|-a: use TUN (-u, default) or TAP (-a)\n");
  fprintf(stderr, "-d: outputs debug information while running\n");
  fprintf(stderr, "-h: prints this help text\n");
  exit(1);
}


/**************************************************************************
 *find proper session for the ip
 **************************************************************************/
struct Session* findsessByDstIP(struct Session* sess, unsigned long dstip)
{

	while(sess != 0){
		if(sess->vip == dstip)
			return sess;
		if(sess->subnet != 0 && (sess->subnet & sess->mask) == (dstip & sess->mask))
			return sess;
		sess = sess->next;
	}
	
	return sess;

}

struct Session* findsessByVPNHeader(struct Session* sess, struct VPNDataHdr* vhd, unsigned long ipaddr)
{
	if(vhd == 0)
		return 0;
	while(sess != 0){
		if(sess->sid == vhd->sid && sess->ip == ipaddr)
			return sess;			
		sess = sess->next;

	}

	return 0;
}


void printHex(unsigned int* seq, int size)
{
	int i = 0;
	for(;i < size; i++){
		printf("%08x",seq[i]);
	}
		
}

void debugSession(struct Session* sess)
{
	if(debug){
		struct in_addr temp, tempm;
		temp.s_addr = sess->ip;
		printf("\nClient IP: %s",inet_ntoa(temp));
		temp.s_addr = sess->vip;
		printf("\nClient Virtual IP Address: %s",inet_ntoa(temp));
		temp.s_addr = sess->subnet;
		tempm.s_addr = sess->mask;
		if(sess->subnet != 0){
			printf("\nClient subnet %s", inet_ntoa(temp));
			printf(", netmask %s",inet_ntoa(tempm));
		}
		printf("\nsession id ");
		printHex((unsigned int *)&(sess->sid),sizeof(long)/sizeof(int));
		printf("\nsession key "); 
		printHex((unsigned int *)(sess->s_key),KEY_LENGTH/sizeof(int));
		printf("\nsession iv ");
		printHex((unsigned int *)(sess->iv),KEY_LENGTH/sizeof(int));
		printf("\n");
	}
}



int main(int argc, char *argv[]) {
  
	int tap_fd, option;
	int flags = IFF_TUN;
	char if_name[IFNAMSIZ] = "";
	int header_len = IP_HDR_LEN;
	int maxfd;
	uint16_t nread, nwrite, plength;
	char buffer[BUFSIZE];
	struct sockaddr_in local, remote;
	char remote_ip[16] = "";
	unsigned short int port = PORT;
	int sock_fd, net_fd, optval = 1;
	socklen_t remotelen;
	int cliserv = -1;    /* must be specified on cmd line */
	unsigned long int tap2net = 0, net2tap = 0;

	int listen_sd;
  	int sd;
  	struct sockaddr_in sa_serv;
	struct sockaddr_in sa_cli;
	size_t client_len;
	int err;
	int yes = 1;

	struct ip *ip_header;            //ip header struct
	struct VPNDataHdr* vpn_header;

	int ptc[2];         //pipe from control process to tunnel process

	progname = argv[0];
  
  /* Check command line options */
  while((option = getopt(argc, argv, "i:sc:p:uahd")) > 0){
    switch(option) {
      case 'd':
        debug = 1;
        break;
      case 'h':
        usage();
        break;
      case 'i':
        strncpy(if_name,optarg,IFNAMSIZ-1);
        break;
      case 's':
        cliserv = SERVER;
        break;
      case 'c':
        cliserv = CLIENT;
        strncpy(remote_ip,optarg,15);
        break;
      case 'p':
        port = atoi(optarg);
        break;
      case 'u':
        flags = IFF_TUN;
        break;
      case 'a':
        flags = IFF_TAP;
        header_len = ETH_HDR_LEN;
        break;
      default:
        my_err("Unknown option %c\n", option);
        usage();
    }
  }

  argv += optind;
  argc -= optind;

  if(argc > 0){
    my_err("Too many options!\n");
    usage();
  }

  if(*if_name == '\0'){
    my_err("Must specify interface name!\n");
    usage();
  }else if(cliserv < 0){
    my_err("Must specify client or server mode!\n");
    usage();
  }else if((cliserv == CLIENT)&&(*remote_ip == '\0')){
    my_err("Must specify server address!\n");
    usage();
  }

  /* initialize tun/tap interface */
  if ( (tap_fd = tun_alloc(if_name, flags | IFF_NO_PI)) < 0 ) {
    my_err("Error connecting to tun/tap interface %s!\n", if_name);
    exit(1);
  }

  do_debug("Successfully connected to interface %s\n", if_name);
  




 
  	if ( (sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    	 perror("socket()");
   	 exit(1);
 	 }

	/////////////////////////////////////////////////////////


    	memset(&local, 0, sizeof(local));
    	local.sin_family = AF_INET;
    	local.sin_addr.s_addr = htonl(INADDR_ANY);
    	local.sin_port = htons(port);
    	if (bind(sock_fd, (struct sockaddr*) &local, sizeof(local)) < 0){
      	perror("bind()");
      	exit(1);
    	}

	//init remote
	memset(&remote, 0, sizeof(remote));
	remote.sin_family = AF_INET;
	//remote.sin_addr.s_addr = inet_addr(remote_ip);
  	remote.sin_port = htons(port);	



	net_fd = sock_fd;

    	do_debug("SERVER: UDP built\n");
	//////////////////////////////////////////////////////////////////////

///////////ssl tcp control connection////////////////
	loadcert();
	listen_sd = socket (AF_INET, SOCK_STREAM, 0);   
	if(err < 0){
		printf("err tcp listening sock!\n");
		return 0;
	}
   
	if(setsockopt(listen_sd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1){
		perror("Server-socket() error lol!");
		exit(1);
	}  


	memset (&sa_serv, '\0', sizeof(sa_serv));
	sa_serv.sin_family      = AF_INET;
	sa_serv.sin_addr.s_addr = INADDR_ANY;
	sa_serv.sin_port        = htons (TCP_LISTEN_PORT);          /* Server Port number */
  
	err = bind(listen_sd, (struct sockaddr*) &sa_serv,
	sizeof (sa_serv));
	if(err < 0){
		printf("err tcp bind!\n");
		return 0;
	}
	     
  /* Receive a TCP connection. */
	     
	err = listen (listen_sd, 5);
	do_debug("Server start Listen OK\n");                    


	if(err < 0){
		printf("err tcp listening!\n");
		return 0;
	}
  /////

////////////////////////////////////////////////////

  
  /* use select() to handle two descriptors at once */
  maxfd = (tap_fd > net_fd)?tap_fd:net_fd;
  maxfd = (maxfd > listen_sd)?maxfd:listen_sd;

  while(1) {
    int ret;
    fd_set rd_set;

	struct Session *cursess;
	cursess= slisthead;

    FD_ZERO(&rd_set);
    FD_SET(tap_fd, &rd_set); FD_SET(net_fd, &rd_set);
	FD_SET(0, &rd_set);
	FD_SET(listen_sd,&rd_set);

	while(cursess != 0){
		FD_SET(cursess->sd,&rd_set);
		maxfd = (maxfd > cursess->sd)?maxfd:cursess->sd;
		cursess = cursess->next;
	}

	cursess = slisthead;

    ret = select(maxfd + 1, &rd_set, NULL, NULL, NULL);

    if (ret < 0 && errno == EINTR){
      continue;
    }

    if (ret < 0) {
      perror("select()");
      exit(1);
    }




	if(FD_ISSET(0, &rd_set)){
		memset(buffer,0,BUFSIZE);
		nread = read(0, buffer, BUFSIZE);
		if(0 == strncmp(buffer,"quit",strlen("quit")))
			break;

	}

	if(FD_ISSET(listen_sd, &rd_set)){
		client_len = sizeof(sa_cli);
		sd = accept (listen_sd, (struct sockaddr*) &sa_cli, &client_len);
		if(err < 0){
			printf("err accept!\n");
			return 0;
		}
		err = sslinit(sd, &slisthead, &sa_cli, "vpnauth");
		if(err)
			do_debug("Client from %s is connected\n", inet_ntoa(sa_cli.sin_addr));
		//continue;

	}


	if(FD_ISSET(tap_fd, &rd_set)){
      /* data from tun/tap: just read it and write it to the network */
		unsigned char outbuffer[VPN_BUFSIZE];
		struct VPNDataHdr *pvpnh = (struct VPNDataHdr*) outbuffer;
		unsigned long curip;
		cursess = 0;
		nread = cread(tap_fd, buffer, BUFSIZE);
		if(nread <= 0)
			continue;
		
		tap2net++;
		do_debug("TAP2NET %lu: Read %d bytes from the tap interface\n", tap2net, nread);

		ip_header = (struct ip*) buffer;
		memcpy(&curip,&(ip_header->ip_dst),sizeof(long));
		cursess = findsessByDstIP(slisthead,curip);
		//memcpy(&tpaddr,&(destsess->ip),sizeof(unsigned long));
		if(cursess == 0)
			continue;
		remote.sin_addr.s_addr = cursess->ip;
		

      /* write length + packet */

		if(32 != getHMACSHA256(buffer, pvpnh->hmac, nread,cursess->s_key,KEY_LENGTH))
			continue;
		nwrite = crypt(buffer,nread,outbuffer + sizeof(struct VPNDataHdr),cursess->s_key,cursess->iv,1);
		if(0 == nwrite)
			continue;
		pvpnh->plen = htons(nwrite);
		sendto(net_fd,outbuffer, nwrite + sizeof(struct VPNDataHdr),0,(struct sockaddr*) &remote, sizeof(remote));
	
		do_debug("TAP2NET %lu: Written %d bytes to the network %s\n", tap2net,nwrite,inet_ntoa(remote.sin_addr));
		//continue;
    }

    if(FD_ISSET(net_fd, &rd_set)){
      /* data from the network: read it, and write it to the tun/tap interface. 
       * We need to read the length first, and then the packet */
		unsigned char inbuffer[VPN_BUFSIZE];
		unsigned char hmac[32];
		int nwrite;
		struct VPNDataHdr *pvpnh = (struct VPNDataHdr*) inbuffer;
		struct sockaddr_in sa;
		socklen_t fromlen;
		fromlen =sizeof(sa);

		nread = recvfrom(net_fd,inbuffer,VPN_BUFSIZE,0,(struct sockaddr*)&sa,&fromlen);

		if(nread <= 0) {
	        continue;
	      }

		cursess = findsessByVPNHeader(slisthead,pvpnh,sa.sin_addr.s_addr);
		if(cursess == 0)
			continue;
			
		nwrite = crypt(inbuffer + sizeof(struct VPNDataHdr),htons(pvpnh->plen),buffer,cursess->s_key,cursess->iv,0);
		if(0 == nwrite)
			continue;
	
		if(32 != getHMACSHA256(buffer, hmac, nwrite,cursess->s_key,KEY_LENGTH) 
		|| !isequal(hmac,pvpnh->hmac,HMAC_LENGTH))
			continue;
		net2tap++;
		/* read packet */
		do_debug("NET2TAP %lu: Read %d bytes from the network\n", net2tap, nread);
		
		/* now buffer[] contains a full packet or frame, write it into the tun/tap interface */ 
		nwrite = cwrite(tap_fd, buffer, nwrite);
		do_debug("NET2TAP %lu: Written %d bytes to the tap interface\n", net2tap, nwrite);
		//continue;
    }

	cursess = slisthead;
	while(cursess != 0){
		if(FD_ISSET(cursess->sd, &rd_set)){
			struct CliRequest* rst;
			memset(buffer,0,BUFSIZE);
			nread = ssl_read(cursess->ssl,buffer,BUFSIZE);
			rst = (struct CliRequest*)buffer;
			if(nread != sizeof(struct CliRequest)){
				break;
			}
			if(nread == 0 || rst->act == QUIT_REQUEST){
				struct in_addr temp;
				temp.s_addr = cursess->ip;
				do_debug("Client from %s break the connection\n", inet_ntoa(temp));
				cleanupSSL(cursess);
				deletesess(&slisthead,cursess);
				//slisthead = 0;
			}

			if(rst->act == IV_REFRESH_REQUEST){
				RefreshIV(cursess);
				do_debug("IV is refreshed:");
				debugSession(cursess);
			}
			if(rst->act == IV_REFRESH_SET){
				struct ServResponse rsp;
				rsp.act = ACT_ACK;
				if(-1 != ssl_write(cursess->ssl,(char*)&rsp, sizeof(struct ServResponse))){
					memcpy(cursess->iv, rst->iv,KEY_LENGTH);
					do_debug("IV is updated:");
					debugSession(cursess);
				}
			}
			if(rst->act == KEY_REFRESH_REQUEST){
				RefreshKEY(cursess);
				do_debug("Key is refreshed:");
				debugSession(cursess);
			}
			if(rst->act == KEY_REFRESH_SET){
				struct ServResponse rsp;
				rsp.act = ACT_ACK;
				if(-1 != ssl_write(cursess->ssl,(char*)&rsp, sizeof(struct ServResponse))){
					memcpy(cursess->s_key, rst->s_key,KEY_LENGTH);
					do_debug("Key is updated:");
					debugSession(cursess);
				}
			}
			
		}
		cursess = cursess->next;
	}


	
  }

	if(slisthead != 0){
		struct Session* tsess = slisthead;
		while(tsess != 0){
			cleanupSSL(tsess);
			tsess = tsess->next;
		}
	}
	freesess(slisthead);
	close(listen_sd);
	close(net_fd);
	close(tap_fd);
	cleanupCTX();
  	
  return(0);
}
