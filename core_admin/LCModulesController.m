//
//  LCModulesController.m
//  LCAdminTools
//
//  Created by James Wilson on 15/03/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LCModulesController.h"
#import "LCModuleHelper.h"
#import "LCModule.h"

#define MAX_PATH 1024

#import <stdlib.h>
#import <string.h>
#import <sys/types.h>
#import <dirent.h>
#import <dlfcn.h>
#import <fcntl.h>
#import <unistd.h>
#import <sys/stat.h>
#import <sys/time.h>

@interface LCModulesController (Private)

- (void) installConfirmDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo;

@end


@implementation LCModulesController

- (IBAction) refreshModules:(id)sender
{
	/* Refresh the module list */
	NSMutableArray *modules = [NSMutableArray array];

	/* Open dir */
	char *dirname = "/Library/Preferences/Lithium/lithium/module_builder";
	DIR *dir = opendir (dirname);
	if (!dir)
	{ NSLog(@"refreshModules failed to open module path '%@'", dirname); return; }
	
	/* Traverse */
	struct dirent *dirent;
	struct timeval now;
	gettimeofday (&now, NULL);
	while ((dirent=readdir(dir))!=NULL)
	{
		/* Skip . and .. */
		if (strcmp(dirent->d_name, ".")==0 || strcmp(dirent->d_name, "..")==0) continue;
		
		/* Add Module */
		LCModule *module = [LCModule new];
		module.filename = [NSString stringWithUTF8String:dirent->d_name];
		[modules addObject:module];
		[module autorelease];
	}

	self.modules = modules;
}

- (LCModule *) selectedModule
{
	if ([[moduleArrayController selectedObjects] count])
	{ return [[moduleArrayController selectedObjects] objectAtIndex:0]; }
	else
	{ return nil; }
}

- (IBAction) installModuleClicked:(id)sender
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
	[openPanel setAllowedFileTypes:[NSArray arrayWithObjects:@"xml", nil]];
	[openPanel setAllowsOtherFileTypes:NO];
	[openPanel beginSheetForDirectory:nil
								 file:nil
					   modalForWindow:windowForSheet
						modalDelegate:self 
					   didEndSelector:@selector(installClicked:returnCode:contextInfo:)
						  contextInfo:nil];
}

- (IBAction) installClicked:(NSSavePanel *)sheet returnCode:(int)returnCode  contextInfo:(void  *)contextInfo
{
	if (returnCode != NSOKButton) return;
	
	/* Check file type */
	if (![[[[sheet filename] lastPathComponent] pathExtension] isEqualToString:@"xml"])
	{ 
		NSAlert *alert;
		if ([[[[sheet filename] lastPathComponent] pathExtension] isEqualToString:@"lcmb"])
		{
			/* ModB Document Specified */
			alert = [NSAlert alertWithMessageText:@"Incorrect File Type"
									defaultButton:@"Cancel"
								  alternateButton:nil
									  otherButton:nil
						informativeTextWithFormat:@"The specified file is a ModuleBuilder Document file, not the actual Module itself. Open this file in Module Builder and Export the file to XML before uploading to Lithium Core"];
		}
		else 
		{
			/* Generic Warning */
			alert = [NSAlert alertWithMessageText:@"Unknown File Type"
									defaultButton:@"Cancel"
								  alternateButton:nil
									  otherButton:@"Install Anyway"
						informativeTextWithFormat:@"The specified file does not have a '.xml' extension. Custom Modules should be exported from Module Builder and saved with a '.xml' extension for installation in to Lithium Core"];
		}
		
		[sheet orderOut:self];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:windowForSheet
						  modalDelegate:self
						 didEndSelector:@selector(installConfirmDidEnd:returnCode:contextInfo:)
							contextInfo:[[sheet filename] copy]];
	}
	else 
	{
		/* File is OK, go straigh to install */
		[self installConfirmDidEnd:nil returnCode:NSAlertOtherReturn contextInfo:[[sheet filename] copy]];
	}
}
		 
- (void) installConfirmDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo;
{	
	NSString *filename = (NSString *) contextInfo;
	if (returnCode == NSAlertOtherReturn)
	{
		LCModuleHelper *helper = [LCModuleHelper new];
		helper.delegate = self;
		[helper installModule:filename filename:[filename lastPathComponent]];
	}
	[filename release];
}

- (void) moduleInstallDidFinish:(LCModuleHelper*)helper
{
	if (!helper.successful)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Error installing Module"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:helper.status];
		[alert beginSheetModalForWindow:windowForSheet
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
	}
	[self refreshModules:nil];
}

- (IBAction) deleteModuleClicked:(id)sender
{
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm Module Delete"
									 defaultButton:@"Delete"
								   alternateButton:@"Cancel"
									   otherButton:nil
						 informativeTextWithFormat:[NSString stringWithFormat:@"The module '%@' will be delete. This can not be undone.", [[self selectedModule] filename]]];
	[alert beginSheetModalForWindow:windowForSheet
					  modalDelegate:self
					 didEndSelector:@selector(deleteConfirmDidEnd:returnCode:contextInfo:)
						contextInfo:[[[self selectedModule] filename] copy]];	
}

- (void) deleteConfirmDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	NSString *filename = (NSString *) contextInfo;
	if (returnCode == NSAlertDefaultReturn)
	{
		LCModuleHelper *helper = [LCModuleHelper new];
		helper.delegate = self;
		[helper deleteModule:filename];
	}
	[filename release];
}

- (void) moduleDeleteDidFinish:(LCModuleHelper*)helper
{
	if (!helper.successful)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Error deleting Module"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:helper.status];
		[alert beginSheetModalForWindow:windowForSheet
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
	}	
	[self refreshModules:nil];
}

@synthesize modules=_modules;

@end
