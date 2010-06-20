//
//  LCSSceneDocument.m
//  Lithium Console
//
//  Created by James Wilson on 27/08/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCSSceneDocument.h"
#import "LCSSceneWindowController.h"
#import "NSData-Base64.h"

@interface LCSSceneDocument (private)
- (void) updateStatusString;
@end

@implementation LCSSceneDocument

#pragma mark "Constructors"

- (LCSSceneDocument *) init
{
	[super init];
	
	/* Set Type */
	self.type = @"scene";

	/* Create blank overlay list */
	overlays = [[NSMutableArray array] retain];
	overlayDictionary = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc
{
	[backgroundImage release];
	[overlays release];
	[overlayDictionary release];
	
	[super dealloc];
}

#pragma mark "Old NSDocument Methods"

//- (NSData *)dataRepresentationOfType:(NSString *)type 
//{
//	return [NSKeyedArchiver archivedDataWithRootObject:properties];
//}
//
//- (BOOL)loadDataRepresentation:(NSData *)data ofType:(NSString *)type 
//{
//	/* Load data */
//	[self setProperties:[NSKeyedUnarchiver unarchiveObjectWithData:data]];	
//	
//	/* Set overlay observer */
//	NSEnumerator *overlayEnum = [[self overlays] objectEnumerator];
//	LCSSceneOverlay *overlay;
//	while ((overlay=[overlayEnum nextObject])!=nil)
//	{ [overlay addObserver:self forKeyPath:@"properties" options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) context:nil]; }
//	
//    return YES;
//}

#pragma mark "XML Methods"

- (NSXMLDocument *) xmlDocument
{
	NSXMLDocument *xmldoc = [super xmlDocument];	
	NSXMLElement *rootNode = (NSXMLElement *) [xmldoc rootElement];

	/* Add Background Image */
	NSData *imageData = [[self backgroundImage] TIFFRepresentation];
	if (imageData)
	{
		NSBitmapImageRep *imageRep = [[NSBitmapImageRep alloc] initWithData:imageData];
		NSXMLElement *element = [NSXMLNode elementWithName:@"background_image"];
		NSMutableDictionary *imageProperties = [NSMutableDictionary dictionary];
		[imageProperties setObject:[NSNumber numberWithFloat:0.8] forKey:@"NSImageCompressionFactor"];
		[element setObjectValue:[imageRep representationUsingType:NSJPEGFileType properties:imageProperties]];
		[rootNode addChild:element];
		[imageRep autorelease];
	}
	
	/* Add Overlays */
	for (LCSSceneOverlay *overlay in [self overlays])
	{
		NSXMLNode *overlayNode = [overlay xmlNode];
		[rootNode addChild:overlayNode];
	}
	
	return xmldoc;	
}

- (void) setXmlValuesUsingXmlNode:(LCXMLNode *)xmlNode
{
	[super setXmlValuesUsingXmlNode:xmlNode];
	
	NSString *backgroundString = [xmlNode.properties objectForKey:@"background_image"];
	if (backgroundString && [backgroundString length] > 0)
	{
		NSData *imageData = [NSData dataWithBase64EncodedString:backgroundString];
		if (imageData) self.backgroundImage = [[[NSImage alloc] initWithData:imageData] autorelease];
	}
	
	for (LCXMLNode *childNode in xmlNode.children)
	{
		if ([childNode.name isEqualToString:@"overlay"])
		{
			NSString *uuid = [childNode.properties objectForKey:@"uuid"];
			LCSSceneOverlay *overlay = [self.overlayDictionary objectForKey:uuid];
			if (!overlay)
			{
				overlay = [LCSSceneOverlay new];
				overlay.uuid = uuid;
				[overlay autorelease];
			}
			[overlay setXmlValuesUsingXmlNode:childNode];
			
			/* Add if new -- this MUST be done after the XML values
			 * are set to ensure the overlay properties are all set 
			 * before the overlay is added
			 */
			if (![overlays containsObject:overlay])
			{
				[self insertObject:overlay inOverlaysAtIndex:[overlays count]];				
			}
		}
	}
}

- (NSString *) xmlRootElement
{ return @"scene"; }

#pragma mark "Properties"

@synthesize backgroundImage;

@synthesize overlays;
- (void) insertObject:(LCSSceneOverlay *)overlay inOverlaysAtIndex:(unsigned int)index
{
	[overlays insertObject:overlay atIndex:index];
	[overlayDictionary setObject:overlay forKey:overlay.uuid];
}

- (void) removeObjectFromOverlaysAtIndex:(unsigned int)index
{
	[overlayDictionary removeObjectForKey:[[overlays objectAtIndex:index] uuid]];
	[overlays removeObjectAtIndex:index];
}
@synthesize overlayDictionary;

- (NSImage *) treeIcon
{
	return [NSImage imageNamed:@"pictures_16.tif"];
}
	
@end
