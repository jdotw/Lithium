//
//  LCMetricGraphController.m
//  Lithium Console
//
//  Created by James Wilson on 28/02/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricGraphController.h"
#import "LCMetric.h"
#import "LCMetricHistory.h"
#import "LCEntityDescriptor.h"
#import "LCError.h"
#import "LCDevice.h"
#import "LCMetricGraphMetricItem.h"

@implementation LCMetricGraphController

#pragma mark "Initialisation"

- (id) init
{
	[super init];
	
	/* Create metrics array */
	metricItems = [[NSMutableArray array] retain];
	
	/* Config */
	self.undoEnabled = YES;
	self.graphPeriod = 1;
	self.referenceDate = [NSDate date];

	return self;
}

- (void) dealloc 
{
	if (refreshXMLRequest) 
	{
		[refreshXMLRequest cancel];
		[refreshXMLRequest release];
	}
	if (baselineXMLRequest) 
	{
		[baselineXMLRequest cancel];
		[baselineXMLRequest release];
	}
	if (urlConn) 
	{ 
		[urlConn cancel]; 
		[urlConn release]; 
	}
	if (baselineConn) 
	{ 
		[baselineConn cancel]; 
		[baselineConn release]; 
	}
	if (activity)
	{
		[activity setStatus:@"Request Cancelled"];
		[activity invalidate];
		[activity release];	
	}
	[metricItems release];
	[referenceDate release];
	[referenceDateString release];
	[xmlImagePaths release];
	[windowTitle release];
	[imageURLString release];
	[graphPDFRep release];
	[baselinePDFRep release];
	[lastRefresh release];
	[receivedData release];
	[baselineReceivedData release];
	[super dealloc];
}

#pragma mark "Metric Selection"

+ (NSArray *) graphableMetricsForEntities:(NSArray *)entityArray
{
	/* Returns an array of graphable metrics for the given entites */
	NSMutableArray *metricArray = [NSMutableArray array];
	
	/* Loop through each entity */
	for (LCEntity *entity in entityArray)
	{
		/* Check type */
		if (entity.type < 4)
		{
			/* Entity is higher than a container, ignore it */
			continue;
		}
		
		/* Get the key path */
		NSString *keyPath = nil;
		switch (entity.type)
		{
			case 4:
				/* Container */
				keyPath = @"children.@unionOfArrays.children";
				break;
			case 5:
				/* Object */
				keyPath = @"children";
				break;
			case 6:
				/* Metric */
				keyPath = nil;
				break;
			case 7:
				/* Trigger, use parent */
				entity = [entity parent];
				keyPath = nil;
				break;
			default:
				/* Unknown */
				continue;
		}
		
		/* Get candidate metric(s) */
		NSArray *candidateArray;
		if (keyPath)
		{
			/* Use KVC to get our metrics */
			candidateArray = [entity valueForKeyPath:keyPath];
		}
		else
		{
			candidateArray = [NSArray arrayWithObject:entity];
		}
		
		/* Process the candidates
		 * First attempt to find entities with a 
		 * '%' unit string with RRD recording enabled
		 */
		NSPredicate *metricPredicate;
		metricPredicate = [NSPredicate predicateWithFormat:@"(units == %@ AND recordEnabled == 1)", @"%"];
		NSArray *filteredArray = [candidateArray filteredArrayUsingPredicate:metricPredicate];
		if (!filteredArray || [filteredArray count] < 1)
		{
			/* If there are no metrics with a 
			 * '%' unit string, just use anything
			 * that has RRD recording enabled 
			 */
			metricPredicate = [NSPredicate predicateWithFormat:@"recordEnabled == 1"];
			filteredArray = [candidateArray filteredArrayUsingPredicate:metricPredicate];
			if (!filteredArray || [filteredArray count] < 1)
			{
				/* Last resort, attempt to use any metrics 
				 * which have not been refreshed yet,
				 * and hence may not have their unitString and 
				 * recordMethods set accurately yet 
				 * (e.g built from entity descriptor)
				 */
				metricPredicate = [NSPredicate predicateWithFormat:@"recordMethod == 0"];
				filteredArray = [candidateArray filteredArrayUsingPredicate:metricPredicate];			
			}
			
		}
		[metricArray addObjectsFromArray:filteredArray];
	}
	
	return metricArray;
}

