//
//  LCAdminToolsController.m
//  LCAdminTools
//
//  Created by Liam Elliott on 18/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCStatusController.h"
#include <SecurityFoundation/SFAuthorization.h>

@implementation LCStatusController

#pragma mark "Constructors"

-(void)awakeFromNib
{
	[self setProcList:[LCProcessList processList]];
	refreshTimer = [[NSTimer scheduledTimerWithTimeInterval:1.0f
													 target:self 
												   selector:@selector(refreshList:)
												   userInfo:nil
													repeats:true] retain];
}
	
-(void)dealloc
{
	if (procList) [procList release];
	if (refreshTimer) [refreshTimer release];
	[super dealloc];
}

#pragma mark "Refresh Timer"

-(void)refreshList:(NSTimer *)timer
{
	[procList getProcessList];
}

#pragma mark "UI Actions"

-(IBAction) startLithium:(id)sender
{
	AuthorizationRef authRef;
	OSStatus err;
	
	err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
	if (err == 0)
	{
		FILE *commsPipe = NULL;
		char *args[] = { "lithium", "start", NULL};
		char *myPath = (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		
		err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&commsPipe);	
		err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
	}
	
	[procList getProcessList];
}

-(IBAction)stopLithium:(id)sender
{
	AuthorizationRef authRef;
	OSStatus err;
	
	err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
	if (err == 0)
	{
		FILE *commsPipe = NULL;
		char *args[] = { "lithium", "stop", NULL};
		char *myPath = (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		
		err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&commsPipe);	
		err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
	}

	[procList getProcessList];
}

-(IBAction) startPostgres:(id)sender
{
	AuthorizationRef authRef;
	OSStatus err;
	
	err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
	if (err == 0)
	{
		FILE *commsPipe = NULL;
		char *args[] = { "postgres", "start", NULL};
		char *myPath = (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		
		err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&commsPipe);	
		err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
	}
	
	[procList getProcessList];
}

-(IBAction)stopPostgres:(id)sender
{
	AuthorizationRef authRef;
	OSStatus err;
	
	err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
	if (err == 0)
	{
		FILE *commsPipe = NULL;
		char *args[] = { "postgres", "stop", NULL};
		char *myPath= (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		
		err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&commsPipe);	
		err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
	}
	
	[procList getProcessList];
}

-(IBAction) startApache:(id)sender
{
	AuthorizationRef authRef;
	OSStatus err;
	
	err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
	if (err == 0)
	{
		FILE *commsPipe = NULL;
		char *args[] = { "clientd", "start", NULL};
		char *myPath = (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		
		err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&commsPipe);	
		err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
	}
	[procList getProcessList];
}

-(IBAction)stopApache:(id)sender
{
	AuthorizationRef authRef;
	OSStatus err;
	
	err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
	if (err == 0)
	{
		FILE *commsPipe = NULL;
		char *args[] = { "clientd", "stop", NULL};
		char *myPath= (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		
		err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&commsPipe);	
		err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
	}
	[procList getProcessList];
}


#pragma mark "Accessors"

-(LCProcessList *)procList
{return procList;}

-(void)setProcList:(LCProcessList *)newList
{
	if (procList) [procList release];
	procList = [newList retain];
}

@end
