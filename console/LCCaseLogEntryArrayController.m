//
//  LCCaseLogEntryArrayController.m
//  Lithium Console
//
//  Created by James Wilson on 27/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCCaseLogEntryArrayController.h"

#import "LCCaseLogEntry.h"

@implementation LCCaseLogEntryArrayController

#pragma mark "TableView Methods"

- (float)tableView:(NSTableView *)tableView heightOfRow:(int)row
{
	/* Calculate row height */
	NSTableColumn *col = [[tableView tableColumns] objectAtIndex:0];
	float width = [col width];
	NSRect rect = NSMakeRect(0, 0, width, 5000.0);
	NSCell *cell = [col dataCellForRow:row]; 
	[cell setWraps:YES];
	LCCaseLogEntry *log = [[self arrangedObjects] objectAtIndex:row];
//	LCCaseLogEntry *log =[[[cas logEntryList] logEntries] objectAtIndex:row];
	NSString *content = [log displayString];
	[cell setObjectValue:content];
	float height = [cell cellSizeForBounds:rect].height + 5;
	if (height <= 0) height = 16.0;
	return height;
}


@end
