//
//  LCXSDeviceView.m
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXSDeviceView.h"
#import "LCXSDriveView.h"
#import "LCXSBlowerView.h"
#import "LCXSCPUView.h"
#import "LCXSRAMStickView.h"
#import "LCXSPowerView.h"
#import "LCObject.h"

@implementation LCXSDeviceView

#pragma mark "Constructors"

- (LCXSDeviceView *) initWithDevice:(LCEntity *)initDevice inFrame:(NSRect)frame
{
	/* Super-class init */
	[super initWithFrame:frame];
	
	/* Set device */
	device = [initDevice retain];
	
	/* Load images */
	xsBaseImage = [[NSImage imageNamed:@"xsbase.png"] retain];
	
	/* Create arrays */
	driveViews = [[NSMutableArray array] retain];
	cpuViews = [[NSMutableArray array] retain];
	ramStickViews = [[NSMutableArray array] retain];
	blowerViews = [[NSMutableArray array] retain];
	powerViews = [[NSMutableArray array] retain];
	
	/* Check for initial lid hiding */
	if ([device initialRefreshPerformed] && ![device refreshInProgress])
	{ 
		/* Device has been refreshed and is not refrsshing */
		hideLid = YES; 
		hidePowerVisor = YES; 
		if ([[[device valueForKeyPath:@"childrenDictionary.xsichassis.childrenDictionary.master"] rawValueForMetricNamed:@"systempower"] isEqualToString:@"0"])
		{
			/* Show power visor -- power is off */
			hidePowerVisor = NO;
		}		
	}
	
	/* Reset views */
	[self resetLidView];
	[self resetPowerVisorView];
	[self resetDriveCageView];
	[self resetDriveViews];
	[self resetBlowerViews];
	[self resetCPUViews];
	[self resetRamStickViews];
	[self resetPowerViews];
	
	/* Add observers */
	[device addObserver:self 
			 forKeyPath:@"refreshInProgress" 
				options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				context:NULL];	
		
	return self;
}

- (void) dealloc
{
	[xsBaseImage release];
	[driveViews release];
	[cpuViews release];
	[ramStickViews release];
	[blowerViews release];
	[powerViews release];
	[super dealloc];
}

#pragma mark "Geometry"

- (float) xsWidth
{ return 431.0; }

- (float) xsHeight
{ return 267.0; }

- (NSRect) xsRect
{ 
	float servicesMargin = 132.0;
	float x = (([self bounds].size.width - [self xsWidth]) / 2);
	if (servicesMargin > (([self bounds].size.width - [self xsWidth]) / 2))
	{ x += servicesMargin - (([self bounds].size.width - [self xsWidth]) / 2); }
	float y = (([self bounds].size.height - [self xsHeight]) / 2);
	return NSMakeRect(x,y,[self xsWidth],[self xsHeight]); 
}

- (NSRect) driveBay1Rect
{
	float x = NSMinX([self xsRect]) + 14;
	float y = NSMinY([self xsRect]) + 177;
	return NSMakeRect(x, y, 122, 75);
}

- (NSRect) driveBay2Rect
{
	float x = NSMinX([self xsRect]) + 14;
	float y = NSMinY([self xsRect]) + 95;
	return NSMakeRect(x, y, 122, 75);
}

- (NSRect) driveBay3Rect
{
	float x = NSMinX([self xsRect]) + 14;
	float y = NSMinY([self xsRect]) + 15;
	return NSMakeRect(x, y, 122, 75);
}

- (NSRect) cpu1Rect
{
	return NSMakeRect(NSMinX([self xsRect]) + 179, NSMinY([self xsRect]) + 18, 84, 106);
}

- (NSRect) cpu2Rect
{
	return NSMakeRect(NSMinX([self xsRect]) + 179, NSMinY([self xsRect]) + 142, 84, 106);	
}

