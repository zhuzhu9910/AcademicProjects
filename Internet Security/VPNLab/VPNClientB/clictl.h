#ifndef SERVCTL_H
#define SERVCTL_H

void loadcert();
void cleanupCTX(struct Session* sess);
void cleanupSSL(struct Session* sess);
int sslinit(int sd, struct Session *sess, char* dfilename, char* pwd);
int ssl_read(SSL* ssl, char* buffer, int len);
int ssl_write(SSL* ssl, char* buffer, int len);

#endif
