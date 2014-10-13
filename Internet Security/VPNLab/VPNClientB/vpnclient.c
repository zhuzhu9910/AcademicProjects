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
 
#include "vpnutilities.h"
#include "clictl.h"

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
		
		printf("current session key "); 
		printHex((unsigned int *)(sess->s_key),KEY_LENGTH/sizeof(int));
		printf("\nsession iv ");
		printHex((unsigned int *)(sess->iv),KEY_LENGTH/sizeof(int));
		printf("\nsession id ");
		printHex((unsigned int *)&(sess->sid),sizeof(long)/sizeof(int));
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
//  uint16_t total_len, ethertype;
  char buffer[BUFSIZE];
  struct sockaddr_in local, remote;
  char remote_ip[16] = "";
  unsigned short int port = PORT;
  int sock_fd, net_fd, optval = 1;
  socklen_t remotelen;
  int cliserv = -1;    /* must be specified on cmd line */
  unsigned long int tap2net = 0, net2tap = 0;

	int sd;
	int err;
	struct sockaddr_in sa;

	unsigned long vip = 0;
	unsigned long subnet = 0;
	unsigned long mask = 0;

	int connected = 0;

  int ptc[2];         //pipe from control process to tunnel process

  progname = argv[0];
  
  /* Check command line options */
  while((option = getopt(argc, argv, "i:sc:p:uahdn:m:v:")) > 0){
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
	case 'n':
		subnet = (unsigned long)inet_addr(optarg);
		break;
	case 'm':
		mask = (unsigned long)inet_addr(optarg);
		break;
	case 'v':
		vip = (unsigned long)inet_addr(optarg);
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

	memset(&remote, 0, sizeof(remote));
	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = inet_addr(remote_ip);
  	remote.sin_port = htons(port);	



	net_fd = sock_fd;

    	do_debug("SERVER: %s\n", inet_ntoa(remote.sin_addr));
	//////////////////////////////////////////////////////////////////////

///////////ssl tcp control connection////////////////
	loadcert();
	slisthead = malloc(sizeof(struct Session));
	memset(slisthead,0,sizeof(struct Session));

	printf("VPN Client is Ready. Enter Your Command:\n");

  /////

////////////////////////////////////////////////////

  
  /* use select() to handle two descriptors at once */
  maxfd = (tap_fd > net_fd)?tap_fd:net_fd;
  maxfd = (maxfd > sd)?maxfd:sd;

  while(1) {
    int ret;
    fd_set rd_set;

	struct Session *cursess;
	cursess= slisthead;

    FD_ZERO(&rd_set);
    FD_SET(tap_fd, &rd_set); FD_SET(net_fd, &rd_set);
	FD_SET(sd, &rd_set);
	FD_SET(0, &rd_set);

    ret = select(maxfd + 1, &rd_set, NULL, NULL, NULL);

    if (ret < 0 && errno == EINTR){
      continue;
    }

    if (ret < 0) {
      perror("select()");
      exit(1);
    }



	if(FD_ISSET(0, &rd_set)){
		struct CliRequest rst;
		struct ServResponse* rsp;
		memset(buffer,0,BUFSIZE);
		nread = read(0, buffer, BUFSIZE);
		rsp = (struct ServResponse*)buffer;
		if(!connected && 0 == strncmp(buffer,"start",strlen("start"))){
			sd = socket (AF_INET, SOCK_STREAM, 0); 
			if(err < 0){
				printf("err socket!\n");
				return 0;
			}
 
			memset (&sa, '\0', sizeof(sa));
			sa.sin_family      = AF_INET;
			sa.sin_addr.s_addr = inet_addr (remote_ip);   /* Server IP */
			sa.sin_port        = htons     (TCP_LISTEN_PORT);          /* Server Port number */
  
			err = connect(sd, (struct sockaddr*) &sa, sizeof(sa));                   
			if(err < 0){
				printf("err connect\n");
				return 0;
			}

			slisthead->subnet = subnet;
			slisthead->mask = mask;
			slisthead->vip = vip;
			slisthead->sd = sd;
			err = sslinit(sd,slisthead,"vpnauth",getpass("Please Enter your VPN Password:\n"));

			if(err == 0){
				printf("Connection is refused by the Server!\n");
				continue;
			}
			connected = 1;

			debugSession(slisthead);
			
			continue;
		}		


		if(0 == strncmp(buffer,"quit",strlen("quit"))){
			
			rst.act = QUIT_REQUEST;
			if(connected){
				ssl_write(cursess->ssl,(char*)&rst,sizeof(struct CliRequest));
				cleanupSSL(cursess);
			}
			connected = 0;
			break;
		}
		if(connected && 0 == strncmp(buffer,"break",strlen("break"))){
			rst.act = QUIT_REQUEST;
			ssl_write(cursess->ssl,(char*)&rst,sizeof(struct CliRequest));
			connected = 0;
			cleanupSSL(cursess);
			do_debug("Disconnected\n");
			continue;
		}
		if(connected && 0 == strncmp(buffer,"refreshiv",strlen("refreshiv"))){

			rst.act = IV_REFRESH_REQUEST;
			err = ssl_write(cursess->ssl,(char*)&rst,sizeof(struct CliRequest));
			if(err != -1){
				err = ssl_read(cursess->ssl, buffer, sizeof(struct ServResponse));
				rsp = (struct ServResponse*)buffer;
				if(err == sizeof(struct ServResponse) && rsp->act == NEW_IV_INFO){
					memcpy(cursess->iv, rsp->iv, KEY_LENGTH);
				}
				debugSession(cursess);
			}
		}

		if(connected && 0 == strncmp(buffer,"refreshkey",strlen("refreshkey"))){
			rst.act = KEY_REFRESH_REQUEST;
			err = ssl_write(cursess->ssl,(char*)&rst,sizeof(struct CliRequest));
			if(err != -1){
				err = ssl_read(cursess->ssl, buffer, sizeof(struct ServResponse));
				rsp = (struct ServResponse*)buffer;
				if(err == sizeof(struct ServResponse) && rsp->act == NEW_KEY_INFO){
					memcpy(cursess->s_key, rsp->s_key, KEY_LENGTH);
				} 
			}
			debugSession(cursess);
		}

		if(connected && 0 == strncmp(buffer,"setkey",strlen("setkey"))){
			int slen;
			char* content;
			
			slen = strlen("setkey");
			if(buffer[slen] != ' ' || buffer[slen + 1] == 0)
				continue;
			content = buffer + slen + 1;
			slen = strlen(content) - 1;
			slen = slen < KEY_LENGTH?slen:KEY_LENGTH;
			memset((void*)&rst,0,sizeof(struct CliRequest));
			rst.act =  KEY_REFRESH_SET;
			memcpy((void*)(rst.s_key),content,slen);
			//rst.sid = cursess->sid;
			err = ssl_write(cursess->ssl,(char*)&rst,sizeof(struct CliRequest));
			if(err != -1){
				err = ssl_read(cursess->ssl, buffer, sizeof(struct ServResponse));
				if(err == sizeof(struct ServResponse) && rsp->act == ACT_ACK){
					memcpy(cursess->s_key, rst.s_key, KEY_LENGTH);
				} 
			}
			debugSession(cursess);
		}


		if(connected && 0 == strncmp(buffer,"setiv",strlen("setiv"))){
			int slen;
			char* content;
			
			slen = strlen("setiv");
			if(buffer[slen] != ' ' || buffer[slen + 1] == 0)
				continue;
			content = buffer + slen + 1;
			slen = strlen(content) - 1;
			memset((void*)&rst,0,sizeof(struct CliRequest));
			slen = slen < KEY_LENGTH?slen:KEY_LENGTH;
			rst.act =  IV_REFRESH_SET;
			memset(rst.iv,0,KEY_LENGTH);
			memcpy(rst.iv,content,slen);
			//rst.sid = cursess->sid;
			err = ssl_write(cursess->ssl,(char*)&rst,sizeof(struct CliRequest));
			if(err != -1){
				err = ssl_read(cursess->ssl, buffer, sizeof(struct ServResponse));
				if(err == sizeof(struct ServResponse) && rsp->act == ACT_ACK){
					memcpy(cursess->iv, rst.iv, KEY_LENGTH);
				} 
			}
			debugSession(cursess);
		}
		
		

	}


	if(connected && FD_ISSET(tap_fd, &rd_set)){
      /* data from tun/tap: just read it and write it to the network */
		unsigned char outbuffer[VPN_BUFSIZE];
		struct VPNDataHdr *pvpnh = (struct VPNDataHdr*) outbuffer;
		nread = cread(tap_fd, buffer, BUFSIZE);
		
		
		tap2net++;
		do_debug("TAP2NET %lu: Read %d bytes from the tap interface\n", tap2net, nread);

      /* write length + packet */

		if(32 != getHMACSHA256(buffer, pvpnh->hmac, nread,cursess->s_key,KEY_LENGTH))
			continue;
		nwrite = crypt(buffer,nread,outbuffer + sizeof(struct VPNDataHdr),cursess->s_key,cursess->iv,1);
		if(0 == nwrite)
			continue;
		pvpnh->plen = htons(nwrite);
		pvpnh->sid = cursess->sid;
		sendto(net_fd,outbuffer, nwrite + sizeof(struct VPNDataHdr),0,(struct sockaddr*) &remote, sizeof(remote));
	
		do_debug("TAP2NET %lu: Written %d bytes to the network\n", tap2net, nwrite);
    }

    if(connected && FD_ISSET(net_fd, &rd_set)){
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
      printf("\nREAD %d BYTES\n",nread);
      if(nread == 0) {
        /* ctrl-c at the other end */
        break;
      }

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
    }

	



	
  }

	if(slisthead != 0){
		cleanupCTX(slisthead);
  		freesess(slisthead);
	}
  return(0);
}