- (NSRect) ramStickRect:(int)index
{
	return NSMakeRect(NSMinX([self xsRect]) + 289, NSMinY([self xsRect]) + 17 + (index * 11), 113, 8);	
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
	if ([self drawWarnings]) return;
	
	/*
	 * Services 
	 */
	
	/* Get services */
	CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
//	CGContextSaveGState(context); 
//	NSArray *services = [device valueForKeyPath:@"childrenDictionary.xservices.children"];
//	NSEnumerator *serviceEnum = [services objectEnumerator];
//	LCEntity *service;
//	float x = 10;
//	float y = NSMaxY([self bounds])-20;
//	while (0)
//		while (service=[serviceEnum nextObject])
//	{
//		/* Check state of service */
//		if ([[service opstateInteger] intValue] != 0 || [[service valueForMetricNamed:@"state"] isEqualToString:@"RUNNING"])
//		{
//			/* 
//			 * Non-Normal or RUNNING, show it 
//			 */
//			
//			/* Shadow */
//			NSShadow *textShadow = [[NSShadow alloc] init]; 
//			[textShadow setShadowOffset:NSMakeSize(3.0, -3.0)]; 
//			[textShadow setShadowBlurRadius:3];
//			[textShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.5]];
//			[textShadow set];
//
//			/* Status Dot */
//			NSRect statusRect = NSMakeRect(x, y, 5, 5);
//			NSBezierPath *statusPath = [NSBezierPath bezierPathWithOvalInRect:statusRect];
//			[[service opStateColor] setFill];
//			[statusPath fill];
//			[[NSColor grayColor] setStroke];
//			[statusPath stroke];			
//			
//			/* Desc */
//			NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
//				[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0], NSForegroundColorAttributeName,
//				[NSFont labelFontOfSize:11.0], NSFontAttributeName,
//				nil];			
//			NSString *description = [service desc];
//			[description drawAtPoint:NSMakePoint(x + 10, y - 3) withAttributes:attr];				
//			[textShadow release];
//			
//			/* Increment x/y */
//			y -= 15;
//		}			
//	}	
//	CGContextRestoreGState(context);	
	
	/* Setup the shadow */
	CGContextSaveGState(context); 
	NSShadow *theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(5.0, -5.0)]; 
	[theShadow setShadowBlurRadius:7];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.8]];
	[theShadow set];	
	
	/* Draw base */
	[xsBaseImage drawInRect:[self xsRect]
				   fromRect:NSMakeRect(0, 0, [xsBaseImage size].width, [xsBaseImage size].height) 
				  operation:NSCompositeSourceOver 
				   fraction:1.0];
	
	[theShadow release];
	CGContextRestoreGState(context);	
	
	/* Cable cables */
	
	/* Setup the cable shadow */
	theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(5.0, -5.0)]; 
	[theShadow setShadowBlurRadius:7];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.8]];
	[theShadow set];		
	
	/*
	 * Ethernet Port 0 
	 */

	LCObject *interface = nil;
	if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_intel"])
	{ interface = [device valueForKeyPath:@"childrenDictionary.xsinetwork.childrenDictionary.en0"]; }
	else if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_ppc"])
	{ interface = [device valueForKeyPath:@"childrenDictionary.xsnetwork.childrenDictionary.en0"]; }
	if (interface && [[interface valueForMetricNamed:@"linkstate"] isEqualToString:@"active"])
	{
		/* Draw plug */
		NSImage *plug = [NSImage imageNamed:@"xsethplug.png"];
		[plug drawAtPoint:NSMakePoint(NSMaxX([self xsRect]),NSMinY([self xsRect])+93) 
				 fromRect:NSMakeRect(0, 0, [plug size].width, [plug size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];	
		
		/* Draw Cable */
		NSBezierPath *cablePath = [NSBezierPath bezierPath];
		[cablePath moveToPoint:NSMakePoint(NSMaxX([self xsRect])+[plug size].width,NSMinY([self xsRect])+100)];
		[cablePath curveToPoint:NSMakePoint(NSMaxX([self bounds]),NSMinY([self xsRect])+70)
				  controlPoint1:NSMakePoint(NSMaxX([self bounds])-20,NSMinY([self xsRect])+100)
				  controlPoint2:NSMakePoint(NSMaxX([self bounds])-20,NSMinY([self xsRect])+100)];
		[cablePath setLineWidth:6.0];
		[[NSColor blackColor] setStroke];
		[cablePath stroke];
		[cablePath setLineWidth:4.0];
		[[NSColor blueColor] setStroke];
		[cablePath stroke];

		/* Desc */
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:0.98 green:0.98 blue:0.98 alpha:1.0], NSForegroundColorAttributeName,
			[NSFont fontWithName:@"Lucida Grande" size:9.0], NSFontAttributeName,
			nil];
		NSString *str = [interface desc];
		[str drawAtPoint:NSMakePoint(NSMaxX([self xsRect])+5,NSMinY([self xsRect])+110) withAttributes:attr];		
		str = [interface valueForMetricNamed:@"linkstate"];
		[str drawAtPoint:NSMakePoint(NSMaxX([self xsRect])+5,NSMinY([self xsRect])+120) withAttributes:attr];		
	}
	
	/*
	 * Ethernet Port 1
	 */
	
	interface = nil;
	if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_intel"])
	{ interface = [device valueForKeyPath:@"childrenDictionary.xsinetwork.childrenDictionary.en1"]; }
	else if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_ppc"])
	{ interface = [device valueForKeyPath:@"childrenDictionary.xsnetwork.childrenDictionary.en1"]; }
	if (interface && [[interface valueForMetricNamed:@"linkstate"] isEqualToString:@"active"])
	{
		/* Draw plug */
		NSImage *plug = [NSImage imageNamed:@"xsethplug.png"];
		[plug drawAtPoint:NSMakePoint(NSMaxX([self xsRect]),NSMinY([self xsRect])+73) 
				 fromRect:NSMakeRect(0, 0, [plug size].width, [plug size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];		
		
		/* Draw Cable */
		NSBezierPath *cablePath = [NSBezierPath bezierPath];
		[cablePath moveToPoint:NSMakePoint(NSMaxX([self xsRect])+[plug size].width,NSMinY([self xsRect])+80)];
		[cablePath curveToPoint:NSMakePoint(NSMaxX([self bounds]),NSMinY([self xsRect])+50)
				  controlPoint1:NSMakePoint(NSMaxX([self bounds])-20,NSMinY([self xsRect])+80)
				  controlPoint2:NSMakePoint(NSMaxX([self bounds])-20,NSMinY([self xsRect])+80)];
		[cablePath setLineWidth:6.0];
		[[NSColor blackColor] setStroke];
		[cablePath stroke];
		[cablePath setLineWidth:4.0];
		[[NSColor blueColor] setStroke];
		[cablePath stroke];	
		
		/* Desc */
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:1.0], NSForegroundColorAttributeName,
			[NSFont fontWithName:@"Lucida Grande" size:9.0], NSFontAttributeName,
			nil];	
		NSString *str = [interface desc];
		[str drawAtPoint:NSMakePoint(NSMaxX([self xsRect])+5,NSMinY([self xsRect])+58) withAttributes:attr];
		str = [interface valueForMetricNamed:@"linkstate"];
		[str drawAtPoint:NSMakePoint(NSMaxX([self xsRect])+5,NSMinY([self xsRect])+48) withAttributes:attr];		
	}
	
	/*
	 * Power cable 
	 */
	
	if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_intel"])
	{
		/* Draw Cable 1 */
		NSBezierPath *cablePath = [NSBezierPath bezierPath];
		[cablePath moveToPoint:NSMakePoint(NSMaxX([self xsRect]),NSMaxY([self xsRect])-25)];
		[cablePath curveToPoint:NSMakePoint(NSMaxX([self bounds]),NSMaxY([self xsRect])-55)
				  controlPoint1:NSMakePoint(NSMaxX([self bounds])-20,NSMaxY([self xsRect])-25)
				  controlPoint2:NSMakePoint(NSMaxX([self bounds])-20,NSMaxY([self xsRect])-25)];
		[cablePath setLineWidth:10.0];
		[[NSColor darkGrayColor] setStroke];
		[cablePath stroke];
		[cablePath setLineWidth:8.0];
		[[NSColor blackColor] setStroke];
		[cablePath stroke];		
		
		/* Draw Cable 2 */
		cablePath = [NSBezierPath bezierPath];
		[cablePath moveToPoint:NSMakePoint(NSMaxX([self xsRect]),NSMaxY([self xsRect])-45)];
		[cablePath curveToPoint:NSMakePoint(NSMaxX([self bounds]),NSMaxY([self xsRect])-75)
				  controlPoint1:NSMakePoint(NSMaxX([self bounds])-20,NSMaxY([self xsRect])-45)
				  controlPoint2:NSMakePoint(NSMaxX([self bounds])-20,NSMaxY([self xsRect])-45)];
		[cablePath setLineWidth:10.0];
		[[NSColor darkGrayColor] setStroke];
		[cablePath stroke];
		[cablePath setLineWidth:8.0];
		[[NSColor blackColor] setStroke];
		[cablePath stroke];		
		
	}
	else if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_ppc"])
	{
		/* Draw Cable */
		NSBezierPath *cablePath = [NSBezierPath bezierPath];
		[cablePath moveToPoint:NSMakePoint(NSMaxX([self xsRect]),NSMaxY([self xsRect])-25)];
		[cablePath curveToPoint:NSMakePoint(NSMaxX([self bounds]),NSMaxY([self xsRect])-55)
				  controlPoint1:NSMakePoint(NSMaxX([self bounds])-20,NSMaxY([self xsRect])-25)
				  controlPoint2:NSMakePoint(NSMaxX([self bounds])-20,NSMaxY([self xsRect])-25)];
		[cablePath setLineWidth:10.0];
		[[NSColor darkGrayColor] setStroke];
		[cablePath stroke];
		[cablePath setLineWidth:8.0];
		[[NSColor blackColor] setStroke];
		[cablePath stroke];		
	}
	
	[theShadow release];
	
	[super drawRect:rect];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"refreshInProgress"])
	{
		if (![device refreshInProgress])
		{
			if (![lidView isHidden]) 
			{
				hideLid = YES;
				[lidView fadeOut]; 
			}
			if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_intel"])
			{
				if (hidePowerVisor && [[device valueForKeyPath:@"childrenDictionary.xsichassis.childrenDictionary.master"] rawValueForMetricNamed:@"systempower"] &&
					[[[device valueForKeyPath:@"childrenDictionary.xsichassis.childrenDictionary.master"] rawValueForMetricNamed:@"systempower"] isEqualToString:@"0"])
				{
					/* Show power visor -- power is off */
					[powerVisorView fadeIn];
					hidePowerVisor = NO;
				}
				else if (!hidePowerVisor && [[[device valueForKeyPath:@"childrenDictionary.xsichassis.childrenDictionary.master"] rawValueForMetricNamed:@"systempower"] isEqualToString:@"1"])
				{
					/* Show power visor -- power is on */
					[powerVisorView fadeOut];
					hidePowerVisor = YES;
				}
				else if (![[device valueForKeyPath:@"childrenDictionary.xsichassis.childrenDictionary.master"] rawValueForMetricNamed:@"systempower"])
				{
					/* Power state not known */
					[powerVisorView fadeOut];
					hidePowerVisor = YES;
				}				
			}
			[self setNeedsDisplay:YES];
			[self resetDriveCageView];
			[self resetDriveViews];
			[self resetBlowerViews];
			[self resetCPUViews];
			[self resetRamStickViews];
			[self resetPowerViews];
		}
	}
}

