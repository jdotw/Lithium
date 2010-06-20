#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "induction.h"
#include "desc.h"

/*
 * Functions relating to the description 
 * or desc_str variable used throughout
 * lithium. The description name should not 
 * contain special (e.g ') characters. Any illegal
 * character is replaced with a white space ' '
 */

/* Parse */

int i_desc_parse (char *desc_str)
{
  size_t i;
  
  if (!desc_str) return -1;
  
  for (i=0; i < strlen(desc_str); i++)
  {
    int space = 0;

    if (desc_str[i] == '\'') space = 1;
    if (desc_str[i] == '`') space = 1;

    if (space == 1)
    { desc_str[i] = ' '; }
  }

  return 0;
}