#pragma mark "Graph Generation"

- (void) refreshGraph:(int)priority
{
	/* Check to see if its too early */
	int lowestRefreshInterval = 0;
	for (LCMetricGraphMetricItem *item in metricItems)
	{
		if (lowestRefreshInterval == 0 || [(LCDevice *)[item.metric device] refreshInterval] < lowestRefreshInterval)
		{ lowestRefreshInterval = [[item.metric device] refreshInterval]; }
	}
	if (graphPDFRep && lastRefresh && [[NSDate date] timeIntervalSinceDate:lastRefresh] < (float) lowestRefreshInterval)
	{ 
		return; 
	}

	/* Check count of metrics */
	if ([metricItems count] < 1) return;
	
	/* Check if refresh is in progress */
	if (refreshInProgress)
	{ 
		/* Refresh is in progress */
		[self cancelRefresh];
	}
	
	/* Prepare activity description */
	NSMutableString *activityDesc = [NSMutableString stringWithString:@"Graphing"];
	
	/* Create XML Doc */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"graph_specification"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	int metric_index = 0;
	for (LCMetricGraphMetricItem *item in metricItems)
	{
		/* Add metric to XML */
		LCEntityDescriptor *entDesc = [LCEntityDescriptor descriptorForEntity:item.metric];
		NSXMLElement *metricNode = (NSXMLElement *) [entDesc xmlNode];
		[metricNode addChild:[NSXMLNode elementWithName:@"units" stringValue:item.metric.units]];		
		[rootNode addChild:metricNode];
		
		/* Add colours to XML */
		NSString *maxColour = @"FFFFFF";
		NSString *avgColour = @"AAAAAA";
		NSString *minColour = @"555555";
		switch (metric_index)
		{
			case 0:		/* Blue */
				minColour = @"0e1869";
				avgColour = @"031bc5";
				maxColour = @"0000ff";
				break;
			case 1:		/* Green */
				minColour = @"006b00";
				avgColour = @"009c00";
				maxColour = @"00ed00";
				break;
			case 2:		/* Red */
				minColour = @"6b0000";
				avgColour = @"9c0000";
				maxColour = @"ed0000";
				break;
			case 3:		/* Purple */
				minColour = @"620585";
				avgColour = @"8e0abf";
				maxColour = @"ba00ff";
				break;
			case 4:		/* Yellow */
				minColour = @"6f7304";
				avgColour = @"a7ad04";
				maxColour = @"f6ff00";
				break;
			case 5:		/* Cyan */
				minColour = @"038a8a";
				avgColour = @"01bcbc";
				maxColour = @"00ffff";
				break;
			case 6:		/* Orange */
				minColour = @"8a5f03";
				avgColour = @"c38501";
				maxColour = @"ffae00";
				break;
			case 7:		/* Pink */
				minColour = @"790352";
				avgColour = @"a51c78";
				maxColour = @"f80baa";
				break;				
		}
		[metricNode addChild:[NSXMLNode elementWithName:@"min_colour" stringValue:minColour]];
		[metricNode addChild:[NSXMLNode elementWithName:@"avg_colour" stringValue:avgColour]];
		[metricNode addChild:[NSXMLNode elementWithName:@"max_colour" stringValue:maxColour]];
		
		/* Update activity description */
		[activityDesc appendFormat:@" %@", [[item.metric entityAddress] addressString]];
	
		metric_index++;
	}

	/* Set reference date and graph period */
	if (graphPeriod > 1 && referenceDate)
	{
		NSString *str = [NSString stringWithFormat:@"%f", [referenceDate timeIntervalSince1970]];
		[rootNode addChild:[NSXMLNode elementWithName:@"ref_sec" stringValue:str]];
		str = [NSString stringWithFormat:@"%i", graphPeriod];
		[rootNode addChild:[NSXMLNode elementWithName:@"period" stringValue:str]];	
 	}
	else
	{
		NSString *str = [NSString stringWithFormat:@"%f", [[NSDate date] timeIntervalSince1970]];
		[rootNode addChild:[NSXMLNode elementWithName:@"ref_sec" stringValue:str]];
		str = [NSString stringWithFormat:@"%i", graphPeriod];
		[rootNode addChild:[NSXMLNode elementWithName:@"period" stringValue:str]];	
	}

	/* Create activity */
	activity = [[LCActivity activityWithDescription:activityDesc
										forCustomer:[[[metricItems objectAtIndex:0] metric] customer] 
										   delegate:self
									   stopSelector:nil] retain];
	[activity setStatus:@"Preparing to refresh"];
	[activity setPriority:[NSNumber numberWithInt:XMLREQ_PRIO_HIGH]];
	
	/* Create XML Request */
	LCMetric *metric = [[metricItems objectAtIndex:0] metric];
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:[metric customer]
												  resource:[[metric device] resourceAddress]
													entity:[metric entityAddress] 
												   xmlname:@"xmlgraph_render" 
													refsec:0 
													xmlout:xmlDoc] retain];
	[refreshXMLRequest setDelegate:self];
 	[refreshXMLRequest setXMLDelegate:self];
	[refreshXMLRequest setPriority:priority];
	
	/* Perform XML request */
	[refreshXMLRequest performAsyncRequest];
	[self setRefreshInProgress:YES];
	
	/* Check if a baseline is also requested */
	if (baselineType > 0)
	{
		/* Calculate Date */
		NSCalendar *calendar = [[[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar] autorelease];
		NSDateComponents *delta = [[NSDateComponents alloc] init];
		switch (baselineType)
		{
			case 1:
				/* Last Week */
				[delta setWeek:-1];
				break;
			case 2:
				/* Last Month */
				[delta setMonth:-1];
			case 3:
				/* Last Quarter */
				[delta setMonth:-3];
				break;
			case 4:
				/* Last Year */
				[delta setYear:-1];
				break;
		}
		NSDate *baselineDate = [calendar dateByAddingComponents:delta toDate:referenceDate options:0];
		[delta release];
		
		/* Revise XML */
		NSXMLDocument *baselineDoc = [[xmlDoc copy] autorelease];
		NSArray *elements = [[baselineDoc rootElement] elementsForName:@"ref_sec"];
		for (NSXMLElement *element in elements)
		{
			[element setStringValue:[NSString stringWithFormat:@"%.0f", [baselineDate timeIntervalSince1970]]];
		}
		
		/* Perform baseline XML request */
		baselineXMLRequest = [[LCXMLRequest requestWithCriteria:[metric customer]
													   resource:[[metric device] resourceAddress]
														 entity:[metric entityAddress] 
													   xmlname:@"xmlgraph_render" 
														 refsec:0 
														 xmlout:baselineDoc] retain];
		[baselineXMLRequest setDelegate:self];
		[baselineXMLRequest setXMLDelegate:self];
		[baselineXMLRequest setPriority:priority];
		[baselineXMLRequest performAsyncRequest];
	}
	
	/* Refresh history */
	if (self.getMinMaxAvgValues)
	{
		for (LCMetricGraphMetricItem *item in metricItems)
		{
			[item.history refresh:XMLREQ_PRIO_HIGH];
		}
	}
}

