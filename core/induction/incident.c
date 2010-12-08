#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "induction.h"
#include "list.h"
#include "data.h"
#include "form.h"
#include "socket.h"
#include "message.h"
#include "msgproc.h"
#include "cement.h"
#include "entity.h"
#include "timer.h"
#include "incident.h"

/** \addtogroup incident Incident Reporting
 * @ingroup backend
 * @{
 */

/*
 * Incidents
 *
 * An incident occurs when a trigger becomes active.
 * Incidents are reported to the customer resource.
 */

/* Struct Manipulation */

i_incident* i_incident_create ()
{
  i_incident *inc;

  inc = (i_incident *) malloc (sizeof(i_incident));
  if (!inc)
  { i_printf (1, "i_incident_create failed to malloc i_incident struct"); return NULL; }
  memset (inc, 0, sizeof(i_incident));

  inc->id = -1;     /* Unassigned ID */

  return inc;
}

void i_incident_free (void *incptr)
{
  i_incident *inc = incptr;

  if (!inc) return;

  if (inc->ent) i_entity_descriptor_free (inc->ent);
  if (inc->prev_ent) i_entity_descriptor_free (inc->prev_ent);
  if (inc->orig_ent) i_entity_descriptor_free (inc->orig_ent);
  if (inc->met) i_entity_descriptor_free (inc->met);
  if (inc->msgcb) i_msgproc_callback_remove (inc->msgcb);
  if (inc->raised_valstr) free (inc->raised_valstr);
  if (inc->cleared_valstr) free (inc->cleared_valstr);
  if (inc->trg_xval_str) free (inc->trg_xval_str);
  if (inc->trg_yval_str) free (inc->trg_yval_str);
  if (inc->action_list) i_list_free (inc->action_list);
  if (inc->destruct_timer) i_timer_remove (inc->destruct_timer);

  free (inc);
}

/* Data/Struct Conversion */

