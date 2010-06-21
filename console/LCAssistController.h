//
//  LCAssistController.h
//  Lithium Console
//
//  Created by James Wilson on 26/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h" 
#import "LCBackgroundView.h"
#import "LCEntity.h"
#import "LCSite.h"

#define TAB_WELCOME 0
#define TAB_SITEADD 1
#define TAB_DEVTYPE 2
#define TAB_DEVADD 3
#define TAB_DEVREV 4
#define TAB_FINISHED 5

@interface LCAssistController : NSWindowController 
{
	LCCustomer *customer;
	
	IBOutlet LCBackgroundView *backView;
	IBOutlet NSTabView *tabView;
	IBOutlet NSButton *nextButton;
	IBOutlet NSButton *backButton;
	IBOutlet NSButton *cancelButton;
	IBOutlet NSButton *addSitesButton;
	IBOutlet NSArrayController *siteArrayController;
	
	/* Device Tasks */
	NSMutableDictionary *devTaskDict;
	NSMutableArray *devTasks;
	NSMutableArray *enabledTasks;
	id currentDeviceTask;
	LCSite *addToSiteSelection;
	
	/* Misc */
	int indexBeforeSiteAdd;
	BOOL doNotShowAgain;
}

#pragma mark "Constructors"
- (LCAssistController *) initForCustomer:(LCCustomer *)initCustomer;

#pragma mark "Create Device Tasks"
- (void) createDeviceTasks;


#pragma mark "Navigation Actions"
- (IBAction) nextClicked:(id)sender;
- (IBAction) backClicked:(id)sender;
- (IBAction) cancelCkicked:(id)sender;
- (IBAction) addMoreDevicesClicked:(id)sender;
- (IBAction) addMoreSitesClicked:(id)sender;
- (IBAction) addSiteClicked:(id)sender;

#pragma mark "View Management Methods"
- (void) showWelcome;
- (void) showSiteAdd;
- (void) exitFromSiteAdd;
- (void) showDeviceTypes;
- (void) showFirstDeviceAdd;
- (void) showNextDeviceAdd;
- (void) showPrevDeviceAdd;
- (void) showLastDeviceAdd;
- (void) showDeviceReview;
- (void) showFinished;

#pragma mark "Accessors"
@property (nonatomic,retain) LCCustomer *customer;
- (LCEntity *) selectedSite;
@property (readonly) NSMutableDictionary *devTaskDict;
@property (readonly) NSMutableArray *devTasks;
@property (readonly) NSMutableArray *enabledTasks;
@property (nonatomic, assign) id currentDeviceTask;
@property (nonatomic, assign) int indexBeforeSiteAdd;
@property (nonatomic, assign) BOOL doNotShowAgain;
@property (nonatomic, assign) LCSite *addToSiteSelection;

@end