- (void) cancelRefresh
{
	if (refreshXMLRequest)
	{ 
		[refreshXMLRequest cancel]; 
		[refreshXMLRequest release];
		refreshXMLRequest = nil; 
	}
	if (baselineXMLRequest)
	{
		[baselineXMLRequest cancel]; 
		[baselineXMLRequest release];
		baselineXMLRequest = nil; 
	}
	if (xmlImagePaths)
	{ 
		[xmlImagePaths release]; 
		xmlImagePaths = nil; 
	}
	if (urlConn)
	{
		[urlConn cancel];
		[urlConn release];
		urlConn = nil;
	}
	if (activity)
	{
		[activity setStatus:@"Request Cancelled"];
		[activity invalidate];
		[activity release];
		activity = nil;	
	}
		
	[self setRefreshInProgress:NO];
}

- (void) XMLRequestPreParse:(id)sender
{
	/* Create image paths */
	if (xmlImagePaths) [xmlImagePaths release];
	xmlImagePaths = [[NSMutableArray array] retain];
	
	if (sender == refreshXMLRequest)
	{
		/* Update activity */
		[activity setStatus:@"Parsing XML"];
		parsingBaseline = NO;
	}
	else if (sender == baselineXMLRequest)
	{ 
		parsingBaseline = YES; 
	}
}
	

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	if (curXMLString)
	{ 
		[curXMLString release]; 
		curXMLString = nil;
	}
	curXMLString = [[NSMutableString string] retain];
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	[curXMLString appendString:string];
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Check for imagefile */
	if ([element isEqualToString:@"imagefile"])
	{ [xmlImagePaths addObject:curXMLString]; }
	
	/* Check for graphv output */
	if ([element isEqualToString:@"output"])
	{
		/* Parse output */
		NSString *graphInfo = [curXMLString copy]; 
		NSArray *pairs = [graphInfo componentsSeparatedByString:@","];
		for (NSString *pair in pairs)
		{
			NSArray *words = [pair componentsSeparatedByString:@" "];
			if ([[words objectAtIndex:0] isEqualToString:@"value_min"])
			{ 
				float min = [[words objectAtIndex:2] floatValue];
				if (parsingBaseline) self.baselineMinValue = min;
				else self.minValue = min;
			}
			else if ([[words objectAtIndex:0] isEqualToString:@"value_max"])
			{
				float max = [[words objectAtIndex:2] floatValue];
				if (parsingBaseline) self.baselineMaxValue = max;
				else self.maxValue = max;
			}
		}		
		[graphInfo release];
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
	/* Check success and retrieve images */
	if ([sender success] && [xmlImagePaths count] > 0 && [metricItems count] > 0)
	{
		/* Act upon received image */
		
		/* Find metric */
		LCEntity *met = [[metricItems objectAtIndex:0] metric];
		
		/* Use NSURLConnection to download the image (async) */
		NSString *urlString = [NSString stringWithFormat:@"%@/image_cache/%@", [[met customer] url], [xmlImagePaths objectAtIndex:0]];
		if (sender == refreshXMLRequest) 
		{
			[self setImageURLString:urlString];
		}
		NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															   cachePolicy:NSURLRequestReloadIgnoringCacheData
															  timeoutInterval:[[NSUserDefaults standardUserDefaults] floatForKey:@"graphHTTPTimeoutSec"]];
		
		NSURLConnection *conn = [NSURLConnection connectionWithRequest:urlRequest delegate:self];
		if (sender == refreshXMLRequest)
		{
			/* This is the main graph refresh */
			if (conn)
			{
				/* Set up data */
				receivedData = [[NSMutableData data] retain];
				[activity setStatus:@"Downloading Graph"];
				urlConn = [conn retain];
			}
			else
			{
				/* Error occurred */
				/* Set refresh flag */
				[self setRefreshInProgress:NO];
				[activity setStatus:@"Request Finished"];
				[activity invalidate];
				[activity release];
				activity = nil;
			}
		}
		else if (sender == baselineXMLRequest)
		{
			/* This is the baseline graph refresh */
			if (conn)
			{
				/* Set up data */
				baselineReceivedData = [[NSMutableData data] retain];
				baselineConn = [conn retain];
			}
		}
	}
	else if (sender == refreshXMLRequest)
	{
		/* No images received */
		/* Set refresh flag */
		[self setRefreshInProgress:NO];
		[activity setStatus:@"Request Finished"];
		[activity invalidate];
		[activity release];
		activity = nil;
	}
	
	/* Free paths */
	[xmlImagePaths release];
	xmlImagePaths = nil;
	
	/* Free request */
	if (sender == refreshXMLRequest)
	{
		[refreshXMLRequest release];
		refreshXMLRequest = nil; 
	}
	else if (sender == baselineXMLRequest)
	{
		[baselineXMLRequest release];
		baselineXMLRequest = nil;
	}
}

