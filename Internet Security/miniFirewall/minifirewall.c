#include <linux/module.h>

#include <linux/kernel.h>

#include <linux/proc_fs.h>

#include <linux/string.h>

#include <linux/vmalloc.h>

#include <asm/uaccess.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/skbuff.h>


#include "minifirewall.h"



MODULE_LICENSE("GPL");

MODULE_DESCRIPTION("MiniFirewall Kernel Module");

MODULE_AUTHOR("Qinyun Zhu");


static struct proc_dir_entry *proc_entry;    //proc entry


struct policy* inrule = 0;          //head pointer of the rule list
struct policy** in_next = &inrule;  //the pointer to the pointer of the next rule to read

struct udphdr *udp_header;          //udp header struct (not used)
struct tcphdr *tcp_header;          //tcp header struct
struct iphdr *ip_header;            //ip header struct

static struct nf_hook_ops nfho_in;   //net filter hook option struct
static struct nf_hook_ops nfho_out;   //net filter hook option struct

//add a rule to the end of the policy list
inline void addruletail(struct policy** header, struct policy* item)
{
	struct policy* pnext;
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


inline void addrulehead(struct policy** header, struct policy* item)
{
	if(item == 0)
		return;

	item->next = *header;
	*header = item;
}

//delete a rule from the list
inline void deleterule(struct policy** header, struct policy* item)
{
	struct policy* ppre;

	if(0 == item || 0 == *header)
		return;
	
	ppre = *header;
	if(*header == item){
		*header = (*header)->next;
		vfree(ppre);
		return;
	}
	while(ppre->next != 0 && ppre->next != item) ppre = ppre->next;
	if(ppre->next){
		struct policy* temp = ppre->next;
		ppre->next = temp->next;
		vfree(temp);
	}
	
}

//find a rule according to its position in the list (from 1 to n)
struct policy* findrule(struct policy* header, int pos)
{
	while(--pos > 0 && header) header = header->next;
	return header;
}

//free all the rules
void freerules(struct policy* item)
{
	if(item != 0)
		freerules(item->next);
	else
		return;
	vfree(item);
}


//callback function when user write a new policy or command to the filter
ssize_t minifirewall_write( struct file *filp, const char __user *buff,

                        unsigned long len, void *data )

{

  struct policy* newrule;


  if (len != sizeof(struct policy)) {



    printk(KERN_INFO "input lenghth is incorrect!\n");

    return -ENOSPC;



  }

  newrule = (struct policy *)vmalloc(sizeof(struct policy));
  
  if(!newrule){
		printk(KERN_INFO "rule insertion failed!\n");
		return -ENOSPC;
  }

  if (copy_from_user( newrule, buff, len )) {
    vfree(newrule);
    return -EFAULT;

  }
	

	//interpret and execute 
	//the control commands from the user space
	if(DIRECT_CTL == newrule->direct){
		struct policy* ptemp;
		switch(newrule->act){
			//delete a rule
			case ACT_CTL_DEL:
				ptemp = findrule(inrule,(int)(newrule->index));
				deleterule(&inrule,ptemp);
				break;
			//clear all the rules
			case ACT_CTL_CLEAR:
				freerules(inrule);
				inrule = 0;
				break;
		}
		in_next = &inrule;
		vfree(newrule);
		return 0;
	}
	
	//add a rule to the end of the rule list
	addruletail(&inrule,newrule);

  return len;

}

int minifirewall_read( char *page, char **start, off_t off,

                   int count, int *eof, void *data )

{

  const int len = sizeof(struct policy);
	

	if(*in_next == 0){
		in_next = &inrule;
		*eof = 1;
    return 0;
	}

	memcpy(page,*in_next,len);

	in_next = &((*in_next)->next);

  return len;

}

//match engine
//if the packet can fire the rule then return 1 else return 0
int matchrule(struct policy* rule, struct sk_buff *sock_buff, int direct)
{
	//if any condition fails in the rule then return 0 else 1
	//check validation of rule, incoming buffer and rule direction type
	if(!rule || !sock_buff || direct != rule->direct)
		return 0;
		
	//check conditions about IP layer
	ip_header = (struct iphdr *)skb_network_header(sock_buff);
	
	//check source ip address without netmask
	if((rule->val & VAL_SRCIP) && !(rule->val & VAL_SRCM) 
		&& rule->srcip != (u_long)(ip_header->saddr))
		return 0;
	
	//check source subnet with netmask
	if((rule->val & VAL_SRCIP) && (rule->val & VAL_SRCM)
		&& rule->srcip != ((u_long)(ip_header->saddr) & rule->srcmask))
		return 0;

	//check destination ip address without netmask
	if((rule->val & VAL_DSTIP) && !(rule->val & VAL_DSTM)
		&& rule->dstip != (u_long)(ip_header->daddr))
		return 0;
	
	//check source subnet with netmask
	if((rule->val & VAL_DSTIP) && (rule->val & VAL_DSTM)
		&& rule->dstip != ((u_long)(ip_header->daddr) & rule->dstmask))
		return 0;
	
	//check upper layer protocol
	if((rule->val & VAL_PROTO) && rule->proto != (u_char)(ip_header->protocol))
		return 0;

	//check ports if it is TCP
	if(rule->proto == IPPROTO_TCP){
		tcp_header = (struct tcphdr *)skb_transport_header(sock_buff);
		if((rule->val & VAL_SRCPORT) && rule->srcport != (u_short)(tcp_header->source))
			return 0;
		if((rule->val & VAL_DSTPORT) && rule->dstport != (u_short)(tcp_header->dest))
			return 0;
	}
		//check ports if it is UDP
	if(rule->proto == IPPROTO_UDP){
		udp_header = (struct udphdr *)skb_transport_header(sock_buff);
		if((rule->val & VAL_SRCPORT) && rule->srcport != (u_short)(udp_header->source))
			return 0;
		if((rule->val & VAL_DSTPORT) && rule->dstport != (u_short)(udp_header->dest))
			return 0;
	}

	printk("rule fired: direct:%d, proto:%d, src:%d, dst:%d, act %d\n",direct,rule->proto,ip_header->saddr,ip_header->daddr,rule->act);
	return 1;
}

//map the policy action to the action of the netfilter
inline unsigned int getaction(u_short act)
{
	switch(act){
		case ACT_BLOCK: return NF_DROP;
		case ACT_UNBLOCK: return NF_ACCEPT;
	}
	return NF_ACCEPT;

}


//hook function for the incoming packets
unsigned int hook_in_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))
{
	struct policy* rhd = inrule;
	unsigned int act = NF_ACCEPT;
       
	if(!skb) { return NF_ACCEPT;}
	
	while(rhd != 0){
		if(matchrule(rhd,skb,DIRECT_IN)){
			act = getaction(rhd->act);
		}
		rhd = rhd->next;
	}	

	return act;
}

