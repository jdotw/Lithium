//
//  NSInspectorPanel.m
//  Lithium Console
//
//  Created by James Wilson on 12/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspectorPanel.h"


@implementation LCInspectorPanel

- (void) awakeFromNib
{
	[self setFloatingPanel:NO];
}

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag 
{
    NSWindow* result = [super initWithContentRect:contentRect styleMask:aStyle backing:NSBackingStoreBuffered defer:NO];
    [result setBackgroundColor: [NSColor clearColor]];
    [result setLevel: NSStatusWindowLevel];
    [result setAlphaValue:1.0];
    [result setOpaque:NO];
    [result setHasShadow: YES];
    return result;
}


@end
