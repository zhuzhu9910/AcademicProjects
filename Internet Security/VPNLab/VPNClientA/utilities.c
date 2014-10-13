#include "vpnutilities.h"

#include <stdlib.h>
#include <openssl/hmac.h>
#include <string.h>



int crypt(unsigned char *inbuf, int inlen, unsigned char *outbuf, unsigned char key[],unsigned char iv[], int do_encrypt)
        {

        int outlen, mlen;

	
	EVP_CIPHER_CTX ctx;
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, iv, do_encrypt);
	outlen = 0;

        if(inlen <= 0) return 0;
        if(!EVP_CipherUpdate(&ctx, outbuf + outlen, &mlen, inbuf, inlen))
        {
                /* Error */
        	EVP_CIPHER_CTX_cleanup(&ctx);
        	return 0;
        }
	outlen += mlen;
        if(!EVP_CipherFinal_ex(&ctx, outbuf + outlen, &mlen))
                {
                /* Error */
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
                }
	outlen += mlen;
        EVP_CIPHER_CTX_cleanup(&ctx);
        return outlen;
        }

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

int getHMACSHA256(char *msg, unsigned char* md_value, int len, unsigned char key[], int keylen)
{
	HMAC_CTX mdctx;
	int md_len;
	const EVP_MD *md;	
	OpenSSL_add_all_digests();

	md = EVP_get_digestbyname("sha256");

	HMAC_CTX_init(&mdctx);
	HMAC_Init_ex(&mdctx,key,keylen,md,NULL);
	HMAC_Update(&mdctx, msg, len);
	HMAC_Final(&mdctx, md_value, &md_len);
	HMAC_CTX_cleanup(&mdctx);
	return md_len;

}


int isequal(char *buf1, char* buf2, int len)
{
	int i = 0;
	while(i < len && buf1[i]==buf2[i]) i++;
	return i == len;
}




///////////////////////session list operations///////////////////////////////////


void addsesstail(struct Session** header, struct Session* item)
{
	struct Session* pnext;
	if(item == 0)
		return;
	if(*header == 0){
		*header = item;
		return;
	}
	item->next = 0;
	pnext = *header;
	while(pnext->next != 0) pnext = pnext->next;
	pnext->next = item;
}

void addsesshead(struct Session** header, struct Session* item)
{
	if(item == 0)
		return;

	item->next = *header;
	*header = item;
}

void deletesess(struct Session** header, struct Session* item)
{
	struct Session* ppre;

	if(0 == item || 0 == *header)
		return;
	
	ppre = *header;
	if(*header == item){
		*header = (*header)->next;
		free(ppre);
		return;
	}
	while(ppre->next != 0 && ppre->next != item) ppre = ppre->next;
	if(ppre->next){
		struct Session* temp = ppre->next;
		ppre->next = temp->next;
		free(temp);
	}
	
}

struct Session* findsessById(struct Session* header, unsigned long sid, unsigned long ip)
{
	while(header != 0)
	{
		if(header->sid == sid && header->ip == ip)
			return header;
		header= header->next;
	}

	return header;

}

struct Session* findsessBypos(struct Session* header, int pos)
{
	while(--pos > 0 && header) header = header->next;
	return header;
}

void freesess(struct Session* item)
{
	if(item != 0)
		freesess(item->next);
	else
		return;
	free(item);
}
