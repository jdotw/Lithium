//
//  LCPopupView.h
//  Lithium Console
//
//  Created by James Wilson on 1/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCShadowTextField.h"

@interface LCPopupView : NSView 
{
	LCShadowTextField *textField;
	
	CGFloat triangleWidth;
	CGFloat roundedRadius;
}

+ (NSBezierPath *) pathForPopupInRect:(NSRect)rect;

@property (readonly) LCShadowTextField *textField;
@property (nonatomic, assign) CGFloat triangleWidth;
@property (nonatomic, assign) CGFloat roundedRadius;
@property (readonly) NSRect usableRect;

@end
