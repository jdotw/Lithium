//
//  LCBWRepItemTreeController.h
//  Lithium Console
//
//  Created by James Wilson on 26/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCOutlineView.h"
#import "LCBWRepDocument.h"

@interface LCBWRepItemTreeController : NSTreeController 
{
	IBOutlet LCOutlineView *outlineView;
	IBOutlet NSMenu *groupMenu;
	IBOutlet NSMenu *deviceMenu;
	IBOutlet NSMenu *ifaceMenu;
	IBOutlet NSMenuItem *arrangeByDeviceMenuItem;
	IBOutlet NSTableColumn *descColumn;

	LCBWRepDocument *reportDocument;
}

#pragma mark "NIB Methods"
- (void)awakeFromNib;

#pragma mark "Drag and Drop Methods"
- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 acceptDrop:(id <NSDraggingInfo>)info 
			   item:(id)item 
		 childIndex:(int)index;
- (NSDragOperation)outlineView:(NSOutlineView *)outlineView 
				  validateDrop:(id <NSDraggingInfo>)info 
				  proposedItem:(id)item 
			proposedChildIndex:(int)index;
- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 writeItems:(NSArray *)items 
	   toPasteboard:(NSPasteboard *)pboard;

#pragma mark Menu Items
- (NSMenu *) menuForEvent:(NSEvent *)event;

#pragma mark Accessors
- (LCBWRepDocument *) reportDocument;
- (void) setReportDocument:(LCBWRepDocument *)value;

@property (nonatomic,retain) LCOutlineView *outlineView;
@property (nonatomic,retain) NSMenu *groupMenu;
@property (nonatomic,retain) NSMenu *deviceMenu;
@property (nonatomic,retain) NSMenu *ifaceMenu;
@property (nonatomic,retain) NSMenuItem *arrangeByDeviceMenuItem;
@property (nonatomic,retain) NSTableColumn *descColumn;
@property (retain,getter=reportDocument,setter=setReportDocument:) LCBWRepDocument *reportDocument;
@end
