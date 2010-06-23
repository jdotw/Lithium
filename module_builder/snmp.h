/*
 *  snmp.h
 *  ModuleBuilder
 *
 *  Created by James Wilson on 10/02/08.
 *  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
 *
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

void snmp_set_homedir (char *dir);
int restart_snmp ();
