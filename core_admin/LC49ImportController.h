//
//  LC49ImportController.h
//  LCAdminTools
//
//  Created by James Wilson on 2/06/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCHelperToolDelegate.h"
#import "LC49ImportTask.h"
#import "LC49ImportHelperTool.h"
#import "LCSetupController.h"

@interface LC49ImportController : LCHelperToolDelegate 
{
	/* UI Elements */
	IBOutlet NSWindow *windowForSheet;
	IBOutlet NSWindow *sheet;	
	IBOutlet NSTabView *contentTabView;
	IBOutlet NSTextView *errorLogTextView;
	IBOutlet LCSetupController *setupController;
	
	/* Lithium 4.9 Path */
	NSString *l49Path;
	
	/* Helper */
	LC49ImportHelperTool *helper;
	
	/* Tasks */
	NSMutableArray *tasks;
	LC49ImportTask *currentTask;
	
	/* Progress */
	int rrdFilesTotal;
	int rrdFilesCopied;
}

@property (copy) NSString *l49Path;
@property (copy) NSMutableArray *tasks;

#pragma mark "External Actions"
- (IBAction) importAssistantClicked:(id)sender;
- (BOOL) checkForExistingInstall;

#pragma mark "Common Tab Methods"
- (IBAction) cancelClicked:(id)sender;

#pragma mark "Welcome Tab Methods"
- (IBAction) welcomeNextClicked:(id)sender;
- (IBAction) revealOldPathClicked:(id)sender;

#pragma mark "Detail Tab Methods"
- (IBAction) detailNextClicked:(id)sender;
- (IBAction) revealNewPathClicked:(id)sender;

#pragma mark "Import Methods"
- (IBAction) sendLogToSupportClicked:(id)sender;

#pragma mark "Tasks"
- (NSMutableArray *) tasks;
- (void) setTasks:(NSMutableArray *)value;
- (void) insertObject:(id)obj inTasksAtIndex:(unsigned int)index;
- (void) removeObjectFromTasksAtIndex:(unsigned int)index;
@property (nonatomic,retain) LC49ImportTask *currentTask;


@end