#pragma mark "NSURLConnection Delegates (Image Download)"

- (void) connection:(NSURLConnection *)connection 
 didReceiveResponse:(NSURLResponse *)response
{
	/* Received non-data response */
	if (connection == urlConn)
	{ 
		[receivedData setLength:0]; 
		[activity setStatus:@"Received non-data response"];
	}
	else if (connection == baselineConn)
	{ [baselineReceivedData setLength:0]; }
}

-(void)	connection:(NSURLConnection *) connection
	didReceiveData:(NSData *) data
{
	/* Received the actual data */
	if (connection == urlConn)
	{ 
		[receivedData appendData:data]; 
		[activity setStatus:@"Receiving data"];
	}
	else if (connection == baselineConn)
	{ [baselineReceivedData appendData:data]; }
}

-(NSURLRequest *)	connection:(NSURLConnection *)connection
			   willSendRequest:(NSURLRequest *)request
			  redirectResponse:(NSURLResponse *)redirectResponse
{
	/* Allow redirects */
	if (connection == urlConn)
	{ [activity setStatus:@"Received re-direction"]; }
    return request;
}

-(void) connectionDidFinishLoading:(NSURLConnection *) connection
{
	/* Create the image rep */
	NSData *data = nil;
	if (connection == urlConn)
	{
		/* Set data and release */
		data = [receivedData copy];
		[receivedData release];
		receivedData = nil;

		/* Set activity */
		[activity setStatus:@"Request Finished"];
		[activity invalidate];
		[activity release];
		activity = nil;
		
		/* Set refrsh flag */
		[self setRefreshInProgress:NO];
	}
	else if (connection == baselineConn)
	{
		/* Set data and release */
		data = [baselineReceivedData copy];
		[baselineReceivedData release];
		baselineReceivedData = nil;
	}
	
	/* Check Data for PDF Header and create image rep */
	NSPDFImageRep *imageRep = nil;
	if (data && [data length] > 4)
	{
		char buf[5];
		[data getBytes:buf length:4];
		buf[4] = '\0';
		if (strcmp(buf, "%PDF") == 0)
		{
			imageRep = [NSPDFImageRep imageRepWithData:data];
		}
	}
	if (!imageRep)
	{
		NSString *dataString = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
		LCEntity *metric = nil;
		if ([metricItems count] > 0) metric = [[metricItems objectAtIndex:0] metric];
		[LCError logError:[NSString stringWithFormat:@"Failed to retrieve metric graph"]
			  forCustomer:[metric customer]
				 fullText:[NSString stringWithFormat:@"Server Returned: %@", dataString]];
	}
	else
	{
		/* Successful refresh */
		self.lastRefresh = [NSDate date];
	}
	
	if (connection == urlConn)
	{ 
		[self setGraphPDFRep:imageRep]; 
		[urlConn release];
		urlConn = nil;
	}
	else if (connection == baselineConn)
	{ 
		[self setBaselinePDFRep:imageRep]; 
		[baselineConn release];
		baselineConn = nil;
	}
	
	[data release];	
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	/* Log Error */
	LCEntity *metric = nil;
	if ([metricItems count] > 0) metric = [[metricItems objectAtIndex:0] metric];
	[LCError logError:[NSString stringWithFormat:@"Failed to retrieve metric graph"]
		  forCustomer:[metric customer]
			 fullText:[NSString stringWithFormat:@"Error: %@", [[error localizedDescription] capitalizedString]]];

	/* Release data */
	if (connection == urlConn)
	{ 
		[receivedData release];
		receivedData = nil;
		[urlConn release];
		urlConn = nil;

		[activity setStatus:@"Request Failed"];
		[activity invalidate];
		[activity release];
		activity = nil;
	}
	else if (connection == baselineConn)
	{
		[baselineReceivedData release];
		baselineReceivedData = nil; 
		[baselineConn release];
		baselineConn = nil;
	}
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
}

