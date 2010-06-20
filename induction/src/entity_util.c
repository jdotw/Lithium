#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "callback.h"
#include "colour.h"
#include "cement.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "entity.h"

/** \addtogroup entity Monitored Entities
 * @{
 */

/*
 * Cement Entity Utility Functions
 */

/* Resource Type */

int i_entity_restype (unsigned short ent_type)
{
  /* This function returns the resource type that
   * the specified ent_type resides within
   */
  switch (ent_type)
  {
    case ENT_CUSTOMER:
    case ENT_SITE:
      return RES_CUSTOMER;
    case ENT_DEVICE:
    case ENT_CONTAINER:
    case ENT_OBJECT:
    case ENT_METRIC:
    case ENT_TRIGGER:
      return RES_DEVICE;
  }

  return -1;
}

/* Colour Utilities */

char* i_entity_statecolstr_fg (unsigned short state)
{
  /* fg = Foreground = Full Colour */
  switch (state)
  {
    case ENTSTATE_GREEN: return COLOUR_GREEN_FG;
    case ENTSTATE_YELLOW: return COLOUR_YELLOW_FG;
    case ENTSTATE_ORANGE: return COLOUR_ORANGE_FG;
    case ENTSTATE_RED: return COLOUR_RED_FG;
  }

  return NULL;
}

char *i_entity_statecolstr_bg (unsigned short state)
{
  /* bg = Background = Half Colour */
  switch (state)
  {
    case ENTSTATE_GREEN: return COLOUR_GREEN_BG;
    case ENTSTATE_YELLOW: return COLOUR_YELLOW_BG;
    case ENTSTATE_ORANGE: return COLOUR_ORANGE_BG;
    case ENTSTATE_RED: return COLOUR_RED_BG;
  }

  return NULL;
}

/* @} */
