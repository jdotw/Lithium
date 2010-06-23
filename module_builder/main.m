//
//  main.m
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright LithiumCorp Pty Ltd 2008 . All rights reserved.
//

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#import <Cocoa/Cocoa.h>

int main(int argc, char *argv[])
{
	/* Init SNMP */
	restart_snmp ();
	
	return NSApplicationMain(argc, (const char **) argv);
}
