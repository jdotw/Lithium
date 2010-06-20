//
//  LCVRackCablePopupView.m
//  Lithium Console
//
//  Created by James Wilson on 3/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCVRackCablePopupView.h"


@implementation LCVRackCablePopupView

- (id)initWithFrame:(NSRect)frame 
{
    self = [super initWithFrame:frame];
    if (self) 
	{
		/* Create text field */
		float fontSize = 10.0;
		float topRowYOffset = 8.0;
		float bottomRowYOffset = 16.0;
		aEndDevLabel = [[LCShadowTextField alloc] initWithFrame:NSMakeRect(NSMinX(self.usableRect),
																		   roundf(NSMaxY(self.usableRect) - (1 * (fontSize + 5)) - topRowYOffset),
																		   NSWidth(self.usableRect),
																		   fontSize + 2.0)];
		[aEndDevLabel setAutoresizingMask:NSViewHeightSizable|NSViewWidthSizable|NSViewMinXMargin|NSViewMaxXMargin|NSViewMinYMargin|NSViewMaxYMargin];
		[aEndDevLabel setFont:[NSFont boldSystemFontOfSize:fontSize]];
		[aEndDevLabel setBackgroundColor:[NSColor clearColor]];
		[aEndDevLabel setTextColor:[NSColor whiteColor]];
		[aEndDevLabel setBordered:NO];
		[aEndDevLabel setAlignment:NSCenterTextAlignment];
		[self addSubview:aEndDevLabel];

		aEndIntLabel = [[LCShadowTextField alloc] initWithFrame:NSMakeRect(NSMinX(self.usableRect),
																		   roundf(NSMaxY(self.usableRect) - (2 * (fontSize + 5)) - topRowYOffset),
																		   NSWidth(self.usableRect),
																		   fontSize + 2.0)];
		[aEndIntLabel setAutoresizingMask:NSViewHeightSizable|NSViewWidthSizable|NSViewMinXMargin|NSViewMaxXMargin|NSViewMinYMargin|NSViewMaxYMargin];
		[aEndIntLabel setFont:[NSFont systemFontOfSize:fontSize]];
		[aEndIntLabel setBackgroundColor:[NSColor clearColor]];
		[aEndIntLabel setTextColor:[NSColor whiteColor]];
		[aEndIntLabel setBordered:NO];
		[aEndIntLabel setAlignment:NSCenterTextAlignment];
		[aEndIntLabel setStringValue:@"aEndInt"];
		[self addSubview:aEndIntLabel];		
		
		bEndDevLabel = [[LCShadowTextField alloc] initWithFrame:NSMakeRect(NSMinX(self.usableRect),
																		   roundf(NSMaxY(self.usableRect) - (3 * (fontSize + 5)) - bottomRowYOffset),
																		   NSWidth(self.usableRect),
																		   fontSize + 2.0)];
		[bEndDevLabel setAutoresizingMask:NSViewHeightSizable|NSViewWidthSizable|NSViewMinXMargin|NSViewMaxXMargin|NSViewMinYMargin|NSViewMaxYMargin];
		[bEndDevLabel setFont:[NSFont boldSystemFontOfSize:fontSize]];
		[bEndDevLabel setBackgroundColor:[NSColor clearColor]];
		[bEndDevLabel setTextColor:[NSColor whiteColor]];
		[bEndDevLabel setBordered:NO];
		[bEndDevLabel setAlignment:NSCenterTextAlignment];
		[bEndDevLabel setStringValue:@"bEndDev"];
		[self addSubview:bEndDevLabel];
		
		bEndIntLabel = [[LCShadowTextField alloc] initWithFrame:NSMakeRect(NSMinX(self.usableRect),
																		   roundf(NSMaxY(self.usableRect) - (4 * (fontSize + 5)) - bottomRowYOffset),
																		   NSWidth(self.usableRect),
																		   fontSize + 2.0)];
		[bEndIntLabel setAutoresizingMask:NSViewHeightSizable|NSViewWidthSizable|NSViewMinXMargin|NSViewMaxXMargin|NSViewMinYMargin|NSViewMaxYMargin];
		[bEndIntLabel setFont:[NSFont systemFontOfSize:fontSize]];
		[bEndIntLabel setBackgroundColor:[NSColor clearColor]];
		[bEndIntLabel setTextColor:[NSColor whiteColor]];
		[bEndIntLabel setBordered:NO];
		[bEndIntLabel setAlignment:NSCenterTextAlignment];
		[bEndIntLabel setStringValue:@"bEndInt"];
		[self addSubview:bEndIntLabel];		
		
    }
    return self;
}

@synthesize aEndDevLabel;
@synthesize aEndIntLabel;
@synthesize bEndDevLabel;
@synthesize bEndIntLabel;

@end
