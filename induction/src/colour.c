#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "cement.h"
#include "entity.h"
#include "colour.h"

/** \addtogroup entity_state Entity State
 * @ingroup entity
 * @{
 */

/*
 * CEMent Entity Colour
 */

char* i_colour_fg_str (short state)
{
  switch (state)
  {
    case ENTSTATE_UNKNOWN: return COLOUR_GREY_BG;
    case ENTSTATE_GREEN: return COLOUR_GREEN_FG;
    case ENTSTATE_YELLOW: return COLOUR_YELLOW_FG;
    case ENTSTATE_ORANGE: return COLOUR_ORANGE_FG;
    case ENTSTATE_RED: return COLOUR_RED_FG;
  }

  return NULL;
}

char* i_colour_bg_str (short state)
{
  switch (state)
  {
    case ENTSTATE_UNKNOWN: return COLOUR_GREY_BG;
    case ENTSTATE_GREEN: return COLOUR_GREEN_BG;
    case ENTSTATE_YELLOW: return COLOUR_YELLOW_BG;
    case ENTSTATE_ORANGE: return COLOUR_ORANGE_BG;
    case ENTSTATE_RED: return COLOUR_RED_BG;
  }

  return NULL;
}

/* @} */
