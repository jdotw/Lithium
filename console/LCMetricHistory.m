//
//  LCMetricHistory.m
//  Lithium Console
//
//  Created by James Wilson on 18/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricHistory.h"
#import "LCMetricHistoryRow.h"
#import "LCEntityDescriptor.h"
#import "NSString-ScaledValue.h"

@implementation LCMetricHistory

#pragma mark "Initialisation"

+ (id) historyWithMetric:(LCMetric *)initMetric
{
	return [[[LCMetricHistory alloc] initWithMetric:initMetric] autorelease];
}

- (id) initWithMetric:(LCMetric *)initMetric
{
	[self init];
	[self setMetric:initMetric];
	[self setMinimum:NAN];
	[self setAverage:NAN];
	[self setMaximum:NAN];
	return self;
}

- (LCMetricHistory *) init
{
	[super init];

	/* Create objects */
	rows = [[NSMutableArray array] retain];
	
	/* Set defaults */
	graphPeriod = 1;
	referenceDate = [[NSDate dateWithTimeIntervalSinceNow:0.0] retain];
	
	return self;
}

- (void) dealloc
{
	if (refreshXMLRequest)
	{
		[refreshXMLRequest cancel];
		[refreshXMLRequest release];
	}
	[rows release];
	[metric release];
	[referenceDate release];
	[maximumNumber release];
	[averageNumber release];
	[minimumNumber release];
	[maximumString release];
	[averageString release];
	[minimumString release];
	[super dealloc];
}

#pragma mark "Refresh Method"

- (void) refresh:(int)priority
{
	/* Set summary values */
	[self setMinimum:NAN];
	[self setAverage:NAN];
	[self setMaximum:NAN];
	
	/* Set current element to nil */
	while (rows.count > 0)
	{
		[self removeObjectFromRowsAtIndex:0];
	}
	
	/* Check progress */
	if (refreshInProgress)
	{
		/* Refresh is in progress, cancel it */
		[self cancelRefresh];
	}
	
	/* Create XML Doc */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"criteria"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	LCEntityDescriptor *entDesc = [LCEntityDescriptor descriptorForEntity:metric];
	NSXMLElement *metricNode = (NSXMLElement *) [entDesc xmlNode];
	[rootNode addChild:metricNode];
	
	/* Set reference date and graph period */
	if (referenceDate)
	{
		NSString *str = [NSString stringWithFormat:@"%f", [referenceDate timeIntervalSince1970]];
		[rootNode addChild:[NSXMLNode elementWithName:@"ref_sec" stringValue:str]];
		str = [NSString stringWithFormat:@"%i", graphPeriod];
		[rootNode addChild:[NSXMLNode elementWithName:@"period" stringValue:str]];	
	}
	else
	{
		NSString *str = [NSString stringWithFormat:@"%f", [[NSDate dateWithTimeIntervalSinceNow:0.0] timeIntervalSince1970]];
		[rootNode addChild:[NSXMLNode elementWithName:@"ref_sec" stringValue:str]];
		str = [NSString stringWithFormat:@"%i", graphPeriod];
		[rootNode addChild:[NSXMLNode elementWithName:@"period" stringValue:str]];
	}
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:[metric customer]
												  resource:[[metric device] resourceAddress]
													entity:[metric entityAddress] 
												   xmlname:@"rrdxport" 
													refsec:0
													xmlout:xmlDoc] retain];
	[refreshXMLRequest setDelegate:self];
 	[refreshXMLRequest setXMLDelegate:self];
	[refreshXMLRequest setPriority:priority];
	
	/* Perform XML request */
	[refreshXMLRequest performAsyncRequest];
	[self setRefreshInProgress:YES];	
}

- (void) cancelRefresh
{
	if (refreshXMLRequest)
	{ 
		[refreshXMLRequest cancel]; 
		[refreshXMLRequest release];
		refreshXMLRequest = nil; 
	}
	
	[self setRefreshInProgress:NO];
}

