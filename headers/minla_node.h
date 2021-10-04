#ifndef MINLA_PREFIX_HH
#define MINLA_PREFIX_HH


#define _MAX_DEPTH_ 10

typedef struct minla_node{

	unsigned int flag;
	int cost;
	int permutation[_MAX_DEPTH_];

} Minla_node;


#endif
