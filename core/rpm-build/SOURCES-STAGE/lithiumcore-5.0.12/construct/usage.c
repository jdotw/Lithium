#include <stdio.h>
#include <stdlib.h>

void c_usage ()
{
	printf ("Usage : lithium [options] [module.so] [function] [args]\n\n");
	printf ("\t -c 	- config path (default %s)\n", CONFIG_PATH);
	printf ("\t -m 	- module path (default %s)\n", MODULE_PATH);
	printf ("\t -l 	- log path (default %s)\n", LOG_PATH);
	printf ("\t -r 	- data path (default %s)\n", DATA_PATH);
	printf ("\t -v  - verbose (log level += 1 per -v arg)\n");
	printf ("\t -V  - version\n");
	printf ("\t -q  - quiet operation (no stdout output)\n");
    printf ("\t -e  - function execution mode\n");
    printf ("\t -u  - username\n");
    printf ("\t -p  - password\n");
    printf ("\t -C  - customer id\n");
    printf ("\t -n  - lithium node ip address\n");

	exit (2);
}
