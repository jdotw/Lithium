#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/inventory.h>
#include <induction/construct.h>
#include <induction/hierarchy.h>
#include <induction/vendor.h>

#include "osx.h"

/* OSX Utility Functions */

static int static_present = 0;
static int static_version = 0;  /* 10.4 == 100400, 10.5 == 100500, 10.6 == 100600 */

int l_osx_present()
{ return static_present; }

void l_osx_set_present(int value)
{ static_present = value; }

int l_osx_version()
{ return static_version; }

void l_osx_set_version(int value)
{ static_version = value; }

