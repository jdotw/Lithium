//
//  LCIncidentArrayController.h
//  Lithium Console
//
//  Created by James Wilson on 3/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCIncidentArrayController : NSArrayController 
{
	/* UI Element */
	IBOutlet NSTableView *tableView;	
	IBOutlet NSMenu *menuTemplate;
}

@end