#pragma mark "Resizing"

- (void)setFrameSize:(NSSize)newSize
{
	NSSize oldSize = [self frame].size;
	[super setFrameSize:newSize];
	if (![NSStringFromSize(newSize) isEqualToString:NSStringFromSize(oldSize)])
	{
		[self resetLidView];
		[self resetPowerVisorView];
		[self resetDriveCageView];
		[self resetDriveViews];
		[self resetBlowerViews];
		[self resetCPUViews];
		[self resetRamStickViews];
		[self resetPowerViews];
		[self setNeedsDisplay:YES];
	}
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	/* Remove observer */
	[device removeObserver:self forKeyPath:@"refreshInProgress"];
	
	[super removeFromSuperview];
}

- (void) resetDriveViews
{
	/* Clear old views */
	LCXSDriveView *driveView;
	for (driveView in driveViews)
	{
		[driveView removeFromSuperview];
		[driveView autorelease];
		driveView = nil;
	}
	[driveViews removeAllObjects];
	
	/* Check for warnings */
	if ([self hasWarnings]) return; 
	
	/* Create views */
	NSArray *driveEntities = [device valueForKeyPath:@"childrenDictionary.xsdrives.children"];
	LCObject *driveObject;
	for (driveObject in driveEntities)
	{
		/* Create drive rect */
		int slot;
		NSRect driveRect = NSZeroRect;
		if ([[driveObject desc] hasSuffix:@"(Bay 1)"])
		{
			/* Drive bay 1*/
			slot = 1;
			driveRect = [self driveBay1Rect];
		}
		else if ([[driveObject desc] hasSuffix:@"(Bay 2)"])
		{
			/* Drive bay 1*/
			slot = 2;
			driveRect = [self driveBay2Rect];
		}
		else if ([[driveObject desc] hasSuffix:@"(Bay 3)"])
		{
			/* Drive bay 3 */
			slot = 3;
			driveRect = [self driveBay3Rect];
		}
		else
		{ 
			continue;
		}
		
		/* Create drive view */
		driveView = [[LCXSDriveView alloc] initWithDrive:driveObject slot:slot inFrame:driveRect];
		if (driveView)
		{ 
			[driveViews addObject:driveView]; 
			[self addSubview:driveView positioned:NSWindowBelow relativeTo:driveCageView];
		}
	}	
}

