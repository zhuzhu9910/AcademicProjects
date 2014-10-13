#ifndef _MINIFIREWALL_H
#define _MINIFIREWALL_H

struct policy{
	/*direction*/
	u_char direct;
	/*conditions*/
	u_char proto;
	u_long srcip;
	u_long dstip;
	u_long srcmask;
	u_long dstmask;
	union{
	u_short srcport;
	u_short index;
	};
	u_short dstport;
	/*action*/
	u_short act;
	/*validation*/
	u_char val;
	struct policy* next;
};

#define DIRECT_IN 0
#define DIRECT_OUT 1
#define DIRECT_CTL 255
#define CTL_FLAG_PRINT 2
#define CTL_FLAG_CLEAR 3
#define CTL_FLAG_DELETE 4

#define PROTO_ALL 250

#define VAL_PROTO 1
#define VAL_SRCIP 2
#define VAL_DSTIP 4
#define VAL_SRCM 8
#define VAL_DSTM 16
#define VAL_SRCPORT 32
#define VAL_DSTPORT 64

#define ACT_BLOCK 0
#define ACT_UNBLOCK 1

#define ACT_CTL_DEL 255
#define ACT_CTL_CLEAR 254


#endif
