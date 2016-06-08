// name = quanquan, ID = qquan

#include "cachelab.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <string.h>
#include "head.h"

struct Line {
    char valid;
    long unsigned tag;
} ;

int main(int argc, char* argv[])
{
    int opt, s, E, b;
    int verbose_mode = 0;
    char* t;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
	    case 'h':
		printf(HELP_MSG);
		exit(EXIT_SUCCESS);
		break;
	    case 'v':
		verbose_mode = 1;
		break;
	    case 's':
		s = atoi(optarg);
		break;
	    case 'E':
		E = atoi(optarg);
		break;
	    case 'b':
		b = atoi(optarg);
		break;
	    case 't':
		t = optarg;
		break;
	    default:
		fprintf(stderr, 
                "Usage: %s [-hv] -s<s> -E<E> -b<b> -t<tracefile>\n", argv[0]);
                exit(EXIT_FAILURE);
	}
    }
    if (s <= 0 || E <= 0 || b <= 0 || t == NULL) {
	printf(HELP_MSG);
	exit(EXIT_FAILURE);
    }
    if (s + E + b > 64) {
	printf("s, E, b inValid!\n");
	exit(EXIT_FAILURE);
    }


    FILE * pFile;
    pFile = fopen(t, "r"); //open file for reading
    if (pFile == NULL) {
	fprintf(stderr, "cannot open file : %s\n", t);	
	exit(EXIT_FAILURE);
    }

    int S = pow(2, s);
    struct Line cache[S][E];
    int m, n;    
    for (m = 0; m < S; m++)
	for (n = 0; n < E; n++) {
	    cache[m][n].valid = 0;
	}

    //memset(cache, 0, S * E * 40);
    int hit_count = 0, miss_count = 0, eviction_count = 0;

    char identifier;
    long unsigned address;
    int size;
    //reading lines like " M 20,1" or " L 19,3"
    while (fscanf(pFile, " %c %lx,%d", &identifier, &address, &size) > 0) {
	if (identifier == 'I') continue;
    	if (verbose_mode) {
	    printf("%c %lx,%d", identifier, address, size);
    	}
	long unsigned set_id = (address >> b) & ~(0xffffffffffffffff << s);
	long unsigned tag = address >> (b + s);
	int i, line_id = -1;	
	for (i = 0; i < E; i++) {	
	    if (cache[set_id][i].valid == 1 && cache[set_id][i].tag == tag) {
	    	line_id = i;
		break;
	    }	
	}
	// miss, remove last one, increase others by one, put addr to the first 
	if (line_id == -1) {
	    if (verbose_mode) printf(" miss");
	    miss_count++;
	    if (cache[set_id][E - 1].valid == 1) {
		if (verbose_mode) printf(" eviction");
		eviction_count++;
	    }
	    for (i = E - 2; i >= 0; i--) {
	        if (cache[set_id][i].valid == 1) {
		    cache[set_id][i + 1].valid = 1;
		    cache[set_id][i + 1].tag = cache[set_id][i].tag;
		}
	    }
	    cache[set_id][0].valid = 1;
	    cache[set_id][0].tag = tag;	
	} else {   // hit! 
	    if (verbose_mode) printf(" hit");
	    hit_count++;
	    for (i = line_id - 1; i >= 0; i--) {
		cache[set_id][i + 1].valid = 1;
		cache[set_id][i + 1].tag = cache[set_id][i].tag;
	    }
	    cache[set_id][0].valid = 1;
	    cache[set_id][0].tag = tag;	
	}
	if (identifier == 'M') {
	    hit_count++;	
	    if (verbose_mode) printf(" hit");
	}
	if (verbose_mode) printf("\n");
    }
    
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
