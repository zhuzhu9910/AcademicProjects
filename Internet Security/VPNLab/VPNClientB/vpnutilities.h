#ifndef VPNUTILITIES_H
#define VPNUTILITIES_H


#include <openssl/evp.h>

#define KEY_LENGTH 16
#define HMAC_LENGTH 32


#define CONNECT_REQUEST 1
#define KEY_REFRESH_REQUEST 2
#define IV_REFRESH_REQUEST 3
#define IV_REFRESH_SET 4
#define KEY_REFRESH_SET 5
#define QUIT_REQUEST 6
#define ACT_ACK 7

#define REFUSE 0
#define NEW_SESSION_INFO 1
#define NEW_KEY_INFO 2
#define NEW_IV_INFO 3



#define PWD_MAX_LEN 16

struct CliRequest
{
	//unsigned long sid;
	unsigned int act;
	union{
	unsigned char pwd[PWD_MAX_LEN];
	unsigned char s_key[KEY_LENGTH];
	unsigned char iv[KEY_LENGTH];
	};
	unsigned long vip;
	unsigned long subnet;
	unsigned long mask;
};

struct ServResponse
{
	unsigned long sid;
	unsigned int act;
	unsigned char s_key[KEY_LENGTH];
	unsigned char iv[KEY_LENGTH];
};



struct VPNDataHdr
{
	unsigned short plen;
	unsigned long sid;
	unsigned char hmac[HMAC_LENGTH];

};


struct Session
{
	unsigned long sid;
	unsigned char s_key[KEY_LENGTH];
	unsigned char iv[KEY_LENGTH];
	unsigned long ip;
	unsigned long vip;
	unsigned long subnet;
	unsigned long mask;
	int sd;
	SSL* ssl;
	struct Session* next;
};



//1 for crypt, 0 for decrypt
int crypt(unsigned char *inbuf, int inlen, unsigned char *outbuf, unsigned char key[],unsigned char iv[], int do_encrypt);
int getSHA256(char *msg, unsigned char* md_value, int len);
int getHMACSHA256(char *msg, unsigned char* md_value, int len, unsigned char key[], int keylen);
int isequal(char *buf1, char* buf2, int len);

void addsesstail(struct Session** header, struct Session* item);
void addsesshead(struct Session** header, struct Session* item);
void deletesess(struct Session** header, struct Session* item);
void freesess(struct Session* item);
struct Session* findsessById(struct Session* header, unsigned long sid, unsigned long ip);
struct Session* findsessBypos(struct Session* header, int pos);


/* buffer for reading from tun/tap interface, must be >= 1500 */
#define BUFSIZE 2000   
#define CLIENT 0
#define SERVER 1
#define PORT 55555
#define TCP_LISTEN_PORT 33333

#define VPN_BUFSIZE BUFSIZE + sizeof(struct VPNDataHdr) + EVP_MAX_BLOCK_LENGTH

/* some common lengths */
#define IP_HDR_LEN 20
#define ETH_HDR_LEN 14
#define ARP_PKT_LEN 28





#endif