- (void) resetBlowerViews
{
	/* Clear old views */
	LCXSBlowerView *blowerView;
	for (blowerView in blowerViews)
	{
		[blowerView removeFromSuperview];
		[blowerView autorelease];
		blowerView = nil;
	}
	[blowerViews removeAllObjects];

	/* Check for warnings */
	if ([self hasWarnings]) return; 
	
	/* Create CPU/SC Fan views */
	int i;
	for (i=0; i < 7; i++)
	{
		/* Set blower object */
		LCEntity *blowerObject = nil;
		if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_intel"])
		{
			switch (i)
			{
				case 0:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsifans.childrenDictionary.CPU_Fan_1"];
					break;
				case 1:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsifans.childrenDictionary.CPU_Fan_2"];
					break;
				case 2:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsifans.childrenDictionary.CPU_Fan_3"];
					break;
				case 3:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsifans.childrenDictionary.CPU_Fan_4"];
					break;
				case 4:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsifans.childrenDictionary.FBDIMM_Fan_1"];
					break;
				case 5:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsifans.childrenDictionary.FBDIMM_Fan_2"];
					break;
				case 6:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsifans.childrenDictionary.FBDIMM_Fan_3"];
					break;
			}
		}
		else if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_ppc"])
		{
			switch (i)
			{
				case 0:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsfans.childrenDictionary.CPU_A1"];
					break;
				case 1:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsfans.childrenDictionary.CPU_A2"];
					break;
				case 2:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsfans.childrenDictionary.CPU_A3"];
					break;
				case 3:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsfans.childrenDictionary.System_Controller"];
					break;
				case 4:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsfans.childrenDictionary.CPU_B1"];
					break;
				case 5:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsfans.childrenDictionary.CPU_B2"];
					break;
				case 6:
					blowerObject = [device valueForKeyPath:@"childrenDictionary.xsfans.childrenDictionary.CPU_B3"];
					break;
			}
		}		
		if (!blowerObject)
		{ continue; }
		
		/* Create rect */
		float xOffset = 146;
		float yOffset = 16;
		NSRect blowerRect = NSMakeRect(NSMinX([self xsRect]) + xOffset,
									   NSMinY([self xsRect]) + yOffset + (i * 33),
									   24, 33);
		
		/* Create blower view */
		blowerView = [[LCXSBlowerView alloc] initWithBlower:blowerObject inFrame:blowerRect];
		[blowerViews addObject:blowerView];
//		[self addSubview:blowerView positioned:NSWindowBelow relativeTo:lidView];
		[self addSubview:blowerView positioned:NSWindowBelow relativeTo:airFlowView];
	}
		
}

