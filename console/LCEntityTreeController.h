//
//  LCEntityTreeController.h
//  Lithium Console
//
//  Created by James Wilson on 1/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserOutlineView.h"

@interface LCEntityTreeController : NSTreeController 
{
	/* UI Elements */
	IBOutlet NSOutlineView *outlineView;
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


@property (nonatomic,retain) NSOutlineView *outlineView;
@end