- (void) XMLRequestPreParse:(id)sender
{
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Free previous curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
 		curXMLString = nil;
	}
	
	/* Check if this is the start of a row */
	if ([element isEqualToString:@"row"])
	{
		LCMetricHistoryRow *row = [LCMetricHistoryRow row];
		[self insertObject:row inRowsAtIndex:[rows count]];
		currentRow = row;
		currentCol = 0;
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
	/* Check for end of row */
	if ([element isEqualToString:@"row"])
	{
		currentRow = nil;
		currentCol = 0;
	}
	
	/* Check for a timestamp */
	else if ([element isEqualToString:@"t"])
	{
		[currentRow setTimeStamp:[NSDate dateWithTimeIntervalSince1970:[curXMLString floatValue]]];
	}
	
	/* Check for a value */
	else if ([element isEqualToString:@"v"])
	{
		float val;
		if ([curXMLString isEqualToString:@"NaN"]) val = NAN;
		else val = [curXMLString floatValue];
		
		switch (currentCol)
		{
			case 0:
				[currentRow setMinimum:val];
				if (!isnan(val) && isnan([self minimum])) [self setMinimum:val];
				else if (val < [self minimum]) [self setMinimum:val];
				break;
			case 1:
				[currentRow setAverage:val];
				break;
			case 2:
				[currentRow setMaximum:val];
				if (!isnan(val) && isnan([self maximum])) [self setMaximum:val];
				else if (val > [self maximum]) [self setMaximum:val];
				break;
		}
		currentCol++;
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
	/* Calculate summary average */
	int row_count = 0;
	float aggregate = 0.0f;
	LCMetricHistoryRow *row;
	for (row in rows)
	{
		if (!isnan([row average]))
		{
			aggregate += [row average];
			row_count++;
		}		
	}
	if (row_count > 0)
	{
		[self setAverage:(aggregate / (float) row_count)];
	}
	else
	{
		[self setAverage:NAN];
	}
	
	/* Calculate 95th Percentile */
	[self calculate95thPercentile];
	
	/* Set flag */
	[self setRefreshInProgress:NO];
	
	/* Free request */
	[sender release];
	refreshXMLRequest = nil;
}

#pragma mark "95th Percentile"

- (void) calculate95thPercentile
{
	/* Loops through all rows and calculate the min/avg/max 95th Percentile result */
	NSMutableArray *sortDescriptors = [NSMutableArray array];
	NSSortDescriptor *sortDesc;
	int discardCount;
	
	/* Sort by min, discard top 5%, set min */
	sortDesc = [[[NSSortDescriptor alloc] initWithKey:@"minimum" ascending:NO] autorelease];
	[sortDescriptors addObject:sortDesc];
	NSMutableArray *minimumRows;
	if (discardNanFor95th)
	{
		minimumRows = [NSMutableArray array];
		LCMetricHistoryRow *row;
		for (row in rows)
		{
			if (!isnan([row minimum]))
			{ [minimumRows addObject:row]; }
		}
	}
	else
	{ minimumRows = [NSMutableArray arrayWithArray:rows]; }
	[minimumRows sortUsingDescriptors:sortDescriptors];
	discardCount = (int) (((float) [minimumRows count]) * 0.05);
	[minimumRows removeObjectsInRange:NSMakeRange(0,discardCount)];
	if ([minimumRows count] > 0)
	{ [self setMin95thPercentile:[(LCMetricHistoryRow *)[minimumRows objectAtIndex:0] minimum]]; }
	else
	{ [self setMin95thPercentile:NAN]; }
	
	/* Sort by avg, discard top 5%, set avg */
	[sortDescriptors removeAllObjects];
	sortDesc = [[[NSSortDescriptor alloc] initWithKey:@"average" ascending:NO] autorelease];
	[sortDescriptors addObject:sortDesc];
	NSMutableArray *averageRows;
	if (discardNanFor95th)
	{
		averageRows = [NSMutableArray array];
		LCMetricHistoryRow *row;
		for (row in rows)
		{
			if (!isnan([row average]))
			{ [averageRows addObject:row]; }
		}
	}
	else
	{ averageRows = [NSMutableArray arrayWithArray:rows]; }	
	[averageRows sortUsingDescriptors:sortDescriptors];
	discardCount = (int) (((float) [averageRows count]) * 0.05);
	[averageRows removeObjectsInRange:NSMakeRange(0,discardCount)];
	if ([averageRows count] > 0)
	{ [self setAvg95thPercentile:[(LCMetricHistoryRow *)[averageRows objectAtIndex:0] average]]; }
	else
	{ [self setAvg95thPercentile:NAN]; }

	/* Sort by max, discard top 5%, set max */
	[sortDescriptors removeAllObjects];
	sortDesc = [[[NSSortDescriptor alloc] initWithKey:@"maximum" ascending:NO] autorelease];
	[sortDescriptors addObject:sortDesc];
	NSMutableArray *maximumRows;
	if (discardNanFor95th)
	{
		maximumRows = [NSMutableArray array];
		LCMetricHistoryRow *row;
		for (row in rows)
		{
			if (!isnan([row maximum]))
			{ [maximumRows addObject:row]; }
		}
	}
	else
	{ maximumRows = [NSMutableArray arrayWithArray:rows]; }	
	[maximumRows sortUsingDescriptors:sortDescriptors];
	discardCount = (int) (((float) [maximumRows count]) * 0.05);
	[maximumRows removeObjectsInRange:NSMakeRange(0,discardCount)];
	if ([maximumRows count] > 0)
	{ [self setMax95thPercentile:[(LCMetricHistoryRow *)[maximumRows objectAtIndex:0] maximum]]; }
	else
	{ [self setMax95thPercentile:NAN]; }
}

- (float) min95thPercentile
{
	return min95thPercentile;
}
- (void) setMin95thPercentile:(float)value
{ 
	[self willChangeValueForKey:@"min95thPercentileNumber"];
	min95thPercentile = value; 
	[self didChangeValueForKey:@"min95thPercentileNumber"];
}
- (NSNumber *) min95thPercentileNumber
{ return [NSNumber numberWithFloat:min95thPercentile]; }

- (float) avg95thPercentile
{
	return avg95thPercentile;
}
- (void) setAvg95thPercentile:(float)value
{ 
	[self willChangeValueForKey:@"avg95thPercentileNumber"];
	avg95thPercentile = value; 
	[self didChangeValueForKey:@"avg95thPercentileNumber"];
}
- (NSNumber *) avg95thPercentileNumber
{ return [NSNumber numberWithFloat:avg95thPercentile]; }

- (float) max95thPercentile
{
	return max95thPercentile;
}
- (void) setMax95thPercentile:(float)value
{ 
	[self willChangeValueForKey:@"max95thPercentileNumber"];
	max95thPercentile = value; 
	[self didChangeValueForKey:@"max95thPercentileNumber"];
}
- (NSNumber *) max95thPercentileNumber
{ return [NSNumber numberWithFloat:max95thPercentile]; }

- (BOOL) discardNanFor95th
{ return discardNanFor95th; }

- (void) setDiscardNanFor95th:(BOOL)value
{
	discardNanFor95th = value; 
	[self calculate95thPercentile];
}

#pragma mark "Accessor Methods"

@synthesize metric;
@synthesize refreshInProgress;

@synthesize referenceDate;
- (void) setReferenceDate:(NSDate *)date
{ 
	if (referenceDate) [referenceDate release];
	referenceDate = [date retain];	
	[self refresh:XMLREQ_PRIO_HIGH];
}

@synthesize graphPeriod;
- (void) setGraphPeriod:(int)flag
{ 
	graphPeriod = flag; 
	[self refresh:XMLREQ_PRIO_HIGH];
}

@synthesize rows;
- (void) insertObject:(LCMetricHistoryRow *)row inRowsAtIndex:(unsigned int)index
{
	[rows insertObject:row atIndex:index];
}
- (void) removeObjectFromRowsAtIndex:(unsigned int)index
{
	[rows removeObjectAtIndex:index];
}

@synthesize minimum;
- (void) setMinimum:(float)value
{ 
	minimum = value; 
	self.minimumNumber = [NSNumber numberWithFloat:value];
	if (isnan(value) == 0)
	{ self.minimumString = [NSString stringWithFormat:@"%@%@", [NSString stringForValue:value], metric.units ? metric.units : @""]; }
	else 
	{ self.minimumString = nil; }
}
@synthesize minimumNumber;
@synthesize minimumString;

@synthesize average;
- (void) setAverage:(float)value
{ 
	average = value; 
	self.averageNumber = [NSNumber numberWithFloat:value];
	if (isnan(value) == 0)
	{ self.averageString = [NSString stringWithFormat:@"%@%@", [NSString stringForValue:value], metric.units ? metric.units : @""]; }
	else 
	{ self.averageString = nil; }
}
@synthesize averageNumber;
@synthesize averageString;

@synthesize maximum;
- (void) setMaximum:(float)value
{ 
	maximum = value; 
	self.maximumNumber = [NSNumber numberWithFloat:value];
	if (isnan(value) == 0)
	{ self.maximumString = [NSString stringWithFormat:@"%@%@", [NSString stringForValue:value], metric.units ? metric.units : @""]; }
	else 
	{ self.maximumString = nil; }
}
@synthesize maximumNumber;
@synthesize maximumString;

@end
