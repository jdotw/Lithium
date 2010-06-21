//
//  LCPropertiesViewerController.h
//  Lithium Console
//
//  Created by James Wilson on 12/12/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCPropertiesViewerController : NSWindowController 
{
	/* Properties */
	NSMutableArray *properties;
	
	/* UI Elements */
	IBOutlet NSObjectController *controllerAlias;
}

- (LCPropertiesViewerController *) initWithDictionary:(NSDictionary *)dictionary;
- (void) dealloc;
- (void) windowWillClose:(NSNotification *)notification;

@property (nonatomic,retain) NSMutableArray *properties;
@property (nonatomic,retain) NSObjectController *controllerAlias;
@end
