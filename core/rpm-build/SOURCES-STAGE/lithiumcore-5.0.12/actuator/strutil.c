#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <induction.h>

#include "strutil.h"

char* a_string_nl_br (char *orig_str)
{
  /* Parse a string (orig_str) and add a <BR> tag 
   * before all instances of '\n' (new line)
   */

  int len;
  char *rtn_str;    /* The string to be returned */
  char *ptr;        /* Current position in rtn_str */
  char *nl_ptr;     /* A pointer to a '\n' character in rtn_str */

  rtn_str = strdup (orig_str);
  ptr = rtn_str;
  len = strlen (rtn_str) + 1;

  while ((nl_ptr = strchr (ptr, '\n')))
  {
    int nl_offset;
    char *temp_str;
    char *temp_ptr;

    /* Calculate offset for this nl. I.e how far
     * into the current state of rtn_str is it
     */
    nl_offset = nl_ptr - rtn_str;

    /* Found a '\n', calculate the new string
     * size to accommodate a <BR> tag
     */
    len += 4;

    /* Realloc more mem for rtn_str to fit
     * the <BR> tag to be added 
     */
    temp_str = (char *) malloc (len);
    if (!temp_str)
    { i_printf (1, "a_string_nl_br failed to malloc temp_str (%i bytes)", len); free (rtn_str); return NULL; }
    strcpy (temp_str, rtn_str);

    /* Add the tag to the right place in temp_str */
    temp_ptr = temp_str + nl_offset;
    snprintf (temp_ptr, 5, "<BR>");

    /* Move past the newly added <BR> tag */
    temp_ptr += 4;

    /* Copy in the rest of the string after the 
     * new line '\n' in rtn_str
     */

    snprintf (temp_ptr, strlen(nl_ptr)+1, "%s", nl_ptr);

    /* Free rtn_str (now superseeded by temp_str) 
     * and set pointers accordingly 
     */
    free (rtn_str);
    rtn_str = temp_str;
    ptr = temp_ptr+1;
  }

  return rtn_str;
}
