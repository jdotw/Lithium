//
//  MBOidArrayController.h
//  ModuleBuilder
//
//  Created by James Wilson on 14/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MBOidArrayController : NSArrayController
{
	/* UI Element */
	IBOutlet NSTableView *tableView;
}

#pragma mark "NIB Methods"
- (void)awakeFromNib;

#pragma mark "Drag and Drop Methods"
- (NSDragOperation)tableView:(NSTableView*)tv validateDrop:(id <NSDraggingInfo>)info proposedRow:(int)row proposedDropOperation:(NSTableViewDropOperation)op;
- (BOOL)tableView:(NSTableView*)tv acceptDrop:(id <NSDraggingInfo>)info row:(int)row dropOperation:(NSTableViewDropOperation)op;

@end
