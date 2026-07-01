#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include "ra_ioctl.h"

#ifndef CONFIG_SUPPORT_OPENWRT
#define ETH_DEVNAME "eth2"
#else
#define ETH_DEVNAME "eth0"
#endif

int qdma_fd;

void qdma_init(void)
{
	qdma_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (qdma_fd < 0) {
		perror("socket");
		exit(0);
	}
}

void qdma_fini(void)
{
	close(qdma_fd);
}

void usage(char *cmd)
{
#if 1
	printf("Usage:\n");
	printf(" %s resv [queue] [hw_resv] [sw_resv]                       - set reservation for queue#\n", cmd);
	printf(" %s sch [queue] [sch]                                      - set scheduler for queue#\n", cmd);
	printf(" %s sch_rate [sch] [sch_en][sch_rate]                      - set SCH rate control\n", cmd);
#if defined (CONFIG_MTK_EMI_7622)
	printf(" %s sch_policy [sch] [wrr/sp]	                           - set PQ arbitration mechanism between MIN and MAX rate\n", cmd);
#endif    
	printf(" %s weight [queue] [weighting]                             - set max rate weighting for queue#\n", cmd);
	printf(" %s rate [queue] [min_en] [min_rate] [max_en] [max_rate]   - set rate control for queue#\n", cmd);
	printf(" %s m2q [mark] [queue]                                     - set set skb->mark to queue mapping table\n", cmd);
#if defined (CONFIG_MTK_EMI_7622)
	printf(" queue: 0 ~ 63.\n");
#else
	printf(" queue: 0 ~ 15.\n");
#endif
	printf(" hw_resv and sw_resv in decimal.\n");
	printf(" sch: 0 for SCH1. 1 for SCH2.\n");
	printf(" wrr(0)/sp(1): Physical Queue arbitration mechanism between MIN and MAX rate");
	printf(" weighting: 0 ~ 15.\n");
	printf(" sch_en: 1 for enable, o for disable. sch rate: 0 ~ 1000000 in Kbps.\n");
	printf(" min_en: 1 for enable, 0 for disable. min rate: 0 ~ 1000000 in Kbps.\n");
	printf(" max_en: 1 for enable, 0 for disable. max rate: 0 ~ 1000000 in Kbps.\n");
	printf(" mark: 0 ~ 63.\n");
#endif	
	qdma_fini();
	exit(0);
}

int reg_read(int offset, int *value)
{
	struct ifreq ifr;
	struct qdma_ioctl_data data;

	if (value == NULL)
		return -1;

	data.cmd = RAETH_QDMA_REG_READ;
	data.off = offset;
	strncpy(ifr.ifr_name, ETH_DEVNAME , 5);
	ifr.ifr_data = &data;
	if (-1 == ioctl(qdma_fd, RAETH_QDMA_IOCTL, &ifr)) {
		perror("ioctl");
		close(qdma_fd);
		exit(0);
	}
	*value = data.val;
	printf("data.val = %x\n", data.val);
	return 0;
}

int reg_write(int offset, int value)
{
	struct ifreq ifr;
	struct qdma_ioctl_data data;

	data.cmd = RAETH_QDMA_REG_WRITE;
	data.off = offset;
	data.val = value;
	strncpy(ifr.ifr_name, ETH_DEVNAME , 5);
	ifr.ifr_data = &data;
	if (-1 == ioctl(qdma_fd, RAETH_QDMA_IOCTL, &ifr)) {
		perror("ioctl");
		close(qdma_fd);
		exit(0);
	}
	return 0;
}

int queue_mapping(int mark, int queue)
{
	struct ifreq ifr;
	struct qdma_ioctl_data data;

	data.cmd = RAETH_QDMA_QUEUE_MAPPING;
	data.off = mark;
	data.val = queue;
	strncpy(ifr.ifr_name, ETH_DEVNAME , 5);
	ifr.ifr_data = &data;
        if (-1 == ioctl(qdma_fd, RAETH_QDMA_IOCTL, &ifr)) {
            perror("ioctl");
            close(qdma_fd);
            exit(0);
	}
	return 0;
}
#if defined (CONFIG_HW_SFQ)  
int sfq_web_enable(int sfqenable)
{
	struct ifreq ifr;
	struct qdma_ioctl_data data;

	data.cmd = RAETH_QDMA_SFQ_WEB_ENABLE;
	data.val = sfqenable;
	strncpy(ifr.ifr_name, ETH_DEVNAME , 5);
	ifr.ifr_data = &data;
  if (-1 == ioctl(qdma_fd, RAETH_QDMA_IOCTL, &ifr)) {
            perror("ioctl");
            close(qdma_fd);
            exit(0);
	}
	return 0;
}

