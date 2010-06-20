//
//  LCBWRepDocument.m
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBWRepDocument.h"
#import "LCBWRepWindowController.h"

@implementation LCBWRepDocument

#pragma mark "Constructors"

- (LCBWRepDocument *) init
{
	[super init];
	
	/* Create properties and dictionaries */
	properties = [[NSMutableDictionary dictionary] retain];
	[self setItems:[NSMutableArray array]];
	deviceItems = [[NSMutableDictionary dictionary] retain];
	interfaceItems = [[NSMutableDictionary dictionary] retain];
	interfaceList = [[NSMutableArray array] retain];
	groupItems = [[NSMutableDictionary dictionary] retain];
	historyRefreshList = [[NSMutableArray array] retain];
	referenceDateChangeTimer = nil;

	/* Set defaults */
	[self setStatsMode:1];					/* 95th Percentile */
	[self setViewMode:0];					/* Tree view */
	[self setDiscardMissing:YES];			/* Do discard */
	[self setReferenceDate:[NSDate date]];
	[self setReferencePeriod:4];			/* Month */
	
	return self;
}

- (void) dealloc
{
	[properties release];
	[deviceItems release];
	[interfaceList release];
	[interfaceItems release];
	[groupItems release];
	[historyRefreshList release];
	[super dealloc];
}

#pragma mark "NSDocument Methods"

- (NSString *)windowNibName 
{
	return nil;
}

- (void)makeWindowControllers
{
	windowController = [[LCBWRepWindowController alloc] initWithReportDocument:self];
	[self addWindowController:windowController];
}


- (NSData *)dataRepresentationOfType:(NSString *)type 
{
	return [NSKeyedArchiver archivedDataWithRootObject:properties];
}

- (BOOL)loadDataRepresentation:(NSData *)data ofType:(NSString *)type 
{
	/* Load data */
	[self setProperties:[NSKeyedUnarchiver unarchiveObjectWithData:data]];	
	
	/* Initialised each unarchived item with the required
	 * parent and document dictionaries 
	 */
	NSEnumerator *itemEnum = [[self items] objectEnumerator];
	LCBWRepItem *item;
	while (item=[itemEnum nextObject])
	{
		[item awakeFromArchive:self parent:nil];
	}
	
    return YES;
}

#pragma mark "KVO Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* BY default, update our change counter */
	[self updateChangeCount:NSChangeDone];
}

#pragma mark "Device Methods"

- (NSMutableDictionary *) deviceItems
{ return deviceItems; }

- (LCBWRepDevice *) locateDeviceItem:(LCEntity *)device
{
	return [deviceItems objectForKey:[[device entityAddress] addressString]];	
}

- (void) addDeviceItem:(LCBWRepDevice *)item
{
	[deviceItems setObject:item forKey:[[[item entity] entityAddress] addressString]];
}

- (void) removeDeviceItem:(LCBWRepDevice *)item
{
	[deviceItems removeObjectForKey:[[[item entity] entityAddress] addressString]];
}

#pragma mark "Group Methods"

- (NSMutableDictionary *) groupItems
{ return groupItems; }

- (LCBWRepGroup *) locateGroupItem:(NSString *)description
{
	return [groupItems objectForKey:description];
}

- (void) addGroupItem:(LCBWRepGroup *)item
{
	[groupItems setObject:item forKey:[item displayDescription]];
}

- (void) removeGroupItem:(LCBWRepGroup *)item
{
	[groupItems removeObjectForKey:[item displayDescription]];
}

- (void) updateGroup:(LCBWRepGroup *)item description:(NSString *)newDescription
{
	[groupItems removeObjectForKey:[item displayDescription]];
	[groupItems setObject:item forKey:newDescription];
}

#pragma mark "Interface Methods"

- (NSMutableDictionary *) interfaceItems
{ return deviceItems; }

- (LCBWRepInterface *) locateInterfaceItem:(LCEntity *)iface
{
	return [interfaceItems objectForKey:[[iface entityAddress] addressString]];	
}

- (void) addInterfaceItem:(LCBWRepInterface *)item
{
	[interfaceItems setObject:item forKey:[[[item entity] entityAddress] addressString]];
	[self insertObject:item inInterfaceListAtIndex:[self countOfInterfaceList]];
}

- (void) removeInterfaceItem:(LCBWRepInterface *)item
{
	[interfaceItems removeObjectForKey:[[[item entity] entityAddress] addressString]];
	[self removeObjectFromInterfaceListAtIndex:[interfaceList indexOfObject:item]];
}

- (NSMutableArray *) interfaceList
{ return interfaceList; }

- (unsigned int) countOfInterfaceList
{ return [[self interfaceList] count]; }

- (LCBWRepItem *) objectInInterfaceListAtIndex:(unsigned int)index
{
	return [[self interfaceList] objectAtIndex:index];
}

- (void) insertObject:(LCBWRepInterface *)item inInterfaceListAtIndex:(unsigned int)index
{
	[[self interfaceList] insertObject:item atIndex:index];
}

