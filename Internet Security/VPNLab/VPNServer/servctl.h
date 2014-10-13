#ifndef SERVCTL_H
#define SERVCTL_H

void loadcert();
void cleanupSSL(struct Session *ss);
void cleanupCTX();
int sslinit(int sd, struct Session **shead, struct sockaddr_in* skar, char* dfilename);
int ssl_read(SSL* ssl, char* buffer, int len);
int ssl_write(SSL* ssl, char* buffer, int len);
void RefreshIV(struct Session *sess);
void RefreshKEY(struct Session *sess);

#endif
