//
//  LCBWRepInterface.m
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBWRepInterface.h"
#import "LCEntityDescriptor.h"
#import "LCBWRepDocument.h"

@implementation LCBWRepInterface

#pragma mark "Constructors"

+ (LCBWRepInterface *) interfaceItemWithEntity:(LCEntity *)initEntity
{
	return [[[LCBWRepInterface alloc] initWithEntity:initEntity] autorelease];
}

- (LCBWRepInterface *) init
{
	[super init];
	[self setType:BWREP_ITEM_INTERFACE];
	return self;
}

- (LCBWRepInterface *) initWithEntity:(LCEntity *)initEntity
{
	[super initWithEntity:initEntity];
	
	[self setType:BWREP_ITEM_INTERFACE];

	return self;
}

- (LCBWRepItem *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	[self setProperties:[[decoder decodeObjectForKey:@"properties"] retain]];	
    return self;
}

#pragma mark "History Methods"

- (void) createHistoryObjects
{
	/* Create descriptors to locate input and output bits/sec */
	LCEntityDescriptor *inMetricDesc = [LCEntityDescriptor descriptorForEntity:[self entity]];
	[[inMetricDesc properties] setObject:@"6" forKey:@"type_num"];
	[[inMetricDesc properties] setObject:@"bps_in" forKey:@"name"];
	[[inMetricDesc properties] setObject:@"Input Bits Per Second" forKey:@"desc"];
	[[inMetricDesc properties] setObject:@"bps_in" forKey:@"met_name"];
	[[inMetricDesc properties] setObject:@"Input Bits Per Second" forKey:@"met_desc"];
	LCEntityDescriptor *outMetricDesc = [LCEntityDescriptor descriptorForEntity:[self entity]];
	[[outMetricDesc properties] setObject:@"6" forKey:@"type_num"];
	[[outMetricDesc properties] setObject:@"bps_out" forKey:@"name"];
	[[outMetricDesc properties] setObject:@"Output Bits Per Second" forKey:@"desc"];
	[[outMetricDesc properties] setObject:@"bps_out" forKey:@"met_name"];
	[[outMetricDesc properties] setObject:@"Output Bits Per Second" forKey:@"met_desc"];
	
	/* Create history classes */
	inHistory = [[LCMetricHistory historyWithMetric:[inMetricDesc locateEntity:YES]] retain];
	[inHistory setDiscardNanFor95th:[reportDocument discardMissing]];
	outHistory = [[LCMetricHistory historyWithMetric:[outMetricDesc locateEntity:YES]] retain];
	[outHistory setDiscardNanFor95th:[reportDocument discardMissing]];
	
	/* Add observers */
	[inHistory addObserver:self 
				forKeyPath:@"refreshInProgress" 
				   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				   context:NULL];	
	[outHistory addObserver:self 
		 	 	 forKeyPath:@"refreshInProgress" 
				    options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				    context:NULL];	
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	LCBWRepDocument *doc = [self reportDocument];
	if (object == inHistory)
	{
		if ([inHistory refreshInProgress])
		{
			inHistoryRefreshInProgress = YES;
			[doc insertObject:inHistory inHistoryRefreshListAtIndex:[doc countOfHistoryRefreshList]];
		}
		else
		{
			[self willChangeValueForKey:@"inMinimum"];
			[self willChangeValueForKey:@"inAverage"];
			[self willChangeValueForKey:@"inMaximum"];
			inHistoryRefreshInProgress = NO;
			[doc removeObjectFromHistoryRefreshListAtIndex:[[doc historyRefreshList] indexOfObject:inHistory]];
			[self didChangeValueForKey:@"inMinimum"];
			[self didChangeValueForKey:@"inAverage"];
			[self didChangeValueForKey:@"inMaximum"];
		}
	}
	else if (object == outHistory)
	{
		if ([outHistory refreshInProgress])
		{
			outHistoryRefreshInProgress = YES;
			[doc insertObject:outHistory inHistoryRefreshListAtIndex:[doc countOfHistoryRefreshList]];
		}
		else			
		{
			[self willChangeValueForKey:@"outMinimum"];
			[self willChangeValueForKey:@"outAverage"];
			[self willChangeValueForKey:@"outMaximum"];
			outHistoryRefreshInProgress = NO;
			[doc removeObjectFromHistoryRefreshListAtIndex:[[doc historyRefreshList] indexOfObject:outHistory]];
			[self didChangeValueForKey:@"outMinimum"];
			[self didChangeValueForKey:@"outAverage"];
			[self didChangeValueForKey:@"outMaximum"];
		}
	}
}

