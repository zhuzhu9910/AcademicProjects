#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pcap.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <getopt.h>
#include "minifirewall.h"

static int vf;

//print a policy
void printpolicy(struct policy* buf){
	struct in_addr src,dst;
	if(buf == 0)
		return;
	//print the direction
	if(buf->direct == DIRECT_IN)	
		printf("Direction:in ");
	if(buf->direct == DIRECT_OUT)	
		printf("Direction:out ");

	//print the protocol
	printf("Protocol:");
	if(buf->val & VAL_PROTO){
		switch(buf->proto){
		case IPPROTO_ICMP:
			printf("ICMP "); break;
		case IPPROTO_TCP:
			printf("TCP "); break;
		case IPPROTO_UDP:
			printf("UDP "); break;
		case IPPROTO_IPIP:
			printf("IPIP "); break;
		default:
			printf("%d ",buf->proto);
		}
	}
	else{
		printf("all ");
	}
	
	//print source ip
	if(buf->val & VAL_SRCIP){
		src.s_addr = (in_addr_t)(buf->srcip);
		printf("SrcIP:%s ", inet_ntoa(src));
	}else{
		printf("SrcIP:all ");
	}
	//print destination ip
	if(buf->val & VAL_DSTIP){
		dst.s_addr = (in_addr_t)(buf->dstip);
		printf("DestIP:%s ", inet_ntoa(dst));
	}else{
		printf("DestIP:all ");
	}
	//print source mask
	if(buf->val & VAL_SRCM){
		src.s_addr = (in_addr_t)(buf->srcmask);
		printf("SrcMask:%s ", inet_ntoa(src));
	}else{
		printf("SrcMask:all ");
	}
	//print destination mask
	if(buf->val & VAL_DSTM){
		dst.s_addr = (in_addr_t)(buf->dstmask);
		printf("DestMask:%s ", inet_ntoa(dst));
	}else{
		printf("DestMask:all ");
	}
	//print source port
	if(buf->val & VAL_SRCPORT)
		printf("SrcPort:%d ",ntohs(buf->srcport));
	else
		printf("SrcPort:all ");
	//print destination port
	if(buf->val & VAL_DSTPORT)
		printf("DestPort:%d ",ntohs(buf->dstport));
	else
		printf("DestPort:all ");
	//print actions
	if(buf->act == ACT_BLOCK)
		printf("Action:block");
	if(buf->act == ACT_UNBLOCK)
		printf("Action:unblock");
	printf("\n");
}

//print the rules in the kernel filter
void printrules()
{
	struct policy* buffer;
	int rt,c;
	int fdr=open("/proc/minifirewall",O_RDONLY);
	if(fdr==-1){
        printf("ERROR: file not found.\n");
        return;
  }
	c = 1;
	while(-1 != (rt = read(fdr,buffer,sizeof(struct policy))) && rt != 0){
			printf("[%d] ",c++);
			printpolicy(buffer);
			lseek(fdr,0,SEEK_SET);
	}
	close(fdr);
}

//interpret the action command to a action value in our policy
u_short getaction(char* opt)
{
	if(0 == strcmp(opt,"BLOCK") || 0 == strcmp(opt,"block"))
		return ACT_BLOCK;

	if(0 == strcmp(opt,"UNBLOCK") || 0 == strcmp(opt,"unblock"))
		return ACT_UNBLOCK;
	return -1;
}

//translate the protocol number to string for printing
//the default return value is the string of the protocol number
u_char getproto(char* proto)
{
	if(0 == strcmp(proto,"TCP") || 0 == strcmp(proto,"tcp"))
		return IPPROTO_TCP;
	if(0 == strcmp(proto,"UDP") || 0 == strcmp(proto,"udp"))
		return IPPROTO_TCP;
	if(0 == strcmp(proto,"ICMP") || 0 == strcmp(proto,"icmp"))
		return IPPROTO_ICMP;
	if(0 == strcmp(proto,"IPIP") || 0 == strcmp(proto,"ipip"))
		return IPPROTO_IPIP;
	return (u_char)atoi(proto);
}