#pragma mark "Context Menu Methods"

- (IBAction) copyURLToClipBoardClicked:(id)sender
{
	if (imageURLString)
	{
		NSPasteboard *pb = [NSPasteboard generalPasteboard];
		NSArray *types = [NSArray arrayWithObject:NSStringPboardType];
		[pb declareTypes:types owner:self];
		[pb setString:imageURLString forType:NSStringPboardType];
	}
}

- (IBAction) openURLInWebBrowserClicked:(id)sender
{
	if (imageURLString)
	{
		NSURL *url = [NSURL URLWithString:imageURLString];
		[[NSWorkspace sharedWorkspace] openURL:url];
	}
}

- (IBAction) saveImageAsClicked:(id)sender
{
	if (graphPDFRep)
	{
		NSSavePanel * savePanel = [NSSavePanel savePanel];
		[savePanel setCanCreateDirectories:YES];
		[savePanel setRequiredFileType:@"pdf"];
		[savePanel setNameFieldLabel:@"Save As:"];
		[savePanel setExtensionHidden:NO];
		if([savePanel runModalForDirectory:NSHomeDirectory() file:@"ConsoleGraph.pdf"] == NSFileHandlingPanelOKButton )
		{
			[[graphPDFRep PDFRepresentation] writeToFile:[savePanel filename] atomically:YES];
		}
	}
}

