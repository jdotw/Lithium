//
//  LCSSceneWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 28/08/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCSSceneWindowController.h"
#import "LCSSceneDocument.h"

@implementation LCSSceneWindowController

#pragma mark "Constructors"

- (LCSSceneWindowController *) initWithSceneDocument:(id)initDocument
{
	/* Super-class init */
	[super initWithWindowNibName:@"StateSceneWindow"];
	[self window];
	
	/* Setup view */
	sceneDocument = initDocument;
	[sceneView setSceneDocument:initDocument];
	
	/* Setup/Show window */
	[[self window] setAcceptsMouseMovedEvents:YES];
	[[self window] makeKeyAndOrderFront:self];

	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Window Delegates"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Autorelease */
	[self autorelease];
}

#pragma mark "UI Actions"

- (IBAction) setBackgroundImageClicked:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSArray *typesArray = [NSArray arrayWithObjects:@"png",@"jpg",@"bmp",@"tiff",@"tif",nil];
	[panel beginSheetForDirectory:nil 
							 file:nil 
							types:typesArray 
				   modalForWindow:[self window] 
					modalDelegate:self 
				   didEndSelector:@selector(filePanelDidEnd:returnCode:contextInfo:) 
					  contextInfo:nil];
}

-(void)filePanelDidEnd:(NSWindow*)sheet
            returnCode:(int)returnCode
           contextInfo:(void*)contextInfo 
{
	[sceneDocument setBackgroundImage:[[NSImage alloc] initWithContentsOfFile:[(NSOpenPanel *)sheet filename]]];
}

- (IBAction) toggleForceViewsVisibleClicked:(id)sender
{
	if ([sceneView forceAllViewsVisible])
	{ [sceneView setForceAllViewsVisible:NO]; }
	else
	{ [sceneView setForceAllViewsVisible:YES]; }
}

@synthesize sceneDocument;
@synthesize sceneView;
@end
