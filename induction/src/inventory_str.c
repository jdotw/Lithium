#include <stdlib.h>

#include "induction.h"
#include "inventory.h"

/* String utilities */

char* i_inventory_type_str (int type)
{
  switch (type)
  {
    /* Reserved types */
    case INV_RESERVED: return "Reserved";
    case INV_ALL: return "All";

    /* Software types */
    case INV_SOFTWARE: return "Software";
    case INV_OS: return "Operating System";
    case INV_APP: return "Application";
    case INV_DRIVER: return "Device driver";

    /* Hardware types */
    case INV_HARDWARE: return "Hardware";
    case INV_CHASSIS: return "Chassis";
    case INV_MAINBOARD: return "Mainboard";
    case INV_INTCARD: return "Interface Card";
    case INV_STORAGE: return "Storage";
    case INV_PERIPHERAL: return "Peripheral";

    default: return "Other";
  }

  return NULL;
}


