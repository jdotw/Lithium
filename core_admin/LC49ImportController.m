//
//  LC49ImportController.m
//  LCAdminTools
//
//  Created by James Wilson on 2/06/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LC49ImportController.h"

@interface LC49ImportController (Private)

- (void) showAssistant;
- (void) startImport;

@end

@implementation LC49ImportController

- (void) awakeFromNib
{
	self.tasks = [NSMutableArray array];
}

#pragma mark "Properties"

@synthesize l49Path;

#pragma mark "External Actions"

- (IBAction) importAssistantClicked:(id)sender
{
	/* Check a previous install was found */
	if (!self.l49Path)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"No Lithium 4.9 Installation Found" 
										  defaultButton:@"OK" 
										alternateButton:nil 
											otherButton:nil 
							  informativeTextWithFormat:@"No existing installation of Lithium 4.9 was found in either /Lithium or /Lithium-4.9-Archive."];
		[alert beginSheetModalForWindow:windowForSheet modalDelegate:self didEndSelector:nil contextInfo:nil];
	}
	
	/* Show Assist */
	[self showAssistant];
}

- (BOOL) checkForExistingInstall
{
	if ([[NSFileManager defaultManager] fileExistsAtPath:@"/Lithium/var/postgres"])
	{
		self.l49Path = @"/Lithium";
		if (![[NSUserDefaults standardUserDefaults] boolForKey:@"DoNotShowImportAssistant"]) [self showAssistant];
		return YES;
	}
	else if ([[NSFileManager defaultManager] fileExistsAtPath:@"/Lithium-4.9-Archive/var/postgres"])
	{
		self.l49Path = @"/Lithium-4.9-Archive";
		return NO;
	}
	else 
	{ return NO; }
}

- (void) showAssistant
{
	while ([tasks count])
	{ [self removeObjectFromTasksAtIndex:0]; }
	[contentTabView selectTabViewItemWithIdentifier:@"welcome"];
	[NSApp beginSheet:sheet
	   modalForWindow:windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

#pragma mark "Common Tab Methods"

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:sheet];
	[sheet close];
	
	NSString *profile = [NSString stringWithContentsOfFile:@"/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/profile.php" 
												  encoding:NSUTF8StringEncoding
													 error:nil];
	if (!profile)
	{
		/* Initial Setup has not been run */
		[setupController runSetupWizard:self];
	}	
}
	 
#pragma mark "Welcome Tab Methods"

- (IBAction) welcomeNextClicked:(id)sender
{
	[contentTabView selectTabViewItemWithIdentifier:@"detail"];	
}

- (IBAction) revealOldPathClicked:(id)sender
{
	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"file://%@", l49Path]]];
}

#pragma mark "Detail Tab Methods"

- (IBAction) detailNextClicked:(id)sender
{
	[contentTabView selectTabViewItemWithIdentifier:@"progress"];	
	[self startImport];
}

- (IBAction) revealNewPathClicked:(id)sender
{
	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"file:///Library/Application%20Support/Lithium"]];
}

#pragma mark "Import Methods"

- (void) startImport
{	
	/* Create helper */
	helper = [[LC49ImportHelperTool alloc] initWithDelegate:self];

	/* Stop All Processes */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Shutdown Lithium 5.0 Components";
	[helper allStop];		
}

- (void) importFinished
{
	[contentTabView selectTabViewItemWithIdentifier:@"success"];
}

- (void) importFailed
{
	/* Setup Text View */
	[[errorLogTextView textStorage] deleteCharactersInRange:NSMakeRange(0, [[errorLogTextView textStorage] length])];
	for (LC49ImportTask *task in tasks)
	{
		NSRange zeroRange = { 0, 0 };
		[errorLogTextView setSelectedRange:zeroRange];
		[errorLogTextView insertText:[NSString stringWithFormat:@"%@ : %@\n", task.name, task.result]];
	}
	
	/* Show error tab */
	[contentTabView selectTabViewItemWithIdentifier:@"failed"];
}

- (IBAction) sendLogToSupportClicked:(id)sender
{
	
}

#pragma mark "All Stop Delegate Methods"

- (void) allStopDidFinish:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];

	/* Check if path needs re-instating */
	if ([self.l49Path isEqualToString:@"/Lithium"])
	{			
		/* Path is OK, Bring up the old database */
		self.currentTask = [LC49ImportTask new];
		self.currentTask.name = @"Start Lithium 4.9 Database";
		[helper startL49Database];	
	}
	else
	{
		/* Path is OK, Bring up the old database */
		self.currentTask = [LC49ImportTask new];
		self.currentTask.name = @"Reinstate Lithium 4.9 Root Directory";
		[helper reinstateL49Root];	
	}
}

#pragma mark "Reinstate Old Root Delegate Methods"

- (void) reinstateOldRootDidFinish:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	
	/* Bring up the old database */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Start Lithium 4.9 Database";
	[helper startL49Database];		
}

#pragma mark "Database Bring-Up Delegate Methods"

- (void) oldDatabaseReady:(LC49ImportHelperTool *)sender
{
	/* Called by the database helper tool to signal that
	 * the old database is up and running
	 */
	
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	
	/* Dump Database */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Export Lithium 4.9 Database";
	[helper exportL49Database];
	
}

