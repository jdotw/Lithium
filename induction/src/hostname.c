#include <unistd.h>
#include <stdio.h>

#include "induction.h"

void i_hostname_set (char *hostname, int len)
{
	if (len < 1 || !hostname) return;

	int num = sethostname (hostname, len);
	if (num != 0) i_printf(1, "i_hostname_set sethostname failed");
}
