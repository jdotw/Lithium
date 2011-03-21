#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/name.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>

#include "record.h"

/* 
 * Recording Rules
 */

/* Enable Recording on a metric */

int l_record_enable (i_resource *self, i_metric *met)
{
  /* Set flag */
  met->record_enabled = 1;

  /* Increase the recorded values to 1 hours worth of samples */
//  met->val_list_maxsize = (60 * 60) / self->hierarchy->dev->refresh_interval;

  return 0;
}

/* Disable recording on a metric */

int l_record_disable (i_resource *self, i_metric *met)
{
  met->record_enabled = 0;

  return 0;
}