- (void) oldDatabaseFailed:(LC49ImportHelperTool *)sender
{
	/* Called by the database helper tool to singla that
	 * the old data base failed to start */

	/* Update task */
	currentTask.result = helper.status;
	[currentTask setFailed];
	
	/* End Import */
	[self importFailed];
}

#pragma mark "Database Export Delegate Methods"

- (void) oldDatabaseExportDidFinish:(LC49ImportHelperTool *)sender
{
	/* Called by the helper tool to signal that the 
	 * old database has been exported
	 */
	
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	
	/* Shutdown Old */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Shutdown Lithium 4.9 Database";
	[helper stopL49Database];
}

- (void) oldDatabaseExportDidFail:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setFailed];
	
	/* End Import */
	[self importFailed];
}

#pragma mark "Old Database Stop Delegate Methods"

- (void) oldDatabaseStopDidFinish:(LC49ImportHelperTool *)sender
{
	/* Called by the helper tool to signal that the 
	 * old database has been stopped
	 */
	
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	
	/* Start New */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Start Lithium 5.0 Database";
	[helper startL50Database];
}

- (void) oldDatabaseStopDidFail:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setFailed];
	
	/* End Import */
	[self importFailed];
}

#pragma mark "New Database Start Delegate Methods"

- (void) newDatabaseStartDidFinish:(LC49ImportHelperTool *)sender
{
	/* Called by the helper tool to signal that the 
	 * new database has been started
	 */
	
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	
	/* Start New */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Import Data into Lithium 5.0 Database";
	[helper importL50Database];
}

- (void) newDatabaseStartDidFail:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setFailed];
	
	/* End Import */
	[self importFailed];
}

#pragma mark "New Database Import Delegate Methods"

- (void) newDatabaseImportDidFinish:(LC49ImportHelperTool *)sender
{
	/* Called by the helper tool to signal that the 
	 * new database has been imported
	 */
	
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	
	/* Start New */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Shutdown Lithium 5.0 Database";
	[helper stopL50Database];
}

- (void) newDatabaseImportDidFail:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setFailed];
	
	/* End Import */
	[self importFailed];
}

#pragma mark "New Database Stop Delegate Methods"

- (void) newDatabaseStopDidFinish:(LC49ImportHelperTool *)sender
{
	/* Called by the helper tool to signal that the 
	 * new database has been stopped
	 */
	
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	
	/* Start New */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Copy Monitoring Data Files";
	self.currentTask.isIndeterminate = NO;
	rrdFilesTotal = 0;
	rrdFilesCopied = 0;
	[helper copyL49RRDFiles];
}

- (void) newDatabaseStopDidFail:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setFailed];
	
	/* End Import */
	[self importFailed];
}

#pragma mark "RRD File Copy Delegate Methods"

- (void) rrdFileCopyDidFinish:(LC49ImportHelperTool *)sender
{
	/* Called by the helper tool to signal that the 
	 * copy of all RRDtool files has finished.
	 */
	
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	
	/* Start New */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Create Customer files";
	[helper copyL49ClientFiles];
}

- (void) setRrdFileCopyTotal:(NSNumber *)total
{
	rrdFilesTotal = [total intValue];
}

- (void) incrementRrdFileCopiedCount:(LC49ImportHelperTool *)sender
{
	rrdFilesCopied++;
	self.currentTask.progressPercent = ((float) rrdFilesCopied / (float) rrdFilesTotal) * 100.0;	
}

- (void) rrdFileCopyDidFail:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setFailed];
	
	/* End Import */
	[self importFailed];	
}

#pragma mark "Create Customer File Delegate Methods"

- (void) customerFileCreateDidFinish:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];

	/* Bring up the old database */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Finalize Lithium 5.0 Configuration";
	[helper finalizeImport];
}

- (void) customerFileCreateDidFail:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setFailed];
	
	/* End Import */
	[self importFailed];
}

#pragma mark "Finalize Import Delegate Methods"

- (void) finalizeImportDidFinish:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	
	/* Bring up the old database */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Rename /Lithium to /Lithium-4.9-Archive";
	[helper renameL49Root];	

}

#pragma mark "Rename Old Root Delegate Methods"

- (void) renameOldRootDidFinish:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	
	/* Bring up the old database */
	self.currentTask = [LC49ImportTask new];
	self.currentTask.name = @"Restart Lithium 5.0 Components";
	[helper allStart];			
}

#pragma mark "All Start Delegate Methods"

- (void) allStartDidFinish:(LC49ImportHelperTool *)sender
{
	/* Update task */
	currentTask.result = helper.status;
	[currentTask setSucceeded];
	self.currentTask = nil;

	/* Finished! */
	[self importFinished];
}

#pragma mark "Tasks"

- (NSMutableArray *) tasks
{ return tasks; }	

- (void) setTasks:(NSMutableArray *)value
{
	[tasks release];
	tasks = [value mutableCopy];
}

- (void) insertObject:(id)obj inTasksAtIndex:(unsigned int)index
{
	[tasks insertObject:obj atIndex:index]; 
}

- (void) removeObjectFromTasksAtIndex:(unsigned int)index
{
	[tasks removeObjectAtIndex:index];
}

@synthesize currentTask;
- (void) setCurrentTask:(LC49ImportTask *)task
{
	if (currentTask)
	{
		[self insertObject:currentTask inTasksAtIndex:tasks.count];
		[currentTask release];
	}
	
	currentTask = [task retain];
	[task autorelease];
}

@end