#pragma mark "Graph Display Methods"

- (void) blankGraph
{ 
	self.graphPDFRep = nil;
}

#pragma mark "Metric Manipulation"

@synthesize metricItems;

- (void) insertObject:(LCMetricGraphMetricItem *)item inMetricItemsAtIndex:(unsigned int)index
{
	if (self.undoEnabled) 
	{ [[[undoResponder undoManager] prepareWithInvocationTarget:self] removeObjectFromMetricItemsAtIndex:index]; }
	[metricItems insertObject:item atIndex:index];
	[self updateTitle];
	self.lastRefresh = nil;
	[self refreshGraph:XMLREQ_PRIO_HIGH];
	if (self.undoEnabled && ![[undoResponder undoManager] isUndoing])
	{ [[undoResponder undoManager] setActionName:@"Add Metric"]; }
}

- (void) removeObjectFromMetricItemsAtIndex:(unsigned int)index
{
	if (self.undoEnabled)
	{ [[[undoResponder undoManager] prepareWithInvocationTarget:self] insertObject:[metricItems objectAtIndex:index] inMetricItemsAtIndex:index]; }
	[metricItems removeObjectAtIndex:index];
	[self updateTitle];
	self.lastRefresh = nil;
	[self refreshGraph:XMLREQ_PRIO_HIGH];
	if (self.undoEnabled && ![[undoResponder undoManager] isUndoing])
	{ [[undoResponder undoManager] setActionName:@"Remove Metric"]; }
}

- (void) removeAllMetricItems;
{
	while (metricItems.count > 0)
	{
		[self removeObjectFromMetricItemsAtIndex:0];
	}
}

- (void) addMetric:(LCMetric *)metric
{
	LCMetricGraphMetricItem *item = [LCMetricGraphMetricItem new];
	item.metric = metric;
	[self insertObject:item inMetricItemsAtIndex:metricItems.count];
	[item autorelease];
}

- (void) addMetricsFromArray:(NSArray *)array
{
	for (LCMetric *metric in array)
	{
		LCMetricGraphMetricItem *item = [LCMetricGraphMetricItem new];
		item.metric = metric;
		[self insertObject:item inMetricItemsAtIndex:metricItems.count];
		[item autorelease];
	}
}

#pragma mark "Window Title"

- (void) updateTitle
{
	if ([metricItems count] > 0)
	{
		LCEntity *firstMetric = [[metricItems objectAtIndex:0] metric];
		if ([metricItems count] == 1)
		{
			[self setWindowTitle:[NSString stringWithFormat:@"%@:%@:%@:%@", 
				[[firstMetric device] displayString], [[firstMetric container] displayString], [[firstMetric object] displayString], [firstMetric displayString]]];
		}
		else
		{
			[self setWindowTitle:[NSString stringWithFormat:@"%@:%@:%@:%@ (+%i)", 
				[[firstMetric device] displayString], [[firstMetric container] displayString], [[firstMetric object] displayString], [firstMetric displayString], ([metricItems count]-1)]];
		}			
	}
	else
	{
		[self setWindowTitle:@""];
	}
 }

