#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>

#include "snmp.h"

/* Variable String utilities */

char* l_snmp_var_to_str (struct variable_list *var)
{
  /* Create a string to represent the variable */

  int num;
  char *str = NULL;
  size_t str_size = 0;
  size_t out_size = 0;
  
  switch (var->type)
  {
    case ASN_INTEGER:
      asprintf (&str, "%li", (long) *var->val.integer);
      break;
    case ASN_GAUGE:
    case ASN_COUNTER:
    case ASN_UINTEGER:
    case ASN_TIMETICKS:
      asprintf (&str, "%lu", (unsigned long) *var->val.integer);
      break;
    case ASN_IPADDRESS:
      asprintf (&str, "%d.%d.%d.%d",
        (u_char) var->val.string[0], (u_char) var->val.string[1],
        (u_char) var->val.string[2], (u_char) var->val.string[3]);
      break;
    case ASN_OCTET_STR:
      str_size = var->val_len+1;
      str = (char*) malloc (str_size);
      memcpy (str, var->val.string, var->val_len);
      str[var->val_len] = '\0';
      break;
    default:
      num = sprint_realloc_by_type ((u_char **) &str, &str_size, &out_size, 1, var, NULL, NULL, NULL);
      if (num != 1) return NULL;    /* SNMP lib funcs return 1 on success */
      break;
  }

  return str;
}

char* l_snmp_var_to_hexstr (struct variable_list *var)
{
  /* Create a hex-string to represent the variable */
  unsigned int i;
  char *str = NULL;

  switch (var->type)
  {
    case ASN_OCTET_STR:
      for (i=0; i < var->val_len; i++)
      {
        char *temp_str;
        if (str)
        { 
          asprintf (&temp_str, "%s:%.2X", str, var->val.string[i]); 
          free (str);
          str = temp_str;
        }
        else
        { asprintf (&str, "%.2X", var->val.string[i]); }
      }
      break;
  }

  return str;
}

char* l_snmp_var_to_hexnumstr (struct variable_list *var)
{
  /* Create a hex-string to represent the variable */
  unsigned int i;
  char *str = NULL;

  switch (var->type)
  {
    case ASN_OCTET_STR:
      for (i=0; i < var->val_len; i++)
      {
        char *temp_str;
        if (str)
        {
          asprintf (&temp_str, "%s%.2X", str, var->val.string[i]);
          free (str);
          str = temp_str;
        }
        else
        { asprintf (&str, "%.2X", var->val.string[i]); }
      }
      break;
  }

  return str;
}


/* FIX LEGACY (replace by func above) */

char* l_snmp_get_string_from_pdu (struct snmp_pdu *pdu)
{ return l_snmp_var_to_str (pdu->variables); }

char* l_snmp_get_string_from_vars (struct variable_list *vars)
{ return l_snmp_var_to_str (vars); }

