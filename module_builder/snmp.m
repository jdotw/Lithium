/*
 *  snmp.c
 *  ModuleBuilder
 *
 *  Created by James Wilson on 10/02/08.
 *  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
 *
 */

#include <pwd.h>

#include "snmp.h"

static int snmp_running = 0;

int restart_snmp ()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	const char *type = "lithium_mb";
	
	/* Set parameters */
	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_DONT_PERSIST_STATE, 1);
	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, 1);
	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_MIB_PARSE_LABEL, 1);
	setenv ("MIBS", "ALL", 1);
	
	/* Set MIB path */
	struct passwd *pw;
	pw = getpwuid (getuid());
	int pid = getpid ();
	char *cachePath;
	if (pw && pw->pw_dir && [[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSUTF8StringEncoding])
	{
		asprintf (&cachePath, "+%s/Library/Caches/com.lithiumcorp.modulebuilder/%i:%s", 
				  pw->pw_dir, pid, [[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSUTF8StringEncoding]);
		printf ("cachePath is %s\n", cachePath);
		netsnmp_get_mib_directory();
		netsnmp_set_mib_directory (cachePath);
	}
	
	/* Re-Initialise */
	if (snmp_running)
	{
		NSLog (@"Re-starting SNMP");
		shutdown_mib ();
		init_mib ();
	}
	else
	{
		NSLog (@"Starting SNMP");
		init_snmp (type);
		snmp_running = 1;
	}
	
	[pool drain];
	
	return 0;
}