@synthesize windowTitle;

#pragma mark "Timeframe Adjustment"

- (IBAction) forwardAndBackClicked:(id)sender
{
	NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
	NSDateComponents *delta = [[NSDateComponents alloc] init];
	switch (graphPeriod)
	{
		case 1:
		case 2:
			/* 48 Hours */
			if (forwardAndBackMode == 1)
			{ [delta setDay:-2]; }
			else if (forwardAndBackMode == 2)
			{ [delta setDay:+2]; }
			break;
		case 3:
			/* Week */
			if (forwardAndBackMode == 1)
			{ [delta setDay:-7]; }
			else if (forwardAndBackMode == 2)
			{ [delta setDay:+7]; }
			break;
		case 4:
			/* Month */
			if (forwardAndBackMode == 1)
			{ [delta setMonth:-1]; }
			else if (forwardAndBackMode == 2)
			{ [delta setMonth:+1]; }
			break;
		case 5:
			/* Year */
			if (forwardAndBackMode == 1)
			{ [delta setYear:-1]; }
			else if (forwardAndBackMode == 2)
			{ [delta setYear:+1]; }
			break;
	}
	self.referenceDate = [calendar dateByAddingComponents:delta toDate:self.referenceDate options:0];
	if (self.graphPeriod == 1)
	{ self.graphPeriod = 2; }
	[calendar release];
	[delta release];	
}

@synthesize forwardAndBackMode;

#pragma mark "Property Methods"

@synthesize refreshInProgress;

- (void) updateReferenceDateString
{
	NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
	[formatter setDateStyle:kCFDateFormatterShortStyle];
	if (self.graphPeriod == 2)
	{ [formatter setTimeStyle:kCFDateFormatterShortStyle]; }
	else
	{ [formatter setTimeStyle:kCFDateFormatterNoStyle]; }
	self.referenceDateString = [formatter stringFromDate:referenceDate]; 	
	[formatter release];
}

@synthesize referenceDate;
- (void) setReferenceDate:(NSDate *)date
{ 
	[[[undoResponder undoManager] prepareWithInvocationTarget:self] setReferenceDate:self.referenceDate];
	if (referenceDate) [referenceDate release];
	referenceDate = [date copy];
	for (LCMetricGraphMetricItem *item in metricItems)
	{ item.history.referenceDate = date; }
	[self updateReferenceDateString];
	self.lastRefresh = nil;
	[self refreshGraph:XMLREQ_PRIO_HIGH];
	[[undoResponder undoManager] setActionName:@"Change Reference Date"];
}
@synthesize referenceDateString;

@synthesize graphPeriod;
- (void) setGraphPeriod:(int)flag
{ 
	[[[undoResponder undoManager] prepareWithInvocationTarget:self] setGraphPeriod:self.graphPeriod];
	graphPeriod = flag; 
	[self updateReferenceDateString];
	if (graphPeriod == 1)
	{ self.userCanSelectReferenceDate = NO; }
	else
	{ self.userCanSelectReferenceDate = YES; }
	for (LCMetricGraphMetricItem *item in metricItems)
	{ item.history.graphPeriod = flag; }
	self.lastRefresh = nil;
	[self refreshGraph:XMLREQ_PRIO_HIGH];
	[[undoResponder undoManager] setActionName:@"Change Graph Period"];
}
@synthesize userCanSelectReferenceDate;

@synthesize imageURLString;
@synthesize graphPDFRep;
@synthesize baselinePDFRep;
@synthesize undoResponder;
@synthesize minValue;
@synthesize maxValue;
@synthesize baselineMinValue;
@synthesize baselineMaxValue;

@synthesize baselineType;
- (void) setBaselineType:(int)value
{
	baselineType = value;
	self.lastRefresh = nil;
	[self refreshGraph:XMLREQ_PRIO_HIGH];
}

@synthesize lastRefresh;
@synthesize undoEnabled;
@synthesize getMinMaxAvgValues;

@end
