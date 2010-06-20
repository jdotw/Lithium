//
//  LCCaseArrayController.h
//  Lithium Console
//
//  Created by James Wilson on 7/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCCaseArrayController : NSArrayController 
{
	/* UI Element */
	IBOutlet NSTableView *tableView;
	
	/* Predicate */
	NSPredicate *preDragPredicate;
	
	/* Config */
	BOOL allowDrop;	
}

#pragma mark "Accessor Methods"
@property (assign) BOOL allowDrop;

@end
