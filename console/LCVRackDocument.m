//
//  LCVirtualRackDocument.m
//  Lithium Console
//
//  Created by James Wilson on 17/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCVRackDocument.h"
#import "NSString-Base64.h"
#import "NSData-Base64.h"

@implementation LCVRackDocument

#pragma mark "Constructors"

- (id) init
{
	[super init];
	
	/* Set Type */
	self.type = @"vrack";
	
	/* Create rack units */
	rackUnits = [[NSMutableArray array] retain];
	int i;
	for (i=0; i < 48; i++)
	{
		LCVRackUnit *ru = [[[LCVRackUnit alloc] init] autorelease];
		ru.ruIndex = i+1;
		[[self rackUnits] insertObject:ru atIndex:i];
	}	
	
	/* Create arrays */
	devices = [[NSMutableArray array] retain];
	deviceDict = [[NSMutableDictionary dictionary] retain];
	cables = [[NSMutableArray array] retain];
	cableDict = [[NSMutableDictionary dictionary] retain];
	cableGroups = [[NSMutableArray array] retain];
	cableGroupDict = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc
{
	[rackUnits release];
	[devices release];
	[deviceDict release];
	[cables release];
	[cableDict release];
	[cableGroups release];
	[cableGroupDict release];
	[super dealloc];
}

#pragma mark "NSDocument Methods"

//- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
//{	
//	return [NSKeyedArchiver archivedDataWithRootObject:properties];
//}
//
//- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
//{
//	/* Load data */
//	[self setProperties:[NSKeyedUnarchiver unarchiveObjectWithData:data]];
//	
//	/* Set up the devices and rack units */
//	NSEnumerator *devEnum = [[self devices] objectEnumerator];
//	LCVRackDevice *rackDev;
//	while ((rackDev=[devEnum nextObject])!=nil)
//	{
//		/* Wire up the devices rack units */
//		int i;
//		for (i=0; i < [rackDev size]; i++)
//		{
//			LCVRackUnit *ru = [[self rackUnits] objectAtIndex:(([rackDev hostRUindex]-1) + i)];
//			[ru setHostedDevice:rackDev];
//			if (i == 0)
//			{ [rackDev setHostRU:ru]; }
//		}
//		
//		/* Add KVO */
//		[rackDev addObserver:self forKeyPath:@"properties" options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) context:nil];
//		
//		/* Perform refresh if needed */
//		[[rackDev entity] highPriorityRefresh]; 
//	}
//	
//	/* Set up cables */
//	[self bindCablestoCableGroups];
//	NSEnumerator *cableEnum = [[self cables] objectEnumerator];
//	LCVRackCable *cable;
//	while ((cable=[cableEnum nextObject])!=nil)
//	{
//		/* Add KVO */
//		[cable addObserver:self forKeyPath:@"properties" options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) context:nil];
//	}
//	
//    return YES;
//}

#pragma mark "XML Methods"

- (NSXMLDocument *) xmlDocument
{
	NSXMLDocument *xmldoc = [super xmlDocument];	
	NSXMLElement *rootNode = (NSXMLElement *) [xmldoc rootElement];
	
	/* Add Background Image */
	NSData *imageData = [[self thumbnail] TIFFRepresentation];
	if (imageData)
	{
		NSBitmapImageRep *imageRep = [[NSBitmapImageRep alloc] initWithData:imageData];
		NSXMLElement *element = [NSXMLNode elementWithName:@"thumbnail_image"];
		NSMutableDictionary *imageProperties = [NSMutableDictionary dictionary];
		[imageProperties setObject:[NSNumber numberWithFloat:0.8] forKey:@"NSImageCompressionFactor"];
		[element setObjectValue:[imageRep representationUsingType:NSPNGFileType properties:imageProperties]];
		[rootNode addChild:element];
		[imageRep autorelease];
	}
	
	/* Add RU Count */
	[rootNode addChild:[NSXMLElement elementWithName:@"ru_count" 
										 stringValue:[NSString stringWithFormat:@"%i", self.rackUnits.count]]];
	
	/* Add rack elements */
	for (LCVRackDevice *device in devices)
	{ [rootNode addChild:[device xmlNode]]; }
	for (LCVRackCable *cable in cables)
	{ [rootNode addChild:[cable xmlNode]]; }
	for (LCVRackCableGroup *group in cableGroups)
	{ [rootNode addChild:[group xmlNode]]; }
	
	return xmldoc;	
}

- (void) setXmlValuesUsingXmlNode:(LCXMLNode *)xmlNode
{
	[super setXmlValuesUsingXmlNode:xmlNode];	
	
	NSString *thumbnailString = [xmlNode.properties objectForKey:@"thumbnail_image"];
	if (thumbnailString && [thumbnailString length] > 0)
	{
		NSData *imageData = [NSData dataWithBase64EncodedString:thumbnailString];
		if (imageData) self.thumbnail = [[[NSImage alloc] initWithData:imageData] autorelease];
	}	
	
	for (LCXMLNode *childNode in xmlNode.children)
	{
		if ([childNode.name isEqualToString:@"device"])
		{
			NSString *uuid = [childNode.properties objectForKey:@"uuid"];
			LCVRackDevice *device = [self.deviceDict objectForKey:uuid];
			if (!device)
			{
				device = [LCVRackDevice new];
				device.uuid = uuid;
				[device autorelease];
			}
			[device setXmlValuesUsingXmlNode:childNode];
			
			/* Add device to list if new */
			if (![devices containsObject:device])
			{
				[self insertObject:device inDevicesAtIndex:[devices count]];
			}
		}
		else if ([childNode.name isEqualToString:@"cable"])
		{
			NSString *uuid = [childNode.properties objectForKey:@"uuid"];
			LCVRackCable *cable = [self.cableDict objectForKey:uuid];
			if (!cable)
			{
				cable = [LCVRackCable new];
				cable.uuid = uuid;
				[cable autorelease];
			}
			[cable setXmlValuesUsingXmlNode:childNode];
			
			/* Add cable to list if new */
			if (![cables containsObject:cable])
			{
				[self insertObject:cable inCablesAtIndex:[cables count]];
			}
		}
		else if ([childNode.name isEqualToString:@"cablegroup"])
		{
			NSString *uuid = [childNode.properties objectForKey:@"uuid"];
			LCVRackCableGroup *group = [self.cableGroupDict objectForKey:uuid];
			if (!group)
			{
				group = [LCVRackCableGroup new];
				group.uuid = uuid;
				[group autorelease];
			}
			[group setXmlValuesUsingXmlNode:childNode];
			
			/* Add group to list if new */
			if (![cableGroups containsObject:group])
			{
				[self insertObject:group inCableGroupsAtIndex:[cableGroups count]];
			}
		}
	}
}

- (NSString *) xmlRootElement
{ return @"vrack"; }

#pragma mark "Thumbnail Accessors"

@synthesize thumbnail;

#pragma mark "Rack Unit Properties"

@synthesize rackUnits;

#pragma mark "Device Properties"

@synthesize devices;
- (LCVRackDevice *) objectInDevicesAtIndex:(unsigned int)index
{
	return [[self devices] objectAtIndex:index];
}
- (void) insertObject:(LCVRackDevice *)device inDevicesAtIndex:(unsigned int)index
{
	[[self devices] insertObject:device atIndex:index];
	[deviceDict setObject:device forKey:device.uuid];
	
	/* Find device RU */
	if (!device.hostRU && device.hostRUindex > 0 && (device.hostRUindex - 1) < rackUnits.count)
	{
		device.hostRU = [rackUnits objectAtIndex:device.hostRUindex-1];
		[(LCVRackUnit *) device.hostRU setHostedDevice:device];
	}
}
- (void) removeObjectFromDevicesAtIndex:(unsigned int)index
{
	[deviceDict removeObjectForKey:[[devices objectAtIndex:index] uuid]];
	[[self devices] removeObjectAtIndex:index];
}
- (void) removeDevice:(LCVRackDevice *)rackDev
{
	if (![[self devices] containsObject:rackDev]) return;
	for (LCVRackUnit *rackUnit in rackUnits)
	{ 
		if (rackUnit.hostedDevice == rackDev)
		{ [rackUnit setHostedDevice:nil]; }
	}
	[self removeObjectFromDevicesAtIndex:[[self devices] indexOfObject:rackDev]];
}
- (void) incrementSizeOfDevice:(LCVRackDevice *)rackDev
{
	/* Check availability of rack units */
	int targetSize = [rackDev size] + 1;
	int targetRUIndex = [rackDev hostRUindex] + (targetSize - 1);
	if ((targetRUIndex - 1) >= [[self rackUnits] count])
	{
		/* No rack units */
		return; 
	}	
	LCVRackUnit *targetRU = [[self rackUnits] objectAtIndex:(targetRUIndex - 1)];
	if ([targetRU hostedDevice])
	{
		/* Rack unit is occupied */
		return; 
	}
	
	/* Adjust size and rack units */
	[rackDev setSize:targetSize];
	[targetRU setHostedDevice:rackDev];
}
- (void) decrementSizeOfDevice:(LCVRackDevice *)rackDev
{
	/* Adjust previous host */
	int targetRUIndex = [rackDev hostRUindex] + ([rackDev size] - 1);
	LCVRackUnit *targetRU = [[self rackUnits] objectAtIndex:(targetRUIndex - 1)];
	int targetSize = [rackDev size] - 1;
	
	/* Adjust size and rack units */
	[rackDev setSize:targetSize];
	[targetRU setHostedDevice:nil];
}
@synthesize deviceDict;

#pragma mark "Cable Properties"

@synthesize cables;
- (LCVRackCable *) objectInCablesAtIndex:(unsigned int)index
{
	return [[self cables] objectAtIndex:index];
}
- (void) insertObject:(LCVRackCable *)cable inCablesAtIndex:(unsigned int)index
{
	[[self cables] insertObject:cable atIndex:index];
	[cableDict setObject:cable forKey:cable.uuid];
}

- (void) removeObjectFromCablesAtIndex:(unsigned int)index
{
	[cableDict removeObjectForKey:[[cables objectAtIndex:index] uuid]];
	[[self cables] removeObjectAtIndex:index];
}
@synthesize cableDict;

#pragma mark "Cable Group Properties"

@synthesize cableGroups;
- (LCVRackCableGroup *) objectInCableGroupsAtIndex:(unsigned int)index
{
	return [[self cableGroups] objectAtIndex:index];
}
- (void) insertObject:(LCVRackCableGroup *)group inCableGroupsAtIndex:(unsigned int)index
{
	[[self cableGroups] insertObject:group atIndex:index];	
	[cableGroupDict setObject:group forKey:group.uuid];
}
- (void) removeObjectFromCableGroupsAtIndex:(unsigned int)index
{
	[cableGroupDict removeObjectForKey:[[cableGroups objectAtIndex:index] uuid]];
	[[self cableGroups] removeObjectAtIndex:index];
}
- (void) removeCableGroup:(LCVRackCableGroup *)group
{
	/* Remove all references */
	for (LCVRackCable *cable in cables)
	{
		if ([cable cableGroup] == group)
		{ [cable setCableGroup:nil]; }
	}
	
	/* Remove */
	[self removeObjectFromCableGroupsAtIndex:[[self cableGroups] indexOfObject:group]];
}
- (void) bindCablestoCableGroups
{
	for (LCVRackCable *cable in cables)
	{
		for (LCVRackCableGroup *group in cableGroups)
		{
			if ([[group desc] isEqualToString:cable.cableGroup.desc])
			{ [cable setCableGroup:group]; }
		}
	}
}
@synthesize cableGroupDict;

@end
