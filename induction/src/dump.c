#include <stdio.h>
#include <stdlib.h>

#include "induction.h"

void ascii_dump (char *data, int datasize)
{
	int i;
	for (i=0; i < datasize; i++)
	{
		/* Loop through each character */
		int x;
		for (x=0; x<8; x++)
		{ printf ("%2x ", data[i]); i++; if (i == datasize) return; }
		printf ("<BR>\n");
	}
	printf ("<BR>\n");
}

