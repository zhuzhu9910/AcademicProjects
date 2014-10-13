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
#include "clictl.h"
//#include "ctlcomm.h"

/* define HOME to be dir for key and cert files... */
#define HOME "./"
/* Make these what you want for cert & key files */
#define CERTF  HOME "client.crt"
#define KEYF  HOME  "client.key"
#define CACERT HOME "ca.crt"

#define COMMON_LEN 128

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

SSL_CTX* ctx;

X509*    server_cert;
SSL_METHOD *meth;

void loadcert()
{
  int err;
  /* SSL preliminaries. We keep the certificate and key with the context. */

  SSLeay_add_ssl_algorithms();
  meth = SSLv23_client_method();
  SSL_load_error_strings();
  ctx = SSL_CTX_new (meth);                        CHK_NULL(ctx);

  CHK_SSL(err);


  SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL);
  SSL_CTX_load_verify_locations(ctx,CACERT,NULL);

  if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) {
	  ERR_print_errors_fp(stderr);
	  exit(-2);
  }
  
  if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0) {
	  ERR_print_errors_fp(stderr);
	  exit(-3);
  }

  if (!SSL_CTX_check_private_key(ctx)) {
	  printf("Private key does not match the certificate public keyn");
	  exit(-4);
  }
}


int ssl_read(SSL* ssl, char* buffer, int len)
{
	return SSL_read(ssl,buffer,len);
}

int ssl_write(SSL* ssl, char* buffer, int len)
{
	return SSL_write(ssl,buffer,len);
}




void cleanupSSL(struct Session* sess)
{

  SSL_shutdown (sess->ssl);  /* send SSL/TLS close_notify */

  /* Clean up. */

  close (sess->sd);
  sess->sd = 0;
  SSL_free (sess->ssl);
  sess->ssl = 0;
}

void cleanupCTX(struct Session* sess)
{
	SSL_CTX_free (ctx);
}


int sslinit(int sd, struct Session *sess, char* dfilename, char* pwd)
{
  int err;
  //size_t client_len;
  char*    str;
  char peer_CN[COMMON_LEN];
  SSL*     ssl;
  int isauth = 0;
  
  /* ----------------------------------------------- */
  /* Now we have TCP conncetion. Start SSL negotiation. */
  
  ssl = SSL_new (ctx);                         CHK_NULL(ssl);    
  SSL_set_fd (ssl, sd);
  err = SSL_connect (ssl);                     CHK_SSL(err);
    
  /* Following two steps are optional and not required for
     data exchange to be successful. */
  
  /* Get the cipher - opt */

  printf ("SSL connection using %s\n", SSL_get_cipher (ssl));
  
  /* Get server's certificate (note: beware of dynamic allocation) - opt */

  server_cert = SSL_get_peer_certificate (ssl);       
  CHK_NULL(server_cert);
  printf ("Server certificate:\n");
  
  str = X509_NAME_oneline (X509_get_subject_name (server_cert),0,0);
  CHK_NULL(str);
  printf ("\t subject: %s\n", str);
  OPENSSL_free (str);

  str = X509_NAME_oneline (X509_get_issuer_name  (server_cert),0,0);
  CHK_NULL(str);
  printf ("\t issuer: %s\n", str);
  OPENSSL_free (str);

  /* We could do all sorts of certificate verification stuff here before
     deallocating the certificate. */

  X509_NAME_get_text_by_NID(X509_get_subject_name(server_cert),NID_commonName, peer_CN, COMMON_LEN);

    if(loadRecords(dfilename) != -1){
		if(findRecord(peer_CN,strlen(peer_CN)) != -1)
			isauth = 1;
    }

	if(X509_cmp_current_time(X509_get_notBefore(server_cert)) >=0 
		|| X509_cmp_current_time(X509_get_notAfter(server_cert)) <=0){  
		isauth = 0;
	}

  X509_free (server_cert);
  
  /* --------------------------------------------------- */
  /* DATA EXCHANGE - Send a message and receive a reply. */
  if(isauth){
	struct CliRequest rqt;
	struct ServResponse rsp;
	int pwd_len = strlen(pwd);
	memset(&rqt,0,sizeof(struct CliRequest));
	rqt.act = CONNECT_REQUEST;
	memcpy(rqt.pwd,pwd,(pwd_len<PWD_MAX_LEN?pwd_len:PWD_MAX_LEN));
	rqt.vip = sess->vip;
	rqt.subnet = sess->subnet;
	rqt.mask = sess->mask;
	

  err = SSL_write (ssl, &rqt, sizeof(rqt));  CHK_SSL(err);
  	
  err = SSL_read (ssl, &rsp, sizeof(rsp));                     CHK_SSL(err);
	if(err < sizeof(rsp) || rsp.act != NEW_SESSION_INFO)
		return 0;
	sess->sid = rsp.sid;
	memcpy(sess->s_key,rsp.s_key,KEY_LENGTH);
	memcpy(sess->iv,rsp.iv,KEY_LENGTH);
	sess->ssl =ssl;
	sess->sd =sd;
  	
  }else{
	printf("The Server is not authenticated\n");
	return 0;
	}

  return 1;
}

#ifdef CTLI_TEST

struct Session sess;

void main()
{
	int sd;
	int err;
	struct sockaddr_in sa;

	loadcert();
	memset(&sess,0,sizeof(sess));
  sd = socket (AF_INET, SOCK_STREAM, 0);       CHK_ERR(sd, "socket");
 
  memset (&sa, '\0', sizeof(sa));
  sa.sin_family      = AF_INET;
  sa.sin_addr.s_addr = inet_addr ("127.0.0.1");   /* Server IP */
  sa.sin_port        = htons     (1111);          /* Server Port number */
  
  err = connect(sd, (struct sockaddr*) &sa,
		sizeof(sa));                   CHK_ERR(err, "connect");

	sess.subnet = (unsigned long)inet_addr("10.0.2.0");
	sess.mask = (unsigned long)inet_addr("255.255.255.0");
	sess.vip = (unsigned long)inet_addr("10.0.4.1");
	sslinit(sd,&sess,"vpnauth","client");
	cleanup(&sess);
}

#endif