int sfq_enable(int sfq)
{
	struct ifreq ifr;
	struct qdma_ioctl_data data;

	data.cmd = RAETH_QDMA_SFQ_WEB_ENABLE;
	data.val = sfq;
	strncpy(ifr.ifr_name, ETH_DEVNAME , 5);
	ifr.ifr_data = &data;
  if (-1 == ioctl(qdma_fd, RAETH_QDMA_IOCTL, &ifr)) {
            perror("ioctl");
            close(qdma_fd);
            exit(0);
	}
	return 0;
}

#endif
unsigned int rate_convert(unsigned int rate)
{
    unsigned int man;
    if(rate % 10)
	    man = rate / 10 +1;
    else
	    man = rate /10;
    return man;
}

int main(int argc, char *argv[])
{
    qdma_init();

    if (argc < 2)
        usage(argv[0]);
    else if (!strncmp(argv[1], "resv", 5)) {
  	printf("resv\n");
	unsigned int off, val, hw_resv, sw_resv;
	if (argc < 5)
	    usage(argv[0]);
	off = strtoul(argv[2], NULL, 10)* 0x10;
	hw_resv = strtoul(argv[3], NULL, 10);
	sw_resv = strtoul(argv[4], NULL, 10);
	val = (hw_resv << 8) | sw_resv;
#if !defined (CONFIG_MTK_EMI_7622)
	if (off > 0xf0 || val > 0xffff){
	    usage(argv[0]);
	}
#else
	if (off > 0x3f0 || val > 0xffff)
	    usage(argv[0]);
#endif	   
	//val = (hw_resv << 8) | sw_resv;
        reg_write(off, val);
	printf("set offset %x as %x for reservation.\n", off, val);
    }
    else if (!strncmp(argv[1], "sch", 4)) {			
	printf("sch\n");
	unsigned int off, val, sch;
	if (argc <4)
            usage(argv[0]);
	off = strtoul(argv[2], NULL, 10)* 0x10 + 0x04;
	sch = strtoul(argv[3], NULL, 10);
/*	
	if (off > 0xf4 || sch > 1)
	    usage(argv[0]);*/
#if !defined (CONFIG_MTK_EMI_7622)
	if (off > 0xf4 || sch > 1)
	    usage(argv[0]);
#else
	if (off > 0x3f4 || sch > 1){
		printf("off = %x val=%x\n", off, val);
	    usage(argv[0]);
	}
#endif	   

	reg_read(off, &val);
	val =(val & 0x7fffffff) | (sch << 31);
	reg_write(off, val);
	printf("set offset %x as %x for sch selection.\n", off, val);
    }
    else if (!strncmp(argv[1], "weight", 7)) {
        printf("!!!!!weight\n");
	unsigned int off, val, weight;
	if (argc < 4) {
		printf("!!!argc!!! = %d\n", argc);
	    usage(argv[0]);
	}
	off = strtoul(argv[2], NULL, 10)* 0x10 + 0x04;
	weight = strtoul(argv[3], NULL, 10);
	if (weight >= 16) {
		printf("!!!!weight!!!! = %d\n", weight);
		usage(argv[0]);
	}
/*
	if (off > 0xf4 || weight > 0xf)
	    usage(argv[0]);*/
#if !defined (CONFIG_MTK_EMI_7622)
	if (off > 0xf4 || weight > 16)
	    usage(argv[0]);
#else
	if (off > 0x3f4 || weight > 16) {
		printf("!!!!! off =%x, val = %x\n", off, val);
	    	usage(argv[0]);
	}
#endif	   
	reg_read(off, &val);
	val = (val & 0xffff0fff) | (weight << 12);
	reg_write(off, val);
	printf("set offset %x as %x for max rate weight.\n", off, val);
    }
    else if (!strncmp(argv[1], "rate", 5)) {			
        printf("rate\n");
	unsigned int off, val, min_en, min_rate, max_en, max_rate, exp, man;
#if 0		
#if defined (CONFIG_RALINK_MT7621)
	unsigned int cpu_clk;
	read_cpu_clk(off, &val);
	cpu_clk = val;
#endif	
#endif	
	if (argc < 7)
	    usage(argv[0]);
	off = strtoul(argv[2], NULL, 10)* 0x10 + 0x04;
	min_en = strtoul(argv[3], NULL, 10);
        min_rate = strtoul(argv[4], NULL, 10);
	max_rate = strtoul(argv[6], NULL, 10);
	max_en = strtoul(argv[5], NULL, 10);
#if !defined (CONFIG_MTK_EMI_7622)
        if (off > 0xf4 || (min_en > 1) || (max_en > 1)|| min_rate > 1000000 || max_rate > 1000000 )		
	    usage(argv[0]);
#else
	if (off > 0x3f4 || (min_en > 1) || (max_en > 1)|| min_rate > 1000000 || max_rate > 1000000 )		
		usage(argv[0]);
#endif
/* only for MT7621 E1 and E2, not needed in E3 and after */	
#if 0	
#if defined (CONFIG_RALINK_MT7621)	
	min_rate = min_rate * 125 / cpu_clk;
	max_rate = max_rate * 125 / cpu_clk;
#endif
#endif	
#if 0	
        if (min_rate > 127){
	     if ( min_rate % 10)
                 min_rate = (min_rate / 10 + 1) << 4 | 0x04;
	     else
	         min_rate = (min_rate / 10) << 4 | 0x04;
	}else
	     min_rate = min_rate << 4 | 0x03;
        if (max_rate > 127){
            if ( max_rate % 10)
	        max_rate = (max_rate / 10 + 1) << 4 | 0x04;
	    else
	        max_rate = (max_rate / 10) << 4 | 0x04;
	}else
             max_rate = max_rate << 4 | 0x03;
#endif
#if (1)
	if (min_rate > 127000){
		exp = 0x04;
		man = rate_convert(min_rate / 1000 );
	}else if (min_rate > 12700){
		exp = 0x03;
		man = rate_convert(min_rate / 100);
	}else if (min_rate >= 100){
	        exp = 0x02;
	        man = rate_convert(min_rate / 10);
	}else{
		exp = 0x00;
		man = min_rate;		
	}
#else
	if (min_rate > 127000){
		exp = 0x04;
		man = rate_convert(min_rate / 1000 );
	}else if (min_rate > 12700){
		exp = 0x03;
		man = rate_convert(min_rate / 100);
	}else if (min_rate > 1270){
	        exp = 0x02;
	        man = rate_convert(min_rate / 10);
	}else if (min_rate > 127){
		exp = 0x01;
		man = rate_convert(min_rate);
	}else{
		exp = 0x00;
		man = min_rate;
	}
#endif
#if defined(CONFIG_MTK_FPGA)
	exp = exp + 1;
#endif
        min_rate = man << 4 | exp;
#if (1)
        if (max_rate > 127000){
	       exp = 0x04;
               man = rate_convert(max_rate / 1000);
	}else if (max_rate > 12700){
		exp = 0x03;
		man = rate_convert(max_rate / 100);
	}else if (max_rate >= 100){
		exp = 0x02;
		man = rate_convert(max_rate / 10);
	}else{
		exp = 0x00;
		man = max_rate;
	}
#else
        if (max_rate > 127000){
	       exp = 0x04;
               man = rate_convert(max_rate / 1000);
	}else if (max_rate > 12700){
		exp = 0x03;
		man = rate_convert(max_rate / 100);
	}else if (max_rate > 1270){
		exp = 0x02;
		man = rate_convert(max_rate / 10);
	}else if (max_rate > 127){
		exp = 0x01;
		man = rate_convert(max_rate);
	}else{
		exp = 0x00;
		man = max_rate;
	}
#endif
#if defined(CONFIG_MTK_FPGA)
	exp = exp + 1;
#endif
        max_rate = man << 4 | exp;

	reg_read(off, &val);
	val = (val&0xf000f000) | (min_en <<27) | (min_rate<<16) | (max_en<<11) | (max_rate);
	reg_write(off, val);
	printf("set offset %x as %x for rate control.\n", off, val);
    }
    else if (!strncmp(argv[1], "sch_rate", 9)) {
        unsigned int off, val, sch, sch_en, sch_rate, man, exp;
#if 0
#if defined (CONFIG_RALINK_MT7621)
        unsigned int cpu_clk;
        read_cpu_clk(off, &val);
        cpu_clk = val;
#endif
#endif
	if (argc < 5)
	    usage(argv[0]);
#if !defined (CONFIG_MTK_EMI_7622)
	off = 0x214;
#else
	off = 0x777; //magic number for ioctl identify CR
#endif
	sch = strtoul(argv[2], NULL, 10);
	sch_en = strtoul(argv[3], NULL, 10);
        sch_rate = strtoul(argv[4], NULL, 10);
        if ( (sch > 1 ) || (sch_en > 1) || (sch_rate > 1000000))
       	    usage(argv[0]);
/* only for MT7621 E1 and E2, not needed in E3 and after */
#if 0
#if defined (CONFIG_RALINK_MT7621)
        sch_rate =  sch_rate * 125 / cpu_clk;
#endif	
#endif

#if 0	
	if (sch_rate > 127){
	    if ( sch_rate % 10)
	        sch_rate = (sch_rate / 10 + 1) << 4 | 0x04;
	    else
	        sch_rate = (sch_rate / 10) << 4 | 0x04;
	}else
	    sch_rate = sch_rate << 4 | 0x03;
#endif
#if (1)
	if (sch_rate > 127000){
		exp = 0x04;
		man = rate_convert(sch_rate / 1000 );
	}else if (sch_rate > 12700){
		exp = 0x03;
		man = rate_convert(sch_rate / 100);
	}else if (sch_rate >= 100){
	        exp = 0x02;
	        man = rate_convert(sch_rate / 10);
	}else{
		exp = 0x00;
		man = sch_rate;		
	}
#else
        if (sch_rate > 127000){
		exp = 0x04;
		man = rate_convert(sch_rate / 1000 );
	}else if (sch_rate > 12700){
		exp = 0x03;
		man = rate_convert(sch_rate / 100);
	}else if (sch_rate > 1270){
		exp = 0x02;
		man = rate_convert(sch_rate / 10);
	}else if (sch_rate > 127){
		exp = 0x01;
		man = rate_convert(sch_rate);
	}else{
		exp = 0x00;
		man = sch_rate;
	}
#endif

	sch_rate = man << 4 | exp;

        reg_read(off, &val);
	if (sch == 1)
	    val = (val&0xffff) | (sch_en << 27) | (sch_rate <<16);
	else
	    val = (val&0xffff0000) | (sch_en << 11) | (sch_rate);
	reg_write(off, val);
	printf("set offset %x as %x for sch rate control.\n", off, val);
    }
    else if (!strncmp(argv[1], "m2q", 4)) {
	int mark, queue;
	mark = strtoul(argv[2], NULL, 10);
	queue = strtoul(argv[3], NULL, 10);
#if !defined (CONFIG_MTK_EMI_7622)
	if( mark > 63 || queue > 15)
	    usage(argv[0]);
#else
	if( queue > 63)
	    usage(argv[0]);
#endif
	/* Separate LAN/WAN packet with the same mark value*/
	
		if(argc == 5)
			mark |= 0x100;			
  		queue_mapping(mark, queue);
		printf("set queue mapping: skb with mark %x to queue %d.\n",mark, queue);
  }
#if defined (CONFIG_MTK_EMI_7622)
	else if (!strncmp(argv[1], "sch_policy", 11)) {
		unsigned int off, val, sch, wrr;
		sch = strtoul(argv[2], NULL, 10);
		wrr = strtoul(argv[3], NULL, 10);
		off = 0x777; //magic number for ioctl identify CR 0x1b101a04
		reg_read(off, &val);
		if (sch == 1)
	   		val = (val & 0x7fffffff) | (wrr << 31);
		else
			val = (val&0xffff7000) | (wrr << 15);
		reg_write(off, val);
		printf("set offset 0x1b101a14 as %x for sch policy.\n", val);
	}
#endif
#if defined (CONFIG_HW_SFQ)  
  else if (!strncmp(argv[1], "sfqweb", 7)) {
  	int sfqweb=0;
  	sfqweb = strtoul(argv[2], NULL, 10);
  	sfq_web_enable(sfqweb);
  }else if (!strncmp(argv[1], "sfq", 4)) {
  	int sfq=0;
  	sfq = strtoul(argv[2], NULL, 10);
  	sfq_enable(sfq);
  }
#endif    

    else
        usage(argv[0]);
        qdma_fini();
	return 0;
}