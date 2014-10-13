#ifndef DATAUTILITY_H
#define DATAUTILITY_H

#define UNAME_MAX_LEN 256
#define MAC_LEN 32
#define MAX_RECORDS 128

struct userRecord
{
	unsigned short u_len;
	unsigned char u_name[UNAME_MAX_LEN];
	unsigned char mac_pwd[MAC_LEN];
	

};

int addRecord(char* datafilename,char* name,int nlen,char* pwd,int plen);
int matchReacord(char* name, int nlen, char* pwd, int plen);
int findRecord(char* name, int nlen);
int loadRecords(char* datafilename);


#endif
