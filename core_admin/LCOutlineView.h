//
//  LCOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 26/03/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCOutlineView : NSOutlineView 
{
	/* outlineView binding */
	NSArrayController *outlineContentController;
	NSString *outlineContentKey;
	
	/* doubleClickTarget binding */
	id doubleClickTarget;
	NSString *doubleClickTargetKey;
	NSString *doubleClickTargetSelectorName;
	
	/* doubleClickArgument binding */
	id doubleClickArgument;
	NSString *doubleClickArgumentKey;	
}

@end
