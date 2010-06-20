#include <stdlib.h>

#include "induction.h"
#include "event.h"
#include "list.h"

/* Event Related Functions */

i_event* i_event_create ()
{
  i_event *event;

  event = (i_event *) malloc (sizeof(i_event));
  if (!event)
  {
    i_printf (1, "i_event_create failed to malloc event");
    return NULL;
  }
  memset (event, 0, sizeof(i_event));

  event->id = -1;   /* -1 == not yet alllocated */

  event->inc_list = i_list_create ();
  if (!event->inc_list)
  { i_printf (1, "i_event_create failed to create incident list"); i_event_free (event); return NULL; }
  i_list_set_destructor (event->inc_list, i_event_inc_free);

  return event;
}

void i_event_free (void *eventptr)
{
  i_event *event = eventptr;

  if (!event) return;

  if (event->inc_list) i_list_free (event->inc_list);

  free (event);
}

/* String Utils */

char *i_event_state_str (i_event *event)
{
  switch (event->state)
  {
    case EVENT_STATE_HOLDDOWN: return "Hold-down";
    case EVENT_STATE_LIVE: return "Live";
    case EVENT_STATE_ARCHIVED: return "Archived";
    default: return "Unknown";
  }

  return NULL;
}
