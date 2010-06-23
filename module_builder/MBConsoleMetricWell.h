//
//  MBConsoleMetricWell.h
//  ModuleBuilder
//
//  Created by James Wilson on 17/08/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBMetric.h"
#import "MBContainer.h"

@interface MBConsoleMetricWell : NSView 
{
	NSString *metricDesc;
	BOOL drawHighlight;
	
	/* UI Elements */
	IBOutlet NSTextField *label;
	IBOutlet NSImageView *imageView;
	IBOutlet NSImageView *dotView;
}

#pragma mark "Drawing"
- (void) drawRect:(NSRect)rect;

#pragma mark "Layout Methods"
- (void) resetUsing:(MBContainer *)newContainer forPosition:(int)newIndex;

#pragma mark "Metric Accessors"
- (NSString *) metricDesc;
- (void) setMetricDesc:(NSString *)value;

#pragma mark "Drag and Drop"
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)info ;
- (void)draggingExited:(id <NSDraggingInfo>)sender;
- (BOOL)performDragOperation:(id <NSDraggingInfo>)info;

#pragma mark "UI Accessors"
- (IBAction) clearMetricClicked:(id)sender;

#pragma mark "General Accessors"
- (BOOL) drawHighlight;
- (void) setDrawHightlight:(BOOL)value;

@end
