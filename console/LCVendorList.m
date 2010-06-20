//
//  LCVendorList.m
//  Lithium Console
//
//  Created by James Wilson on 7/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVendorList.h"

#import "LCCustomer.h"
#import "LCVendor.h"

@implementation LCVendorList

#pragma mark Constructors

+ (LCVendorList *) vendorListWithCustomer:(id)initCustomer
{
	return [[[LCVendorList alloc] initWithCustomer:initCustomer] autorelease];
}

- (LCVendorList *) initWithCustomer:(id)initCustomer
{
	[self init];
	customer = initCustomer;
	return self;
}

- (LCVendorList *) init
{
	[super init];
	vendors = [[NSMutableArray array] retain];
	vendorDictionary = [[NSMutableDictionary dictionary] retain];
	return self;
}

- (void) dealloc
{
	[vendors release];
	[vendorDictionary release];
	if (refreshXMLRequest) { [refreshXMLRequest cancel]; [refreshXMLRequest release]; }
	if (curXMLDictionary) [curXMLDictionary release];
	if (curXMLString) [curXMLString release];
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	if (!customer) return;
	if (refreshXMLRequest) return;
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[(LCCustomer *)customer resourceAddress] 
													entity:[(LCCustomer *)customer entityAddress] 
												   xmlname:@"vendor_list"
													refsec:0 
													xmlout:nil] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setXMLDelegate:self];
	[refreshXMLRequest setPriority:priority];
	[refreshXMLRequest performAsyncRequest];
	
	[self setRefreshInProgress:YES];
}

- (void) XMLRequestPreParse:(id)sender
{
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Check for start of a vendor */
	if ([element isEqualToString:@"vendor_module"])
	{
		if (curXMLDictionary) [curXMLDictionary release];
		curXMLDictionary = [[NSMutableDictionary dictionary] retain];
	}
	
	/* Free previous curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
		curXMLString = nil;
	}
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Append characters to curXMLString */
	if (!curXMLString)
	{ curXMLString = [[NSMutableString stringWithString:string] retain]; }
	else
	{ [curXMLString appendString:string]; }
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Add element string to dict */
	if (curXMLString && curXMLDictionary)
	{ 
		[curXMLDictionary setObject:curXMLString forKey:element]; 
	}
	
	/* Check for end of a vendor */
	if ([element isEqualToString:@"vendor_module"])
	{
		NSString *name = [curXMLDictionary objectForKey:@"name"];
		if ([vendorDictionary objectForKey:name])
		{
			/* Existing */
			LCVendor *vendor = [vendorDictionary objectForKey:name];
			NSEnumerator *keyEnum = [curXMLDictionary keyEnumerator];
			NSString *key;
			while (key = [keyEnum nextObject])
			{ [[vendor properties] setObject:[curXMLDictionary objectForKey:key] forKey:key]; }
		}
		else
		{
			/* Create vendor */
			LCVendor *vendor = [LCVendor vendorWithProperties:curXMLDictionary];
		
			/* Add to list */
			[self insertObject:vendor inVendorsAtIndex:[vendors count]];
		
			/* Release current dictionary */
			[curXMLDictionary release];
			curXMLDictionary = nil;
		}
	}
	
	/* Free current curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
		curXMLString = nil;
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{	
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LCVendorListRefreshFinished" object:self];
}

#pragma mark "Vendor Accessor Methods"

- (NSArray *) vendors 
{ 
	return (NSArray *) vendors; 
}

- (void) insertObject:(id)vendor inVendorsAtIndex:(unsigned int)index
{ 
	[vendorDictionary setObject:vendor forKey:[vendor name]];
	[vendors insertObject:vendor atIndex:index];
}

- (void) removeObjectFromVendorsAtIndex:(unsigned int) index
{ 
	if ([vendors objectAtIndex:index])
	{ [vendorDictionary removeObjectForKey:[[vendors objectAtIndex:index] name]]; }
	[vendors removeObjectAtIndex:index];
}

- (NSDictionary *) vendorDictionary 
{ return vendorDictionary; };

#pragma mark "General Accessor Methods"

@synthesize refreshInProgress;

@end
