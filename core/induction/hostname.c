#include <unistd.h>
#include <stdio.h>

#include "induction.h"

void i_hostname_set (char *hostname, int len)
{
	if (len < 1 || !hostname) return;

	sethostname (hostname, len);
}