//write a policy to the kernel filter
void writearule(struct policy* prule)
{
	int fd = open("/proc/minifirewall",O_WRONLY);
	if(fd==-1){
        printf("ERROR: file not found.\n");
        return;
  }
	write(fd,(void*)prule,sizeof(struct policy));
	close(fd);
}

int main(int argc, char** argv)
{
	int c;
  struct policy rule = {0,0,0,0,0,0,0,0,0,0,0};
	if(argc < 1)
		return;
	//read and interpret the commands from the user input
	for(;;){
		static struct option long_opts[] =
		{
			{"in",no_argument,&vf,DIRECT_IN},
			{"out", no_argument,&vf,DIRECT_OUT},
			{"print", no_argument,&vf,CTL_FLAG_PRINT},
			{"clear", no_argument,&vf,CTL_FLAG_CLEAR},
			{"delete", required_argument,&vf, CTL_FLAG_DELETE},
			{"proto", required_argument,0, 'p'},
			{"srcip", required_argument,0, 's'},
			{"dstip", required_argument,0, 'd'},
			{"srcport", required_argument,0, 'm'},
			{"dstport", required_argument,0, 'n'},
			{"srcmask", required_argument,0, 'l'},
			{"dstmask", required_argument,0, 'k'},
			{"action", required_argument,0, 'a'},
			{0,0,0,0}
		};

		int opt_index = 0;
	
		c = getopt_long(argc,argv,"",long_opts,&opt_index);
		
		if(c == -1)
			break;
		
		
		switch(c){
		case 0:
			if(vf == CTL_FLAG_PRINT){
				printrules();
				return;
			}
			if(vf == CTL_FLAG_CLEAR){
				rule.direct = DIRECT_CTL;
				rule.act = ACT_CTL_CLEAR;
				writearule(&rule);
				return;
			}
			if(vf == CTL_FLAG_DELETE){
				if(!optarg)
					printf("ERROR: delete number is required\n");
				rule.direct = DIRECT_CTL;
				rule.act = ACT_CTL_DEL;
				rule.index = (u_short)atoi(optarg);
				writearule(&rule);
				printrules();
				return;			
			}
			rule.direct = vf;
			break;
		case 'p':
			if(0 == strcmp(optarg,"all") || 0 == strcmp(optarg,"ALL")) break;
			rule.proto = getproto(optarg);
			rule.val |= VAL_PROTO;
			break;
		case 's':
			if(0 == strcmp(optarg,"all") || 0 == strcmp(optarg,"ALL")) break;
			rule.srcip = (u_long)inet_addr(optarg);
			rule.val |= VAL_SRCIP;
			break;
		case 'd':
			if(0 == strcmp(optarg,"all") || 0 == strcmp(optarg,"ALL")) break;
			rule.dstip = (u_long)inet_addr(optarg);
			rule.val |= VAL_DSTIP;
			break;
		case 'l':
			if(0 == strcmp(optarg,"all") || 0 == strcmp(optarg,"ALL")) break;
			rule.srcmask = (u_long)inet_addr(optarg);
			rule.val |= VAL_SRCM;
			break;
		case 'k':
			if(0 == strcmp(optarg,"all") || 0 == strcmp(optarg,"ALL")) break;
			rule.dstmask = (u_long)inet_addr(optarg);
			rule.val |= VAL_DSTM;
			break;
		case 'm':
			if(0 == strcmp(optarg,"all") || 0 == strcmp(optarg,"ALL")) break;
			rule.srcport = htons(atoi(optarg));
			rule.val |= VAL_SRCPORT;
			break;
		case 'n':
			if(0 == strcmp(optarg,"all") || 0 == strcmp(optarg,"ALL")) break;
			rule.dstport = htons(atoi(optarg));
			rule.val |= VAL_DSTPORT;
			break;
		case 'a':
			if(-1 == (rule.act = getaction(optarg))){
				puts("ERROR: illegal action!\n");
				return;
			}
			break;
		case '?':
			puts("ERROR: check command format\n");
			break;
		default:
			puts("ERROR\n");
			abort();
		}
	}
	writearule(&rule);

	return 0;
}