- (void) resetCPUViews
{
	/* Clear old views */
	LCXSCPUView *cpuView;
	for (cpuView in cpuViews)
	{
		[cpuView removeFromSuperview];
		[cpuView autorelease];
		cpuView = nil;
	}
	[cpuViews removeAllObjects];

	/* Check for warnings */
	if ([self hasWarnings]) return; 
	
	/* Create CPU views */
	int i;
	for (i=0; i < 2; i++)
	{
		/* Set cpu object */
		LCObject *cpuObject = nil;
		NSRect cpuRect;
		if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_intel"])
		{
			switch (i)
			{
				case 0:
					cpuObject = [device valueForKeyPath:@"childrenDictionary.xsicpu.childrenDictionary.cpu_1"];
					cpuRect = [self cpu1Rect];
					break;
				case 1:
					cpuObject = [device valueForKeyPath:@"childrenDictionary.xsicpu.childrenDictionary.cpu_2"];
					cpuRect = [self cpu2Rect];
					break;
			}
		}
		else
		{
			switch (i)
			{
				case 0:
					cpuObject = [device valueForKeyPath:@"childrenDictionary.xscpu.childrenDictionary.cpu_1"];
					cpuRect = [self cpu1Rect];
					break;
				case 1:
					cpuObject = [device valueForKeyPath:@"childrenDictionary.xscpu.childrenDictionary.cpu_2"];
					cpuRect = [self cpu2Rect];
					break;
			}
		}
		if (!cpuObject)
		{ continue; }
		
		/* Create cpu view */
		cpuView = [[LCXSCPUView alloc] initWithCPU:cpuObject inFrame:cpuRect];
		[cpuViews addObject:cpuView];
		[self addSubview:cpuView positioned:NSWindowBelow relativeTo:airFlowView];
	}
}

