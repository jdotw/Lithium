//
//  LCBrowserContView.h
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCBrowserContView : NSView 
{
	NSViewAnimation *fadeInAnim;
	NSViewAnimation *fadeOutAnim;
	BOOL initialFadeOutDone;
}

@property (retain) NSViewAnimation *fadeInAnim;
@property (retain) NSViewAnimation *fadeOutAnim;
@property BOOL initialFadeOutDone;
@end