//hook function for the outgoing packets
unsigned int hook_out_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))
{
	struct policy* rhd = inrule;
	unsigned int act = NF_ACCEPT;
       
	if(!skb) { return NF_ACCEPT;}
	
	while(rhd != 0){
		if(matchrule(rhd,skb,DIRECT_OUT)){
			act = getaction(rhd->act);
		}
		rhd = rhd->next;
	}	

	return act;
}


int init_minifirewall_module( void )

{

  int ret = 0;


  proc_entry = create_proc_entry( "minifirewall", 0644, NULL );



  if (proc_entry == NULL) {



      ret = -ENOMEM;

      printk(KERN_INFO "minifirewall: Couldn't create proc entry\n");



  } else {




      proc_entry->read_proc = minifirewall_read;

      proc_entry->write_proc = minifirewall_write;

      printk(KERN_INFO "minifirewall: Module loaded.\n");

			nfho_in.hook = hook_in_func;
			nfho_in.hooknum = NF_INET_PRE_ROUTING;
			nfho_in.pf = PF_INET;
			nfho_in.priority = NF_IP_PRI_FIRST;
			nf_register_hook(&nfho_in);

			nfho_out.hook = hook_out_func;
			nfho_out.hooknum = NF_INET_POST_ROUTING;
			nfho_out.pf = PF_INET;
			nfho_out.priority = NF_IP_PRI_FIRST;
			nf_register_hook(&nfho_out);


  }



  return ret;

}





void cleanup_minifirewall_module( void )

{

  remove_proc_entry("minifirewall", NULL);

	nf_unregister_hook(&nfho_in);
	nf_unregister_hook(&nfho_out);

  freerules(inrule);

	

  printk(KERN_INFO "minifirewall: Module unloaded.\n");

}





module_init( init_minifirewall_module );

module_exit( cleanup_minifirewall_module );