- (void) resetRamStickViews
{
	/* Clear old views */
	LCXSRAMStickView *stickView;
	for (stickView in ramStickViews)
	{
		[stickView removeFromSuperview];
		[stickView autorelease];
		stickView = nil;
	}
	[ramStickViews removeAllObjects];

	/* Check for warnings */
	if ([self hasWarnings]) return; 
	
	/* Create RAM Stick views */
	int i;
	for (i=0; i < 8; i++)
	{
		/* Set ram object */
		LCEntity *stickObject = nil;
		NSRect stickRect;

		if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_intel"])
		{
			NSArray *ramObjects = [device valueForKeyPath:@"childrenDictionary.xsiram.children"];
			if ([ramObjects count] > i)
			{
				stickObject = [ramObjects objectAtIndex:i];
				stickRect = [self ramStickRect:i];
			}
		}
		else if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_ppc"])
		{
			NSArray *ramObjects = [device valueForKeyPath:@"childrenDictionary.xsram.children"];
			if ([ramObjects count] > i)
			{
				stickObject = [ramObjects objectAtIndex:i];
				stickRect = [self ramStickRect:i];
			}
		}				
		if (!stickObject)
		{ continue; }

		/* Create ram view */
		stickView = [[LCXSRAMStickView alloc] initWithRAMStick:stickObject inFrame:stickRect];
		[ramStickViews addObject:stickView];
		[self addSubview:stickView positioned:NSWindowBelow relativeTo:airFlowView];
		
	}
}

