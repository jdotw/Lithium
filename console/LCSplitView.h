//
//  LCSplitView.h
//  Lithium Console
//
//  Created by James Wilson on 4/03/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCSplitView : NSSplitView
{

}

#pragma mark Frame Sizes
- (NSArray *) frameStrings;
- (void) setFrameStrings:(NSArray *)strings;

@end
