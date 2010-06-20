//
//  LCVRackCable.m
//  Lithium Console
//
//  Created by James Wilson on 11/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackCable.h"
#import "LCEntityDescriptor.h"

@interface LCVRackCable (private)

- (LCEntity *) extractEntityFromEndNode:(LCXMLNode *)xmlNode;

@end

@implementation LCVRackCable

#pragma mark Constructors

- (id) initWithAEnd:(LCEntity *)initAEnd bEnd:(LCEntity *)initBEnd
{
	[self init];
	
	/* Set ends */
	self.aEndInterface = initAEnd;
	self.bEndInterface = initBEnd;
	
	return self;
}

- (id) init
{
	[super init];

	/* Set defaults */
	self.colour = [NSColor colorWithCalibratedRed:0.2 green:0.2 blue:0.8 alpha:1.0];
	CFUUIDRef uuidRef = CFUUIDCreate(nil);
	self.uuid = (NSString *)CFUUIDCreateString(nil, uuidRef);
    CFRelease(uuidRef);	
	
	/* Setup XML Translation */
	if (!self.xmlTranslation)
	{
		self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"vlans" forKey:@"vlans"];
		[self.xmlTranslation setObject:@"notes" forKey:@"notes"];
		[self.xmlTranslation setObject:@"locked" forKey:@"locked"];
	}		
	
	return self;
}

- (void) dealloc
{
	[vlans release];
	[notes release];
	[aEndInterface release];
	[aEndDevice release];
	[bEndInterface release];
	[bEndDevice release];
	[cableGroup release];
	[super dealloc];
}

#pragma mark "Cable Colour"

@synthesize colour;
- (NSColor *) colourWithAlpha:(float)alpha
{
	return [NSColor colorWithCalibratedRed:[[self colour] redComponent] green:[[self colour] greenComponent] blue:[[self colour] blueComponent] alpha:alpha];
}
- (NSColor *) opStateColourWithAlpha:(float)alpha
{
	/* Get states */
	int aEndOpState = [[[self aEndInterface] opstateInteger] intValue];
	int bEndOpState = [[[self bEndInterface] opstateInteger] intValue];
	
	/* Find highest state */
	if (aEndOpState >= bEndOpState)
	{ return [[self aEndInterface] opStateColorWithAlpha:alpha]; }
	else
	{ return [[self bEndInterface] opStateColorWithAlpha:alpha]; }
}

#pragma mark "XML Methods"

- (void) setXmlValuesUsingXmlNode:(LCXMLNode *)xmlNode
{
	[super setXmlValuesUsingXmlNode:xmlNode];
	
	self.colour = [NSColor colorWithCalibratedRed:[[xmlNode.properties objectForKey:@"color_red"] floatValue]
											green:[[xmlNode.properties objectForKey:@"color_green"] floatValue]
											 blue:[[xmlNode.properties objectForKey:@"color_blue"] floatValue]
											alpha:1.0];
	
	for (LCXMLNode *childNode in xmlNode.children)
	{
		if ([childNode.name isEqualToString:@"a_end"])
		{
			self.aEndInterface = [self extractEntityFromEndNode:childNode];
		}
		else if ([childNode.name isEqualToString:@"b_end"])
		{
			self.bEndInterface = [self extractEntityFromEndNode:childNode];
		}
	}	
}

- (LCEntity *) extractEntityFromEndNode:(LCXMLNode *)xmlNode
{
	for (LCXMLNode *childNode in xmlNode.children)
	{
		if ([childNode.name isEqualToString:@"entity_descriptor"])
		{
			LCEntityDescriptor *entityDescriptor = [LCEntityDescriptor descriptorWithXmlNode:childNode];
			return [entityDescriptor locateEntity:YES];
		}
	}
	return nil;
}

- (NSXMLElement *) xmlNode
{
	NSXMLElement *rootNode = (NSXMLElement *) [super xmlNode];

	[rootNode addChild:[NSXMLNode elementWithName:@"uuid" stringValue:self.uuid]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"color_red" stringValue:[NSString stringWithFormat:@"%.2f", [colour redComponent]]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"color_green" stringValue:[NSString stringWithFormat:@"%.2f", [colour greenComponent]]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"color_blue" stringValue:[NSString stringWithFormat:@"%.2f", [colour blueComponent]]]]; 
	if (self.aEndInterface)
	{
		NSXMLElement *endNode = [NSXMLElement elementWithName:@"a_end"];
		[endNode addChild:[[self.aEndInterface entityDescriptor] xmlNode]];
		[rootNode addChild:endNode];
	}
	if (self.bEndInterface)
	{
		NSXMLElement *endNode = [NSXMLElement elementWithName:@"b_end"];
		[endNode addChild:[[self.bEndInterface entityDescriptor] xmlNode]];
		[rootNode addChild:endNode];
	}
	
	return (NSXMLElement *) rootNode;	
}

- (NSString *) xmlRootElement
{ return @"cable"; }

#pragma mark "Document Import"

- (LCVRackCable *) initWithCoder:(NSCoder *)decoder
{
	/*
	 * This is only used for document importing 
	 */
	[self init];
	
	NSDictionary *properties = [decoder decodeObjectForKey:@"properties"];
	self.aEndInterface = [properties objectForKey:@"aEndInterface"];
	self.bEndInterface = [properties objectForKey:@"bEndInterface"];
	if ([properties objectForKey:@"colour"]) self.colour = [properties objectForKey:@"colour"];
	
    return self;
}

#pragma mark "Properties"

@synthesize uuid;
- (LCEntity *) aEndDevice
{ 
	return aEndDevice ? : [[self aEndInterface] device];
}
- (void) setAEndDevice:(LCEntity *)newDevice
{ 
	[aEndDevice release];
	aEndDevice = [newDevice retain];
	
	if (aEndInterface.device != aEndDevice)
	{ self.aEndInterface = nil; }
}
- (LCEntity *) bEndDevice
{ 
	return bEndDevice ? : [[self bEndInterface] device];
}
- (void) setBEndDevice:(LCEntity *)newDevice
{ 
	[bEndDevice release];
	bEndDevice = [newDevice retain];
	if (bEndInterface.device != bEndDevice)
	{ self.bEndInterface = nil; }
}
@synthesize aEndInterface;
@synthesize bEndInterface;
@synthesize vlans;
@synthesize notes;
@synthesize highlighted;
@synthesize cableGroup;
@synthesize locked;
@synthesize orphaned;
@synthesize abSpeedIndicatorT;
@synthesize abSpeedIndex;
@synthesize baSpeedIndicatorT;
@synthesize baSpeedIndex;

- (void) incrementAbSpeedIndicatorT
{ 
	abSpeedIndicatorT = abSpeedIndicatorT + ((float) abSpeedIndex / 20000.0);
	if (abSpeedIndicatorT >= 1) abSpeedIndicatorT = 0.0;
}

- (void) incrementBaSpeedIndicatorT
{ 
	baSpeedIndicatorT = baSpeedIndicatorT + ((float) baSpeedIndex / 20000.0);
	if (baSpeedIndicatorT >= 1) baSpeedIndicatorT = 0.0;
}

@end
