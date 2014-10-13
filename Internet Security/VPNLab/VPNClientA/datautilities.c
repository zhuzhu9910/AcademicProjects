#include <stdlib.h>
#include <openssl/evp.h>
#include <string.h>
#include <fcntl.h>
#include "datautility.h"

struct userRecord records[MAX_RECORDS];
int rs_size = 0;
/*
 int getSHA256(char *msg, unsigned char* md_value, int len)
 {
	EVP_MD_CTX mdctx;
	int md_len;
	const EVP_MD *md;	
	OpenSSL_add_all_digests();
	md = EVP_get_digestbyname("sha256");

	EVP_MD_CTX_init(&mdctx);
	EVP_DigestInit_ex(&mdctx, md, NULL);
	EVP_DigestUpdate(&mdctx, msg, len);
	EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
	EVP_MD_CTX_cleanup(&mdctx);
	return md_len;
 }
*/

int readRecords(int fd, struct userRecord *buf, int rdnum)
{
	int nrd;
	nrd = read(fd,(void*)buf, rdnum*sizeof(struct userRecord));
	
	if(0 == nrd % sizeof(struct userRecord))
		return nrd/sizeof(struct userRecord);
	return -1;
}

int loadRecords(char* datafilename)
{
	int fd;
	int num;
	rs_size = 0;
	memset(records,0,sizeof(struct userRecord)*MAX_RECORDS);
	fd = open(datafilename,O_RDONLY);

	num = readRecords(fd,records,MAX_RECORDS);
	if(num != -1){
		rs_size = num;
		
	}

	close(fd);
	return num;
}

int compareString(char* s1, char* s2, int len)
{
	int i = 0;
	while(i < len && s1[i] == s2[i]) i++;
	return i == len;

}

int findRecord(char* name, int nlen)
{
	int i;
	if(0 == rs_size)
		return -1;
	for(i = 0; i < rs_size; ++i){
		if(nlen != records[i].u_len)
			continue;
		if(compareString(name,records[i].u_name,nlen))
			return i;
	}
		
	return -1;
}


int matchReacord(char* name, int nlen, char* pwd, int plen)
{
	unsigned char mac[MAC_LEN];
	int pos;
	pos = findRecord(name,nlen);
	if(pos == -1)
		return -1;
	getSHA256(pwd,mac,plen);
	return compareString(mac,records[pos].mac_pwd,MAC_LEN);
	
}

int addRecord(char* datafilename,char* name,int nlen,char* pwd,int plen)
{
	int fd;
	int num;
	struct userRecord rd;
	if(nlen > UNAME_MAX_LEN)
		return -1;
	fd = open(datafilename,O_APPEND|O_WRONLY);
	rd.u_len = nlen;
	memcpy(rd.u_name,name,nlen);
	if(MAC_LEN != getSHA256(pwd,rd.mac_pwd,plen))
		return -1;
	num = write(fd,(void*)&rd,sizeof(struct userRecord));

	close(fd);
	return num;
}

