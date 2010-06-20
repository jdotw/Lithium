//
//  LCDeploymentWizardController.m
//  Lithium Console
//
//  Created by James Wilson on 10/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCDeploymentWizardController.h"

#import "LCCustomerList.h"
#import "LCPreferencesController.h"
#import "LCConsoleController.h"
#import "LCBrowser2Controller.h"


@implementation LCDeploymentWizardController

#pragma mark "Constructors"

- (LCDeploymentWizardController *) init
{
	/* Super class init */
	[super initWithWindowNibName:@"DeploymentWizard"];
	
	/* Window setup */
	[self window];
	[backgroundView setImage:[NSImage imageNamed:@"slateback.png"]];	
	
	/* Show Window */
	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Autorelease */
	[self autorelease];
}

#pragma mark "UI Actions"

- (IBAction) testAndContinueClicked:(id)sender
{
	/* Create Deployment */
	[self setDeployment:[[[LCCoreDeployment alloc] init] autorelease]];
	NSURL *urlObject = [NSURL URLWithString:url];
	[deployment setAddress:[urlObject host]];

	/* Test Deploy */
	[deployment testDeployment];
	
	/* Show sheet */
	[NSApp beginSheet:testSheet 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];		
	
}

- (IBAction) advancedConfigClicked:(id)sender
{
	[LCPreferencesController preferencesController];
	[[self window] performClose:self];
}

- (IBAction) addDeploymentClicked:(id)sender
{
	/* Add Deployment */
	[[LCCustomerList masterList] insertObject:deployment 
				   inStaticDeploymentsAtIndex:[[[LCCustomerList masterList] staticDeployments] count]];
	[self setDeployment:nil];
	
	/* Close sheet */
	[NSApp endSheet:testSheet];
	[testSheet close];
	
	/* Check for a browser */
	if ([[LCBrowser2Controller activeControllers] count] < 1)
	{ [[[LCBrowser2Controller alloc] init] autorelease]; }
	
	/* Close wizard */
	[self close];
}

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:testSheet];
	[testSheet close];	
}

- (IBAction) testAgainClicked:(id)sender
{
	[deployment testDeployment];
}

#pragma mark "Accessors"
- (NSString *) ipAddress
{
	return ipAddress;
}

- (void) setIpAddress:(NSString *)value
{
	[ipAddress release];
	ipAddress = [value retain];
	NSString *urlString = [NSString stringWithFormat:@"http://%@:51180", ipAddress];
	[self setUrl:urlString];
}

- (NSString *) url
{
	return url;
}

- (void) setUrl:(NSString *)value
{
	[url release];
	url = [value retain];
}

- (BOOL) editUrl
{
	return editUrl;	
}

- (void) setEditUrl:(BOOL)value
{
	editUrl = value;
}

- (NSString *) errorString
{ return errorString; }

- (void) setErrorString:(NSString *)value
{
	[errorString release];
	errorString = [value retain];
}

- (LCCustomerList *) customerList
{ return [LCCustomerList masterList]; }

- (LCCoreDeployment *) deployment
{ return deployment; }

- (void) setDeployment:(LCCoreDeployment *)newDep
{ 
	[deployment release];
	deployment = [newDep retain];
}

@synthesize backgroundView;
@synthesize testSheet;
@end
