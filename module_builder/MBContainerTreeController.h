//
//  MBContainerTreeController.h
//  ModuleBuilder
//
//  Created by James Wilson on 14/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "ModuleDocument.h"
#import "MBContOutlineView.h"

@interface MBContainerTreeController : NSTreeController 
{
	IBOutlet MBContOutlineView *outlineView;
	ModuleDocument *document;
}

#pragma mark "NIB Methods"
- (void)awakeFromNib;

#pragma mark "Drag and Drop Methods"
- (NSDragOperation)outlineView:(NSOutlineView *)ov 
				  validateDrop:(id <NSDraggingInfo>)info 
				  proposedItem:(id)item 
			proposedChildIndex:(int)index;
- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 acceptDrop:(id <NSDraggingInfo>)info 
			   item:(id)item 
		 childIndex:(int)index;
- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 writeItems:(NSArray *)items 
	   toPasteboard:(NSPasteboard *)pboard;

#pragma mark NSOutlineView Hacks for Pretending to be a data source
- (BOOL) outlineView: (NSOutlineView *)ov
	isItemExpandable: (id)item;
- (int)  outlineView: (NSOutlineView *)ov
numberOfChildrenOfItem:(id)item;
- (id)   outlineView: (NSOutlineView *)ov
			   child:(int)index
			  ofItem:(id)item;
- (id)   outlineView: (NSOutlineView *)ov
objectValueForTableColumn:(NSTableColumn*)col
			  byItem:(id)item;
- (void)outlineView:(NSOutlineView *)outlineView 
	willDisplayCell:(id)cell 
	 forTableColumn:(NSTableColumn *)tableColumn 
			   item:(id)item;

#pragma mark "Accessors"
- (ModuleDocument *) document;
- (void) setDocument:(ModuleDocument *)newDocument;


@end
