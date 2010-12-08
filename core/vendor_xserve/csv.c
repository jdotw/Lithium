#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>

#include "csv.h"

/* 
 * Comma separately list (CSV) processing
 */

i_list* v_csv_process_data (char *data)
{
  /* Processes the data string as a CSV formatted data block.
   * An i_list, of i_list's of char* strings is returned
   *
   * i_list *list
   *    i_list *row
   *      char *str
   *      char *str
   *      char *str
   *    
   */

  i_list *list = i_list_create ();
  char *line_str;
  char *dataptr = data;

  if (!data) return NULL;

  i_list_set_destructor (list, i_list_free);
  
  /* Process each line */
  while ((line_str = v_csv_get_line (&dataptr)))
  {
    /* Create row */
    i_list *row = i_list_create ();
    i_list_set_destructor (row, free);
    i_list_enqueue (list, row);

    /* Get elements */
    char *lineptr = line_str;
    char *element_str;
    while ((element_str = v_csv_get_element(&lineptr)))
    {
      i_list_enqueue (row, element_str);
    }

    free (line_str);
  }

  return list;
}

char* v_csv_get_line (char **dataptr)
{
  if (*dataptr == NULL) return NULL;

  /* Find end of line */
  char *line_str = NULL;
  char *end_ptr = strchr (*dataptr, '\n');
  if (end_ptr)
  {
    /* End of line found, create string, move beyond */
    size_t len = end_ptr - *dataptr;
    line_str = malloc (len+1);
    memset (line_str, 0, len+1);
    strncpy (line_str, *dataptr, len);
    *dataptr += len + 1; 
  }
  else
  {
    /* No end of line, return up to end of string */
    end_ptr = *dataptr + strlen(*dataptr);
    size_t len = end_ptr - *dataptr;
    line_str = malloc (len+1);
    memset (line_str, 0, len+1);
    strncpy (line_str, *dataptr, len);
    *dataptr = NULL;
  }

  return line_str;
}

char *v_csv_get_element (char **lineptr)
{
  /* Return the next element on the line */
  if (*lineptr == NULL) return NULL;

  /* Find comma */
  char *element_str = NULL;
  char *comma_ptr = strchr (*lineptr, ',');
  if (comma_ptr)
  {
    /* Comma found. Create string, move beyond */
    size_t len = comma_ptr - *lineptr;
    element_str = malloc (len+1);
    memset (element_str, 0, len+1);
    strncpy (element_str, *lineptr, len);
    *lineptr += len+1;
  }
  else
  {
    /* No comma, return up to end of string */
    comma_ptr = *lineptr + strlen(*lineptr);
    size_t len = comma_ptr - *lineptr;
    element_str = malloc (len+1);
    memset (element_str, 0, len+1);
    strncpy (element_str, *lineptr, len);
    *lineptr = NULL;
  }

  return element_str;
}   
