#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <time.h>

#include "vpnutilities.h"
#include "datautility.h"
//#include "ctlcomm.h"

/* define HOME to be dir for key and cert files... */
#define HOME "./"
/* Make these what you want for cert & key files */
#define CERTF  HOME "vpnserver.crt"
#define KEYF  HOME  "vpnserver.key"
#define CACERT HOME "ca.crt"

#define COMMON_LEN 128


#define CHK_NULL(x,d,s) if ((x)==NULL) {close (d);SSL_free (s);return 0;}
#define CHK_SSL(err,d,s) if ((err)==-1) { ERR_print_errors_fp(stderr); close (d);SSL_free (s); return 0; }
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }

SSL_CTX* ctx;
X509*    client_cert;
char     buf [sizeof(struct CliRequest)];
SSL_METHOD *meth;

void loadcert()
{
  /* SSL preliminaries. We keep the certificate and key with the context. */

  SSL_load_error_strings();
  SSLeay_add_ssl_algorithms();
  meth = SSLv23_server_method();
  ctx = SSL_CTX_new (meth);
  if (!ctx) {
    ERR_print_errors_fp(stderr);
    exit(2);
  }

  SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL); /* whether verify the certificate */
  SSL_CTX_load_verify_locations(ctx,CACERT,NULL);
  
  if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    exit(3);
  }
  if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    exit(4);
  }

  if (!SSL_CTX_check_private_key(ctx)) {
    fprintf(stderr,"Private key does not match the certificate public key\n");
    exit(5);
  }
}


void getResponse(struct Session **shead, struct CliRequest* preq, struct ServResponse* rsp, struct sockaddr_in* skar, int sd, SSL* ssl)
{
	unsigned int seed = (unsigned int)time(NULL);
	int err;
	RAND_seed((void*) &seed,sizeof(int));
	if(CONNECT_REQUEST == preq->act){
		struct Session* ss = malloc(sizeof(struct Session));
		unsigned long rd;
		memset(ss,0,sizeof(struct Session));
		ss->vip = preq->vip;
		err = RAND_bytes((unsigned char*)&(ss->sid),sizeof(long));
		if(err == -1)
			printf("random generate sid failed\n");
		err = RAND_bytes((unsigned char*)&(ss->s_key),KEY_LENGTH);
		if(err == -1)
			printf("random generate key failed\n");
		err = RAND_bytes((unsigned char*)&(ss->iv),KEY_LENGTH);
		if(err == -1)
			printf("random generate iv failed\n");
		ss->ip = (unsigned long)(skar->sin_addr.s_addr);
		ss->subnet = preq->subnet;
		ss->mask = preq->mask;
		ss->ssl =ssl;
		ss->sd = sd;
		addsesstail(shead,ss);
		rsp->sid = ss->sid;
		rsp->act = NEW_SESSION_INFO;
		memcpy(rsp->s_key,ss->s_key,KEY_LENGTH);
		memcpy(rsp->iv,ss->iv,KEY_LENGTH);
	}


}


void cleanupCTX()
{
  SSL_CTX_free (ctx);
}

void cleanupSSL(struct Session *ss)
{
	close (ss->sd);
  	SSL_free (ss->ssl);
}


int ssl_read(SSL* ssl, char* buffer, int len)
{
	return SSL_read(ssl,buffer,len);
}

int ssl_write(SSL* ssl, char* buffer, int len)
{
	return SSL_write(ssl,buffer,len);
}



void RefreshIV(struct Session *sess)
{
	int err;
	struct ServResponse rsp;
	err = RAND_bytes((unsigned char*)&(sess->iv),KEY_LENGTH);
	if(err == -1)
		printf("Refresh: random generate iv failed\n");
	rsp.sid = sess->sid;
	rsp.act = NEW_IV_INFO;
	memcpy(rsp.s_key,sess->s_key,KEY_LENGTH);
	memcpy(rsp.iv,sess->iv,KEY_LENGTH);
	err = SSL_write(sess->ssl,&rsp,sizeof(struct ServResponse));
	if(err == -1)
		printf("Refresh: communication failed\n");
}

void RefreshKEY(struct Session *sess)
{
	int err;
	struct ServResponse rsp;
	err = RAND_bytes((unsigned char*)&(sess->s_key),KEY_LENGTH);
	if(err == -1)
		printf("Refresh: random generate iv failed\n");
	rsp.sid = sess->sid;
	rsp.act = NEW_KEY_INFO;
	memcpy(rsp.s_key,sess->s_key,KEY_LENGTH);
	memcpy(rsp.iv,sess->iv,KEY_LENGTH);
	err = SSL_write(sess->ssl,&rsp,sizeof(struct ServResponse));
	if(err == -1)
		printf("Refresh: communication failed\n");
}



