//
//  LCBrowserSceneContentController.m
//  Lithium Console
//
//  Created by James Wilson on 17/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserSceneContentController.h"
#import "LCDocumentBreadcrumItem.h"
#import "LCSSceneOverlayView.h"

@implementation LCBrowserSceneContentController

#pragma mark "Constructors"

- (id) initWithDocument:(LCSSceneDocument *)initDocument inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"SceneContent" document:initDocument inBrowser:initBrowser];
	
	[sceneView setSceneDocument:initDocument];
	[sceneView setController:self];
	
	
	/* Setup Breadcrum View */
	LCDocumentBreadcrumItem *item = [LCDocumentBreadcrumItem new];
	item.title = @"State Scenes";
	[crumView insertObject:item inItemsAtIndex:0];
	[item autorelease];
	item = [LCDocumentBreadcrumItem new];
	item.title = self.document.desc;
	item.document = self.document;
	[crumView insertObject:item inItemsAtIndex:1];
	[item autorelease];
	
	/* Bind inspector */
	[inspectorController bind:@"target"
					 toObject:self
				  withKeyPath:@"selectedEntity"
					  options:nil];
	
	return self;
}

- (void) dealloc
{
	[inspectorController unbind:@"target"];
	[super dealloc];
}

#pragma mark "Breadcrum Methods"

- (IBAction) breadcrumClicked:(id)sender
{
	NSButton *button = sender;
	LCDocumentBreadcrumItem *crumItem = [[button cell] representedObject];
	if (!crumItem.document)
	{
		/* Select root */
		[browser.browserTreeOutlineView selectScenesRoot];
	}
}

#pragma mark "UI Actions"

- (IBAction) setBackgroundImageClicked:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSArray *typesArray = [NSArray arrayWithObjects:@"png",@"jpg",@"bmp",@"tiff",@"tif",nil];
	[panel beginSheetForDirectory:nil 
							 file:nil 
							types:typesArray 
				   modalForWindow:[[self view] window] 
					modalDelegate:self 
				   didEndSelector:@selector(filePanelDidEnd:returnCode:contextInfo:) 
					  contextInfo:nil];
}

-(void)filePanelDidEnd:(NSWindow*)sheet
            returnCode:(int)returnCode
           contextInfo:(void*)contextInfo 
{
	LCSSceneDocument *sceneDocument = (LCSSceneDocument *) self.document;
	[sceneDocument setBackgroundImage:[[[NSImage alloc] initWithContentsOfFile:[(NSOpenPanel *)sheet filename]] autorelease]];
}

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	SEL action = [item action];
	
	/* 
	 * Monitored Entity
	 */
	
	if (action == @selector(setBackgroundImageClicked:)) 
	{ if (document.editing) return YES; }

	if (action == @selector(toggleForceViewsVisibleClicked:)) 
	{ return YES; }
	
	return [super validateUserInterfaceItem:item];
}	

#pragma mark "Selection"

@synthesize selectedEntity;

@synthesize selectedOverlay;
- (void) setSelectedOverlay:(LCSSceneOverlay *)value
{
	/* Clear old selection */
	LCSSceneOverlayView *overlayView = [sceneView.overlayViewDict objectForKey:selectedOverlay.uuid];
	overlayView.selected = NO;
	[overlayView setNeedsDisplay:YES];
	[selectedOverlay release];
	
	/* Set new selection */
	selectedOverlay = [value retain];
	self.selectedEntity = selectedOverlay.entity;
	overlayView = [sceneView.overlayViewDict objectForKey:selectedOverlay.uuid];
	overlayView.selected = YES;
	[overlayView setNeedsDisplay:YES];
}

@end
