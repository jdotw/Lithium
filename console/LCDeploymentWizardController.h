//
//  LCDeploymentWizardController.h
//  Lithium Console
//
//  Created by James Wilson on 10/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBackgroundView.h"

#import "LCCustomerList.h"
#import "LCCoreDeployment.h"

@interface LCDeploymentWizardController : NSWindowController 
{
	IBOutlet LCBackgroundView *backgroundView;
	IBOutlet NSWindow *testSheet;
	NSString *ipAddress;
	NSString *url;
	NSString *errorString;
	BOOL editUrl;
	LCCoreDeployment *deployment;
}

#pragma mark "Constructors"
- (LCDeploymentWizardController *) init;
- (void) dealloc;

#pragma mark "Window Delegate Methods"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "UI Actions"
- (IBAction) testAndContinueClicked:(id)sender;
- (IBAction) advancedConfigClicked:(id)sender;
- (IBAction) addDeploymentClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;
- (IBAction) testAgainClicked:(id)sender;

#pragma mark "Accessors"
- (NSString *) ipAddress;
- (void) setIpAddress:(NSString *)value;
- (NSString *) url;
- (void) setUrl:(NSString *)value;
- (BOOL) editUrl;
- (void) setEditUrl:(BOOL)value;
- (NSString *) errorString;
- (void) setErrorString:(NSString *)value;
- (LCCustomerList *) customerList;
- (LCCoreDeployment *) deployment;
- (void) setDeployment:(LCCoreDeployment *)newDep;

@property (nonatomic,retain) LCBackgroundView *backgroundView;
@property (nonatomic,retain) NSWindow *testSheet;
@property (retain,getter=ipAddress,setter=setIpAddress:) NSString *ipAddress;
@property (retain,getter=url,setter=setUrl:) NSString *url;
@property (retain,getter=errorString,setter=setErrorString:) NSString *errorString;
@property (getter=editUrl,setter=setEditUrl:) BOOL editUrl;
@property (retain,getter=deployment,setter=setDeployment:) LCCoreDeployment *deployment;
@end