char* i_incident_data (i_incident *inc, int *datasizeptr)
{
  int datasize;
  char *data;
  char *dataptr;
  int entdatasize;
  char *entdata;
  int metdatasize;
  char *metdata;

  /* Initialise datasizeptr */
  memset (datasizeptr, 0, sizeof(int));

  /* Convert entity descriptor to data */
  entdata = i_entity_descriptor_data (inc->ent, NULL, &entdatasize);
  metdata = i_entity_descriptor_data (inc->met, NULL, &metdatasize);

  /* Create data */
  datasize = (5*sizeof(long)) + (2*sizeof(unsigned short)) + (7*sizeof(int)) + entdatasize + metdatasize;
  if (inc->raised_valstr) datasize += strlen(inc->raised_valstr) + 1;
  if (inc->cleared_valstr) datasize += strlen(inc->cleared_valstr) + 1;
  if (inc->trg_xval_str) datasize += strlen(inc->trg_xval_str) + 1;
  if (inc->trg_yval_str) datasize += strlen(inc->trg_yval_str) + 1;
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_incident_data failed to malloc data (%i bytes)", datasize); return NULL; }
  dataptr = data;

  /* Add ID */
  dataptr = i_data_add_long (data, dataptr, datasize, &inc->id);
  if (!dataptr)
  { free (data); if (entdata) free (entdata); return NULL; }

  /* Add State */
  dataptr = i_data_add_ushort (data, dataptr, datasize, &inc->state);
  if (!dataptr)
  { free (data); if (entdata) free (entdata); return NULL; }
  
  /* Add Type */
  dataptr = i_data_add_ushort (data, dataptr, datasize, &inc->type);
  if (!dataptr)
  { free (data); if (entdata) free (entdata); return NULL; }

  /* Add Entity Descriptor Data */
  dataptr = i_data_add_chunk (data, dataptr, datasize, entdata, entdatasize);
  if (entdata) free (entdata);
  if (!dataptr)
  { free (data); return NULL; }

  /* Add Metric Descriptor Data */
  dataptr = i_data_add_chunk (data, dataptr, datasize, metdata, metdatasize);
  if (metdata) free (metdata);
  if (!dataptr)
  { free (data); return NULL; }

  /* Add Start Timestamp */
  dataptr = i_data_add_long (data, dataptr, datasize, (long *) &inc->start_tv.tv_sec);
  if (!dataptr)
  { free (data); return NULL; }
  dataptr = i_data_add_long (data, dataptr, datasize, (long *) &inc->start_tv.tv_usec);
  if (!dataptr)
  { free (data); return NULL; }

  /* Add Start Timestamp */
  dataptr = i_data_add_long (data, dataptr, datasize, (long *) &inc->end_tv.tv_sec);
  if (!dataptr)
  { free (data); return NULL; }
  dataptr = i_data_add_long (data, dataptr, datasize, (long *) &inc->end_tv.tv_usec);
  if (!dataptr)
  { free (data); return NULL; }

  /* Add Raised Value String */
  dataptr = i_data_add_string (data, dataptr, datasize, inc->raised_valstr);
  if (!dataptr)
  { free (data); return NULL; }

  /* Add Cleared Value String */
  dataptr = i_data_add_string (data, dataptr, datasize, inc->cleared_valstr);
  if (!dataptr)
  { free (data); return NULL; }

  /* Add Trigger Info */
  dataptr = i_data_add_int (data, dataptr, datasize, &inc->trg_type);
  if (!dataptr)
  { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, inc->trg_xval_str);
  if (!dataptr)
  { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, inc->trg_yval_str);
  if (!dataptr)
  { free (data); return NULL; }

  /* Copy datasize */
  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

i_incident* i_incident_struct (char *data, int datasize)
{
  int offset;
  int entdatasize;
  char *entdata;
  char *dataptr = data;
  i_incident *inc;

  /* Create incident */
  inc = i_incident_create ();
  if (!inc)
  { i_printf (1, "i_incident_struct failed to create i_incident struct"); return NULL; }

  /* Get ID */
  inc->id = i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get ID from data"); i_incident_free (inc); return NULL; }
  dataptr += offset;
  
  /* Get state */
  inc->state = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get state from data"); i_incident_free (inc); return NULL; }
  dataptr += offset;
  
  /* Get type */
  inc->type = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get type from data"); i_incident_free (inc); return NULL; }
  dataptr += offset;

  /* Get entity descriptor data */
  entdata = i_data_get_chunk (data, dataptr, datasize, &entdatasize, &offset);
  if (!entdata || offset < 1)
  { i_printf (1, "i_incident_struct failed to get entdesc chunk from data"); i_incident_free (inc); return NULL; }
  dataptr += offset;

  /* Convert entdesc data to struct */
  inc->ent = i_entity_descriptor_struct (entdata, entdatasize);
  free (entdata);
  if (!inc->ent)
  { i_printf (1, "i_incident_struct failed to convert entdesc data to struct"); i_incident_free (inc); return NULL; }
  inc->orig_ent = i_entity_descriptor_duplicate (inc->ent);
  inc->highest_opstate = inc->ent->opstate;
  inc->lowest_opstate = inc->ent->opstate;
  
  /* Get metric descriptor data */
  entdata = i_data_get_chunk (data, dataptr, datasize, &entdatasize, &offset);
  if (!entdata || offset < 1)
  { i_printf (1, "i_incident_struct failed to get metdesc chunk from data"); i_incident_free (inc); return NULL; }
  dataptr += offset;

  /* Convert entdesc data to metric struct */
  inc->met = i_entity_descriptor_struct (entdata, entdatasize);
  free (entdata);
  if (!inc->met)
  { i_printf (1, "i_incident_struct failed to convert entdesc data to metric struct"); i_incident_free (inc); return NULL; }
  
  /* Get Start Timestamp */
  inc->start_tv.tv_sec = (time_t) i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get start_tv.tv_sec from data"); i_incident_free (inc); return NULL; }
  dataptr += offset;
  inc->start_tv.tv_usec = (time_t) i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get start_tv.tv_usec from data"); i_incident_free (inc); return NULL; }
  dataptr += offset;
  
  /* Get End Timestamp */
  inc->end_tv.tv_sec = (time_t) i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get end_tv.tv_sec from data"); i_incident_free (inc); return NULL; }
  dataptr += offset;
  inc->end_tv.tv_usec = (time_t) i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get start_tv.tv_usec from data"); i_incident_free (inc); return NULL; }
  dataptr += offset;

  /* Get Raised Value String */
  inc->raised_valstr = (char *) i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get raised value string"); i_incident_free (inc); return NULL; }
  dataptr += offset;

  /* Get Cleared Value String */
  inc->cleared_valstr = (char *) i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get cleared value string"); i_incident_free (inc); return NULL; }
  dataptr += offset;

  /* Get Trigger Info */
  inc->trg_type = (int) i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get inc->trg_type from data"); i_incident_free (inc); return NULL; }
  dataptr += offset;
  inc->trg_xval_str = (char *) i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get trigger x value string"); i_incident_free (inc); return NULL; }
  dataptr += offset;
  inc->trg_yval_str = (char *) i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_incident_struct failed to get trigger y value string"); i_incident_free (inc); return NULL; }
  dataptr += offset;

  return inc;
}

/* URL */

char* i_incident_url (i_incident *inc)
{
  /* Returns a lithium://node/incident/incid style URL */
  char hostname[256];
  gethostname (hostname, 255);
  char *url_str;
  asprintf (&url_str, "lithium://%s/incident/%s/%li", hostname, inc->ent->cust_name, inc->id);

  return url_str;
}

/* String Utility */

char* i_incident_typestr (unsigned short type)
{
  switch (type)
  {
    case INCTYPE_ADMINSTATE:  return "Administrative";
    case INCTYPE_OPSTATE: return "Operational";
    default: return "Unknown";
  }

  return NULL;
}

char* i_incident_statestr (unsigned short state)
{
  switch (state)
  {
    case INCSTATE_INACTIVE: return "Inactive";
    case INCSTATE_ACTIVE: return "Active";
    default: return "Unknown";
  }

  return NULL;
}

/* @} */
