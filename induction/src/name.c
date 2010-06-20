#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "induction.h"
#include "name.h"

/*
 * Functions relating to the 'unique name' 
 * or name_str variable used throughout
 * lithium. The unique name should only ever
 * container alpha numeric characters. Any other
 * character is replaced with an underscore '_'
 */

/* Parse 
 *
 * Takes the name_str and replaces any non-alphanumeric
 * characters with an underscore
 */

int i_name_parse (char *name_str)
{
  size_t i;
  
  if (!name_str) return -1;
  
  for (i=0; i < strlen(name_str); i++)
  {
    if (isalnum(name_str[i]) == 0 && name_str[i] != '-')
    { name_str[i] = '_'; }
  }

  return 0;
}