- (void) removeObjectFromInterfaceListAtIndex:(unsigned int)index
{
	[[self interfaceList] removeObjectAtIndex:index];
}

#pragma mark "Refresh Methods"

- (void) refresh:(int)priority
{
	LCBWRepInterface *iface;
	for (iface in interfaceList)
	{
		[iface refresh:priority];
	}
}

- (NSMutableArray *) historyRefreshList
{ return historyRefreshList; }

- (unsigned int) countOfHistoryRefreshList
{ return [[self historyRefreshList] count]; }

- (LCBWRepItem *) objectInHistoryRefreshListAtIndex:(unsigned int)index
{
	return [[self historyRefreshList] objectAtIndex:index];
}

- (void) insertObject:(LCMetricHistory *)item inHistoryRefreshListAtIndex:(unsigned int)index
{
	[[self historyRefreshList] insertObject:item atIndex:index];
	if ([self metricsLeftToBeRefreshed] == 0)
	{ 
		[self setTotalMetricsBeingRefreshed:0];
		[self setMetricsLeftToBeRefreshed:0];
		[windowController showRefreshSheet]; 
	}
	[self setTotalMetricsBeingRefreshed:totalMetricsBeingRefreshed+1];
	[self setMetricsLeftToBeRefreshed:metricsLeftToBeRefreshed+1];
}

- (void) removeObjectFromHistoryRefreshListAtIndex:(unsigned int)index
{
	[[self historyRefreshList] removeObjectAtIndex:index];
	[self setMetricsLeftToBeRefreshed:metricsLeftToBeRefreshed-1];
	if ([self metricsLeftToBeRefreshed] == 0)
	{ [windowController hideRefreshSheet]; }
}

- (int) totalMetricsBeingRefreshed
{ return totalMetricsBeingRefreshed; }

- (void) setTotalMetricsBeingRefreshed:(int)value
{ 
	[self willChangeValueForKey:@"metricsThatHaveBeenRefreshed"];
	totalMetricsBeingRefreshed = value; 
	[self didChangeValueForKey:@"metricsThatHaveBeenRefreshed"];
}

- (int) metricsLeftToBeRefreshed
{ return metricsLeftToBeRefreshed; }

- (void) setMetricsLeftToBeRefreshed:(int)value
{ 
	[self willChangeValueForKey:@"metricsThatHaveBeenRefreshed"];
	metricsLeftToBeRefreshed = value; 
	[self didChangeValueForKey:@"metricsThatHaveBeenRefreshed"];
}

- (int) metricsThatHaveBeenRefreshed
{
	return totalMetricsBeingRefreshed - metricsLeftToBeRefreshed; 
}

- (void) cancelRefresh
{
	LCMetricHistory *history;
	for (history in historyRefreshList)
	{
		[history cancelRefresh];
	}
}

#pragma mark "Export to CSV"

- (void) exportToCsv:(NSString *)filename
{
	/* Create header */
	NSMutableString *csvString = [NSMutableString stringWithFormat:@"group,device_name,device_desc,iface_name,iface_desc,in_min,in_avg,in_max,out_min,out_avg,out_max,in_min_95th,in_avg_95th,in_max_95th,out_min_95th,out_avg_95th,out_max_95th\n"];
	
	/* Loop through interfaces */
	LCBWRepInterface *iface;
	for (iface in interfaceList)
	{
		/* Get entities */
		LCEntity *interfaceEntity = [iface entity];
		LCEntity *deviceEntity = [interfaceEntity device];
		
		/* Get group */
		LCBWRepGroup *parentGroup = [iface parentGroup];
		NSString *groupDesc;
		if (parentGroup)
		{ groupDesc = [parentGroup displayDescription]; }
		else
		{ groupDesc = @""; }
		
		/* Get history */
		LCMetricHistory *in = [iface inMetricHistory];
		LCMetricHistory *out = [iface outMetricHistory];
		
		/* Append line */
		[csvString appendFormat:@"%@,%@,%@,%@,%@,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
			groupDesc, [deviceEntity name], [deviceEntity desc], [interfaceEntity name], [interfaceEntity desc],
			[in minimum], [in average], [in maximum],
			[out minimum], [out average], [out maximum],
			[in min95thPercentile], [in avg95thPercentile], [in max95thPercentile],
			[out min95thPercentile], [out avg95thPercentile], [out max95thPercentile]];
	}
	
	/* Write */
	BOOL result = [csvString writeToFile:filename atomically:YES];
	if (!result)
	{
		/* Error! */
		NSAlert *alert = [NSAlert alertWithMessageText:@"Failed to write CSV data to file."
										 defaultButton:@"OK" 
									   alternateButton:nil 
										   otherButton:nil 
							 informativeTextWithFormat:@"Please check the filename and directory and try again."];
		[alert runModal];
	}
}

#pragma mark "Accessors"