#pragma mark "Refresh"

- (void) refresh:(int)priority
{
	[inHistory refresh:priority];
	[outHistory refresh:priority];
}

#pragma mark Report Document 

- (void) setReportDocument:(id)value
{ 
	/* Set document */
	reportDocument = value; 
	
	/* Create history objects */
	[self createHistoryObjects];
	[self setReferenceDate:[reportDocument referenceDate]];
	[self setReferencePeriod:[reportDocument referencePeriod]];
}

#pragma mark "Accessors"

- (NSString *) displayDescription
{ 
	LCBWRepGroup *parentGroup = [self parentGroup];
	if (!parentGroup || [parentGroup arrangeByDevice])
	{ return [[self entity] displayString]; }
	else 
	{ return [NSString stringWithFormat:@"%@: %@", [[[self entity] device] displayString], [[self entity] displayString]]; }
}

- (NSNumber *) inMinimum
{ 
	if ([(LCBWRepDocument *)reportDocument statsMode] == 1)
	{ return [inHistory min95thPercentileNumber]; }
	else if ([(LCBWRepDocument *)reportDocument statsMode] == 2)
	{ return [inHistory minimumNumber]; }
	else 
	{ return nil; }
}

- (NSNumber *) inAverage
{ 
	if ([(LCBWRepDocument *)reportDocument statsMode] == 1)
	{ return [inHistory avg95thPercentileNumber]; }
	else if ([(LCBWRepDocument *)reportDocument statsMode] == 2)
	{ return [inHistory averageNumber]; }
	else 
	{ return nil; }
}

- (NSNumber *) inMaximum
{ 
	if ([(LCBWRepDocument *)reportDocument statsMode] == 1)
	{ return [inHistory max95thPercentileNumber]; }
	else if ([(LCBWRepDocument *)reportDocument statsMode] == 2)
	{ return [inHistory maximumNumber]; }
	else 
	{ return nil; }
}

- (NSNumber *) outMinimum
{ 
	if ([(LCBWRepDocument *)reportDocument statsMode] == 1)
	{ return [outHistory min95thPercentileNumber]; }
	else if ([(LCBWRepDocument *)reportDocument statsMode] == 2)
	{ return [outHistory minimumNumber]; }
	else 
	{ return nil; }
}

- (NSNumber *) outAverage
{ 
	if ([(LCBWRepDocument *)reportDocument statsMode] == 1)
	{ return [outHistory avg95thPercentileNumber]; }
	else if ([(LCBWRepDocument *)reportDocument statsMode] == 2)
	{ return [outHistory averageNumber]; }
	else 
	{ return nil; }
}

- (NSNumber *) outMaximum
{ 
	if ([(LCBWRepDocument *)reportDocument statsMode] == 1)
	{ return [outHistory max95thPercentileNumber]; }
	else if ([(LCBWRepDocument *)reportDocument statsMode] == 2)
	{ return [outHistory maximumNumber]; }
	else 
	{ return nil; }
}

- (void) setDiscardMissing:(BOOL)value
{
	[self willChangeValueForKey:@"inMinimum"]; 
	[self willChangeValueForKey:@"inAverage"]; 
	[self willChangeValueForKey:@"inMaximum"]; 
	[inHistory setDiscardNanFor95th:value];
	[self didChangeValueForKey:@"inMinimum"]; 
	[self didChangeValueForKey:@"inAverage"]; 
	[self didChangeValueForKey:@"inMaximum"]; 
	
	[self willChangeValueForKey:@"outMinimum"]; 
	[self willChangeValueForKey:@"outAverage"]; 
	[self willChangeValueForKey:@"outMaximum"]; 	
	[outHistory setDiscardNanFor95th:value];
	[self didChangeValueForKey:@"outMinimum"]; 
	[self didChangeValueForKey:@"outAverage"]; 
	[self didChangeValueForKey:@"outMaximum"]; 
}	

- (void) setReferenceDate:(NSDate *)value
{
	[inHistory setReferenceDate:value];
	[outHistory setReferenceDate:value];
}	

- (void) setReferencePeriod:(int)value
{
	[inHistory setGraphPeriod:value];
	[outHistory setGraphPeriod:value];
}	

- (LCMetricHistory *) inMetricHistory
{ return inHistory; }

- (LCMetricHistory *) outMetricHistory
{ return outHistory; }

@synthesize inHistory;
@synthesize inHistoryRefreshInProgress;
@synthesize outHistory;
@synthesize outHistoryRefreshInProgress;
@end