int sslinit(int sd, struct Session **shead, struct sockaddr_in* skar, char* dfilename)
{
  int err;
  size_t client_len;
  char*    str;
  char peer_CN[COMMON_LEN];
  SSL*     ssl;
  int isauth = 0;
  
  /* ----------------------------------------------- */
  /* TCP connection is ready. Do server side SSL. */

  ssl = SSL_new (ctx);                           CHK_NULL(ssl,sd,ssl);
  SSL_set_fd (ssl, sd);
  err = SSL_accept (ssl);                        CHK_SSL(err,sd,ssl);
  
  /* Get the cipher - opt */
  
  printf ("SSL connection using %s\n", SSL_get_cipher (ssl));
  
  /* Get client's certificate (note: beware of dynamic allocation) - opt */

  client_cert = SSL_get_peer_certificate (ssl);
  if (client_cert != NULL) {
    struct ServResponse rsp;
    struct CliRequest *preq;
    //printf ("Client certificate:\n");
    
   /* str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0);
    CHK_NULL(str,sd,ssl);
    printf ("\t subject: %s\n", str);
    OPENSSL_free (str);
    
    str = X509_NAME_oneline (X509_get_issuer_name  (client_cert), 0, 0);
    CHK_NULL(str,sd,ssl);
    printf ("\t issuer: %s\n", str);
    OPENSSL_free (str);*/
    
    /* We could do all sorts of certificate verification stuff here before
       deallocating the certificate. */

    X509_NAME_get_text_by_NID(X509_get_subject_name(client_cert),NID_commonName, peer_CN, COMMON_LEN);

    

  /* DATA EXCHANGE - Receive message and send reply. */

    //rsp = malloc(sizeof(struct ServResponse));
    memset(&rsp,0,sizeof(struct ServResponse));
    err = SSL_read (ssl, buf, sizeof(struct CliRequest));         CHK_SSL(err,sd,ssl);

    if(err != sizeof(struct CliRequest)){
	X509_free (client_cert);
	close(sd);
	SSL_free(ssl);
	printf("Client has weird behavior, it must be mad.\n");
	return 0;
    }
    preq = (struct CliRequest *) buf;

	if(X509_cmp_current_time(X509_get_notBefore(client_cert)) >=0 
		|| X509_cmp_current_time(X509_get_notAfter(client_cert)) <=0){
		close(sd);
		SSL_free(ssl);
		printf("Client Certificate is expired\n");
		rsp.act = REFUSE;
		return 0;
	}

    if(loadRecords(dfilename) == -1 || !matchReacord(peer_CN,strlen(peer_CN),preq->pwd,strlen(preq->pwd))){
		close(sd);
		SSL_free(ssl);
		printf("Client is denied\n");
		rsp.act = REFUSE;
		return 0;
		
	}else{
		printf("Client %s is verified\n", peer_CN);
		getResponse(shead,preq,&rsp,skar,sd,ssl);
    
	}
    err = SSL_write (ssl, &rsp, sizeof(struct ServResponse));  CHK_SSL(err,sd,ssl);
    X509_free (client_cert);
  } else{
    printf ("Client does not have certificate.\n");
    return 0;
  }




  /* Clean up. */


 // SSL_CTX_free (ctx);

  return 1;
}


#ifdef SERVER_TEST

struct Session* st;

void main()
{
	int listen_sd;
  	int sd;
  	struct sockaddr_in sa_serv;
	struct sockaddr_in sa_cli;
	int yes = 1;
	int err;
	size_t client_len;
	fd_set rd_set;


	//char* testpass;
  	//testpass = getpass("Please Enter the Password");
	//printf("%s,%d\n",testpass,strlen(testpass));
	loadcert();
  /* ----------------------------------------------- */
  /* Prepare TCP socket for receiving connections */

	listen_sd = socket (AF_INET, SOCK_STREAM, 0);   CHK_ERR(listen_sd, "socket");
   
	if(setsockopt(listen_sd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1){
		perror("Server-socket() error lol!");
		exit(1);
	}  


	memset (&sa_serv, '\0', sizeof(sa_serv));
	sa_serv.sin_family      = AF_INET;
	sa_serv.sin_addr.s_addr = INADDR_ANY;
	sa_serv.sin_port        = htons (1111);          /* Server Port number */
  
	err = bind(listen_sd, (struct sockaddr*) &sa_serv,
	sizeof (sa_serv));                   CHK_ERR(err, "bind");
	     
  /* Receive a TCP connection. */
	     
	err = listen (listen_sd, 5);
	printf("Server LIsten OK\n");                    
	err = 0;

	FD_ZERO(&rd_set);
	FD_SET(listen_sd,&rd_set);
	FD_SET(0,&rd_set);
	
	//while(err != listen_sd){
  	err = select(listen_sd + 1, &rd_set,NULL,NULL,NULL);
	CHK_ERR(err,"select");
	//printf("hi\n");
	//if(err != listen_sd)
	//	read(0,buf,16);
	//printf("%s\n",buf);
	//}

  //CHK_ERR(err, "listen");
	memset(buf,0,512);
	client_len = sizeof(sa_cli);
	sd = accept (listen_sd, (struct sockaddr*) &sa_cli, &client_len);
	CHK_ERR(sd, "accept");
	yes = sslinit(sd, &st, &sa_cli, "vpnauth");
	close (listen_sd);
	if(yes)
		cleanupSSL(st);
	cleanupCTX();
}


#endif