- (void) resetPowerViews
{
	/* Clear old views */
	LCXSPowerView *powerView;
	for (powerView in powerViews)
	{
		[powerView removeFromSuperview];
		[powerView autorelease];
		powerView = nil;
	}
	[powerViews removeAllObjects];

	/* Check for warnings */
	if ([self hasWarnings]) return; 
	
	if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_intel"])
	{
		/* Set power object */
		LCEntity *psu1Object = [device valueForKeyPath:@"childrenDictionary.xsipsu.childrenDictionary.PSU1"];
		LCEntity *psu2Object = [device valueForKeyPath:@"childrenDictionary.xsipsu.childrenDictionary.PSU2"];
		
		/* Create rect */
		NSRect psu1Rect = NSMakeRect (NSMinX([self xsRect]) + 309, NSMinY([self xsRect]) + 180, 116, 35);
		NSRect psu2Rect = NSMakeRect (NSMinX([self xsRect]) + 309, NSMinY([self xsRect]) + 215, 116, 35);
			
		/* Create psu views */
		if (psu1Object)
		{
			powerView = [[LCXSPowerView alloc] initWithPower:psu1Object inFrame:psu1Rect];
			[powerViews addObject:powerView];
			[self addSubview:powerView positioned:NSWindowBelow relativeTo:airFlowView];
		}
		if (psu2Object)
		{
			powerView = [[LCXSPowerView alloc] initWithPower:psu2Object inFrame:psu2Rect];
			[powerViews addObject:powerView];
			[self addSubview:powerView positioned:NSWindowBelow relativeTo:airFlowView];
		}			
	}
	else if ([[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_ppc"])
	{
		/* Set power object */
		LCEntity *powerObject = [device valueForKeyPath:@"childrenDictionary.xspower.childrenDictionary.master"];
		if (!powerObject)
		{ return; }
		
		/* Create rect */
		NSRect powerRect = NSMakeRect (NSMinX([self xsRect]) + 309, NSMinY([self xsRect]) + 180, 116, 71);
		
		/* Create power view */
		powerView = [[LCXSPowerView alloc] initWithPower:powerObject inFrame:powerRect];
		[powerViews addObject:powerView];
		[self addSubview:powerView positioned:NSWindowBelow relativeTo:airFlowView];		
	}
}

//- (void) resetAirFlowView
//{
//	if (airFlowView)
//	{
//		[airFlowView removeFromSuperview];
//		[airFlowView autorelease];
//		airFlowView = nil;
//	}
//
//	/* Check for warnings */
//	if ([self hasWarnings]) return; 
//	
//	NSRect airFlowRect = NSMakeRect(NSMinX([self xsRect]) - 20, NSMinY([self xsRect]), NSWidth([self xsRect])+40, NSHeight([self xsRect]));
//	airFlowView = [[LCXSAirFlowView alloc] initWithDevice:device inFrame:airFlowRect];
//	[self addSubview:airFlowView positioned:NSWindowBelow relativeTo:powerVisorView];
//}

- (void) resetPowerVisorView
{
	if (powerVisorView)
	{
		[powerVisorView removeFromSuperview];
		[powerVisorView autorelease];
		powerVisorView = nil;
	}
	
	/* Check for warnings */
	if ([self hasWarnings]) return; 	
	
	NSRect visorRect = NSMakeRect(NSMinX([self xsRect]) + 13, NSMinY([self xsRect]) + 9, 419, 250);
	powerVisorView = [[LCXSPowerVisorView alloc] initWithFrame:visorRect];
	[self addSubview:powerVisorView positioned:NSWindowBelow relativeTo:lidView];
	if (hidePowerVisor || ![[[device properties] objectForKey:@"vendor"] isEqualToString:@"xserve_intel"])
	{ 
		[powerVisorView setHidden:YES]; 
		hidePowerVisor = YES;
	}
}

- (void) resetLidView
{
	if (lidView)
	{
		[lidView removeFromSuperview];
		[lidView autorelease];
		lidView = nil;
	}
	
	/* Check for warnings */
	if ([self hasWarnings]) return; 	
	
	NSRect lidRect = NSMakeRect(NSMinX([self xsRect]) + 13, NSMinY([self xsRect]) + 9, 419, 250);
	lidView = [[LCXSLidView alloc] initWithFrame:lidRect];
	[self addSubview:lidView];
	if (hideLid)
	{
		[lidView setHidden:YES]; 
	}
}

- (void) resetDriveCageView
{
	if (driveCageView)
	{
		[driveCageView removeFromSuperview];
		[driveCageView autorelease];
		driveCageView = nil;
	}
	
	/* Check for warnings */
	if ([self hasWarnings]) return; 
	
	NSRect cageRect = NSMakeRect(NSMinX([self xsRect]) + 66, NSMinY([self xsRect]) + 10, 69, 247);
	driveCageView = [[LCXSDriveCageView alloc] initWithFrame:cageRect];
	[self addSubview:driveCageView positioned:NSWindowBelow relativeTo:airFlowView];
}


@synthesize xsBaseImage;
@synthesize driveViews;
@synthesize cpuViews;
@synthesize ramStickViews;
@synthesize blowerViews;
@synthesize powerViews;
@synthesize driveCageView;
@synthesize lidView;
@synthesize hideLid;
@synthesize airFlowView;
@synthesize powerVisorView;
@synthesize hidePowerVisor;
@end