- (NSMutableDictionary *) properties
{ return properties; }

- (void) setProperties:(NSMutableDictionary *)newProperties
{
	if (properties) [properties release];
	properties = [newProperties retain];
}

- (int) statsMode
{ return [[properties objectForKey:@"statsMode"] intValue]; }

- (void) setStatsMode:(int)value
{ 
	/* Set will change */
	NSEnumerator *ifaceEnum = [interfaceItems objectEnumerator];
	LCBWRepInterface *iface;
	while (iface=[ifaceEnum nextObject])
	{ 
		[iface willChangeValueForKey:@"inMinimum"]; 
		[iface willChangeValueForKey:@"inAverage"]; 
		[iface willChangeValueForKey:@"inMaximum"]; 
		[iface willChangeValueForKey:@"outMinimum"]; 
		[iface willChangeValueForKey:@"outAverage"]; 
		[iface willChangeValueForKey:@"outMaximum"]; 
	}

	/* Change mode */
	[properties setObject:[NSNumber numberWithInt:value] forKey:@"statsMode"];
	
	/* Set did change */
	ifaceEnum = [interfaceItems objectEnumerator];
	while (iface=[ifaceEnum nextObject])
	{ 
		[iface didChangeValueForKey:@"inMinimum"]; 
		[iface didChangeValueForKey:@"inAverage"]; 
		[iface didChangeValueForKey:@"inMaximum"]; 
		[iface didChangeValueForKey:@"outMinimum"]; 
		[iface didChangeValueForKey:@"outAverage"]; 
		[iface didChangeValueForKey:@"outMaximum"]; 
	}
}

- (int) viewMode
{ return [[properties objectForKey:@"viewMode"] intValue]; }

- (void) setViewMode:(int)value
{
	[properties setObject:[NSNumber numberWithInt:value] forKey:@"viewMode"];
}

- (BOOL) discardMissing
{ 
	return [[properties objectForKey:@"discardMissing"] boolValue]; 
}
- (void) setDiscardMissing:(BOOL)value
{
	/* Set value */
	[properties setObject:[NSNumber numberWithBool:value] forKey:@"discardMissing"];
	
	/* Change value for each iface */
	NSEnumerator *ifaceEnum = [interfaceItems objectEnumerator];
	LCBWRepInterface *iface;
	while (iface=[ifaceEnum nextObject])
	{ 
		[iface setDiscardMissing:value];
	}
}

- (NSDate *) referenceDate
{ return referenceDate; }

- (void) setReferenceDate:(NSDate *)value
{
	[referenceDate release];
	referenceDate = [value retain];

	if (referenceDateChangeTimer)
	{ [referenceDateChangeTimer invalidate]; }
	referenceDateChangeTimer = [[NSTimer scheduledTimerWithTimeInterval:0.7
																target:self 
															  selector:@selector(refreshDateChangeTimerFired)
															  userInfo:nil
															   repeats:NO] retain];
}

- (void) refreshDateChangeTimerFired
{
	/* Change value for each iface */
	NSEnumerator *ifaceEnum = [interfaceItems objectEnumerator];
	LCBWRepInterface *iface;
	while (iface=[ifaceEnum nextObject])
	{ 
		[iface setReferenceDate:[self referenceDate]];
	}
	
	[referenceDateChangeTimer release];
	referenceDateChangeTimer = nil;
}

- (int) referencePeriod
{ return [[properties objectForKey:@"referencePeriod"] intValue]; }

- (void) setReferencePeriod:(int)value
{ 
	[properties setObject:[NSNumber numberWithInt:value] forKey:@"referencePeriod"]; 

	/* Change value for each iface */
	NSEnumerator *ifaceEnum = [interfaceItems objectEnumerator];
	LCBWRepInterface *iface;
	while (iface=[ifaceEnum nextObject])
	{ 
		[iface setReferencePeriod:[self referencePeriod]];
	}
}

#pragma mark "Item Accessors"

- (NSMutableArray *) items
{
	return [properties objectForKey:@"items"];
}

- (void) setItems:(NSMutableArray *)array
{
	[properties setObject:array forKey:@"items"];
}

- (unsigned int) countOfItems
{ return [[self items] count]; }

- (LCBWRepItem *) objectInItemsAtIndex:(unsigned int)index
{
	return [[self items] objectAtIndex:index];
}

- (void) insertObject:(LCBWRepItem *)item inItemsAtIndex:(unsigned int)index
{
	[item setReportDocument:self];
	[self updateChangeCount:NSChangeDone];
	[[self items] insertObject:item atIndex:index];
}

- (void) removeObjectFromItemsAtIndex:(unsigned int)index
{
	[self updateChangeCount:NSChangeDone];
	[[self items] removeObjectAtIndex:index];
}

@synthesize groupItems;
@synthesize interfaceList;
@synthesize historyRefreshList;
@synthesize referenceDateChangeTimer;
@synthesize windowController;
@end
