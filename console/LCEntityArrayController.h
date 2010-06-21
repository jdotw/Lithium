//
//  LCEntityArrayController.h
//  Lithium Console
//
//  Created by James Wilson on 3/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCEntityArrayController : NSArrayController
{
	/* UI Element */
	IBOutlet NSTableView *tableView;
	
	/* Config */
	BOOL allowDrop;
}

#pragma mark "NIB Methods"
- (void)awakeFromNib;

#pragma mark "Drag and Drop Methods"
- (NSDragOperation)tableView:(NSTableView*)tv validateDrop:(id <NSDraggingInfo>)info proposedRow:(int)row proposedDropOperation:(NSTableViewDropOperation)op;
- (BOOL)tableView:(NSTableView*)tv acceptDrop:(id <NSDraggingInfo>)info row:(int)row dropOperation:(NSTableViewDropOperation)op;

#pragma mark "Accessor Methods"
- (BOOL) allowDrop;
- (void) setAllowDrop:(BOOL)flag;

@property (nonatomic,retain) NSTableView *tableView;
@property (getter=allowDrop,setter=setAllowDrop:) BOOL allowDrop;
@end
