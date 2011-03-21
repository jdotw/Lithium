#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>
#include <induction/xml.h>

#include "scene.h"

/*
 * Scene overlays
 */

/* Struct */

l_scene_overlay* l_scene_overlay_create ()
{
  l_scene_overlay *scene_overlay = malloc (sizeof(l_scene_overlay));
  memset (scene_overlay, 0, sizeof(l_scene_overlay));

  return scene_overlay;
}

void l_scene_overlay_free (void *scene_overlayptr)
{
  l_scene_overlay *scene_overlay = scene_overlayptr;

  if (scene_overlay->entdesc) i_entity_descriptor_free (scene_overlay->entdesc);

  free (scene_overlay);
}

