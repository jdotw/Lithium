//
//  MBOutlineViewController.h
//  ModuleBuilder
//
//  Created by James Wilson on 8/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MBOutlineViewController : NSViewController 
{
	/* Additional Views */
	IBOutlet NSView *prefsView;
	IBOutlet NSView *customView;
}

#pragma mark "Row height"
- (float) rowHeight;

#pragma mark "UI Actions"
- (IBAction) showInfoClicked:(id)sender;

#pragma mark "Properties"
@property (assign) NSView *prefsView;
@property (assign) NSView *customView;

@end
