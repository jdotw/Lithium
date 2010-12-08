#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "induction.h"
#include "metric.h"
#include "str.h"

char* i_string_glue (char *str1, char *str2)
{
  int datasize;
  char *data;

  if (!str1 || !str2) return NULL;

  datasize = strlen(str1) + strlen(str2) + 1;
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_string_glue failed ot malloc data (%i bytes)", datasize); return NULL; }

  strcpy (data, str1);
  strcat (data, str2);
  
  return data;
}

char* i_string_extract_word (char *str, int *offsetptr)    /* Offset is the number of bytes before the string starts */
{
  int offset = 0;
  char *ptr;
  char *extract;

  if (!str) return NULL;
  
  if (*str == ' ')    /* Skip preceeding spaces */
  {
    while (*str == ' ') { str++; offset++; }
  }

  ptr = str;
  if (offsetptr) memcpy (offsetptr, &offset, sizeof(int));
  
  while (*ptr != ' ' && *ptr != '\n' && *ptr != '\0' && ((ptr - str) < (int) strlen(str)))
  {
    /* Cycle untill space/newline/NULL or end of string */

    ptr++;
  }

  if ((ptr - str) < 1) return NULL;

  extract = (char *) malloc ((ptr - str)+1);
  if (!extract) 
  { 
    i_printf (1, "i_string_extract_word unable to malloc extract"); 
    return NULL; 
  }

  strncpy (extract, str, (ptr-str));
  extract[ptr-str] = '\0';

  return extract;

}

char* i_string_to_lower (char *str)
{
	unsigned int i;
	char *ret_str;

	if (!str) return NULL;

	ret_str = strdup (str);
	if (!ret_str) { i_printf (1, "i_string_to_lower couldnt strdup(str)"); return NULL; }

	for (i=0; i < (strlen(str)); i++)
	{
		ret_str[i] = tolower (ret_str[i]);
	}
	
	return ret_str;
}

char* i_string_web (char *str)
{
	unsigned int i;
	int special_count = 0;
	int ret_str_size;
	char *ret_str;
	char *dataptr;

	if (!str) return NULL;

	for (i=0; i < strlen(str); i++)
	{
		if (str[i] > 0 && str[i] < 48) special_count++;
		if (str[i] > 57 && str[i] < 65) special_count++;
		if (str[i] > 90 && str[i] < 97) special_count++;
		if (str[i] > 122) special_count++;
	}

	if (special_count == 0) return (strdup(str));

	ret_str_size = strlen (str) + (3 * special_count) + 1;
	ret_str = (char *) malloc (ret_str_size);
	if (!ret_str) return NULL;

	dataptr = ret_str;

	for (i=0; i < strlen(str); i++)
	{
		if (str[i] > 47 && str[i] < 58) { *dataptr = str[i]; dataptr++; continue; }				/* Numbers */
		if (str[i] > 64 && str[i] < 91) { *dataptr = str[i]; dataptr++; continue; }				/* Upper Case */
		if (str[i] > 96 && str[i] < 123) { *dataptr = str[i]; dataptr++; continue; }				/* Upper Case */
	
		/* If we reach here it's a special character */

		snprintf (dataptr, 4, "%%%02x", str[i]);
		dataptr += 3;

	}

	*dataptr = '\0';

	return ret_str;
}

char* i_string_volume (double units, char *unit_str)
{
  char *str;
  double k;
  double m;
  double g;

  if (!unit_str) unit_str = "";
  
  /* Check for a 0 alloc unit or units */
  if (units == 0)
  {
    asprintf (&str, "0%s", unit_str);
    return str;
  }

  /* Set k/m/g factors */
  if (unit_str && (strstr(unit_str, "byte") || strstr(unit_str, "Byte")))
  {
    k = 1024.0;
    m = 1024.0 * 1024.0;
    g = 1024.0 * 1024.0 * 1024.0;
  }
  else
  {
    k = 1000.0;
    m = 1000.0 * 1000.0;
    g = 1000.0 * 1000.0 * 1000.0;
  }

  if (units < k)
  { asprintf (&str, "%.2f%s", units, unit_str); return str; }

  if (units < m)
  { asprintf (&str, "%.2fk%s", units / k, unit_str); return str; }

  if (units < g)
  { asprintf (&str, "%.2fM%s", units / m, unit_str); return str; }

  if ((units / k) < g)
  { asprintf (&str, "%.2fG%s", units / g, unit_str); return str; }

  asprintf (&str, "%.2fT%s", (units / g) * (1 / k), unit_str);

  return str;
}

char* i_string_volume_metric (i_metric *met, i_metric_value *val)
{
  double valflt;

  /* Get value flt, this will take alloc_units into account */
  valflt = i_metric_valflt (met, val);

  return i_string_volume (valflt, met->unit_str);
}

char* i_string_rate (double rate, char *unit_str)
{
  char *str;

  if (!unit_str) unit_str = "";

  if (rate < 1000)
  { asprintf (&str, "%.2f%s", rate, unit_str); return str; }

  if ((rate / 1000) < 1000)
  { asprintf (&str, "%.2fk%s", (double) rate / 1000, unit_str); return str; }

  if ((rate / 1000000) < 1000)
  { asprintf (&str, "%.2fM%s", (double) rate / 1000000, unit_str); return str; }

  if ((rate / 1000000000) < 1000)
  { asprintf (&str, "%.2fG%s", (double) rate / 1000000000, unit_str); return str; }

  asprintf (&str, "%.2fT%s", ((double) rate / 1000) / 1000000000, unit_str);

  return str;
}

char* i_string_rate_metric (i_metric *met, i_metric_value *val)
{
  double valflt;

  valflt = i_metric_valflt (met, val);
  
  return i_string_rate (valflt, met->unit_str);
}

char* i_string_divby_metric (i_metric *met, i_metric_value *val)
{ 
  char *str;
  double valflt;
  valflt = i_metric_valflt (met, val);  /* Will perform div_by */
  asprintf (&str, "%.2f%s", valflt, met->unit_str);
  return str;
}   

