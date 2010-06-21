//
//  LCNotificationView.h
//  Lithium Console
//
//  Created by James Wilson on 5/05/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCNotificationView : NSView 
{
    NSString *note_str;
    NSTextView *text_view;
	NSString *splashText;	
}

- (void) showNotificationWindow;
- (void) fadeNotificationWindow;

@property (retain,getter=notificationText) NSString *note_str;
@property (nonatomic,retain) NSTextView *text_view;
@end
