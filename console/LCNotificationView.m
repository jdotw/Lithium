//
//  LCNotificationView.m
//  Lithium Console
//
//  Created by James Wilson on 5/05/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCNotificationView.h"

@implementation LCNotificationView

- (id)initWithFrame:(NSRect)frameRect
{
	[super initWithFrame:frameRect];
	
	[self setNotificationText: @"Lithium Console"];
	
	return self;
}

- (void) dealloc 
{
	[super dealloc];
}

//extern void *CGSReadObjectFromCString(char*);
//extern char *CGSUniqueCString(char*);
//extern void *CGSSetGStateAttribute(void*,char*,void*);
//extern void *CGSReleaseGenericObj(void*);

- (void)drawRect:(NSRect)rect
{
	const float kRoundedRadius = 25;
    NSSize size = [self bounds].size;
    CGRect pageRect;
    CGContextRef context = 
        (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	
    pageRect = CGRectMake(0, 0, rect.size.width, rect.size.height);
	
    CGContextBeginPage(context, &pageRect);
	
    //  Start with black translucent fill
    CGContextSetRGBFillColor(context, 0.2, 0.2, 0.2, 0.2);
	
    // Draw rounded rectangle.
    CGContextBeginPath(context);
    CGContextMoveToPoint(context,0,kRoundedRadius);
    CGContextAddArcToPoint(context, 0,0, kRoundedRadius,0, kRoundedRadius);
    CGContextAddLineToPoint(context, size.width - kRoundedRadius, 0);
    CGContextAddArcToPoint(context, size.width,0, 
						   size.width,kRoundedRadius, kRoundedRadius);
    CGContextAddLineToPoint(context, size.width , size.height - kRoundedRadius);
    CGContextAddArcToPoint(context, size.width,size.height, 
						   size.width - kRoundedRadius,size.height, kRoundedRadius);
    CGContextAddLineToPoint(context, kRoundedRadius,size.height);
    CGContextAddArcToPoint(context, 0,size.height, 
						   0,size.height-kRoundedRadius, kRoundedRadius);
    CGContextClosePath(context);
    CGContextFillPath(context);
	
    // Draw icon image
    NSImage *image = [NSImage imageNamed: @"LithiumConsoleLogo3"];
    NSPoint point;
    point.x = (size.width - [image size].width) / 2;
    point.y = 0.6 * size.height - [image size].height / 2;
    [image compositeToPoint: point operation: NSCompositeSourceOver];
    
    // Setup shadow magic. This shadow magic uses undocumented
    // APIs from the web. This should be checked with each release
    // of OS X to make sure we still work. Many people don't hold
    // with all this hacky stuff. OTOH this entire app is a nice
    // GUI written round a hack.
//    void *graphicsPort, *shadowValues;
//    [NSGraphicsContext saveGraphicsState];
//    NSString *shadowValuesString = [NSString stringWithFormat: 
//        @"{ Style = Shadow; Height = %d; Radius = %d; Azimuth = %d; Ka = %f; }",
//        1, 3, 90, 0.0];
//    shadowValues = CGSReadObjectFromCString((char*) [shadowValuesString UTF8String]);
//    graphicsPort = [[NSGraphicsContext currentContext] graphicsPort];
//    CGSSetGStateAttribute(graphicsPort, CGSUniqueCString("Style"), shadowValues);
    
    // Print workspace name
    int fontSize = 16;
    NSPoint textPoint;
    NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
        [NSColor whiteColor], NSForegroundColorAttributeName,
        [NSFont boldSystemFontOfSize: fontSize], NSFontAttributeName,
        nil];
    
    NSSize textSize = [note_str sizeWithAttributes: attr];
    textPoint.x = 0.5 * (size.width - textSize.width);
    textPoint.y = 0.5 * (point.y - textSize.height) + 10;
    [note_str drawAtPoint: textPoint withAttributes: attr];
	
    // Print Status
    fontSize = 10;
    textPoint;
    attr = [NSDictionary dictionaryWithObjectsAndKeys:
        [NSColor whiteColor], NSForegroundColorAttributeName,
        [NSFont boldSystemFontOfSize: fontSize], NSFontAttributeName,
        nil];
    
    textSize = [splashText sizeWithAttributes: attr];
    textPoint.x = 0.5 * (size.width - textSize.width);
    textPoint.y = 0.5 * (point.y - textSize.height) -18;
    [splashText drawAtPoint: textPoint withAttributes: attr];
	
    // Undo shadow magic
    [NSGraphicsContext restoreGraphicsState];
//    CGSReleaseGenericObj(shadowValues);
	
    CGContextEndPage(context);
	
    CGContextFlush(context);
}

- (NSString *) splashText
{ return splashText; }
- (void) setSplashText:(NSString *)text
{
	if (splashText) [splashText release];
	splashText = [text retain];
	[self setNeedsDisplay:YES];
}

- (void) setNotificationText: (NSString *) text
{
    if (note_str != nil) [note_str release];
    note_str = [[NSString stringWithString: text] retain];
    [self setNeedsDisplay: YES];
}

- (NSString *) notificationText
{
    return note_str;
}

@synthesize note_str;
@synthesize text_view;
@end
