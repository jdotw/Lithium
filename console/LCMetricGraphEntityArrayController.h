//
//  LCMetricGraphEntityArrayController.h
//  Lithium Console
//
//  Created by James Wilson on 3/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntityArrayController.h"
#import "LCMetricGraphController.h"

@interface LCMetricGraphEntityArrayController : LCEntityArrayController 
{
	IBOutlet NSTableColumn *colourColumn;
	IBOutlet LCMetricGraphController *graphController;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex;
- (int)numberOfRowsInTableView:(NSTableView *)aTableView;
- (void) insertObject:(id)object atArrangedObjectIndex:(unsigned int)index;
- (void) removeObjectsAtArrangedObjectIndexes:(NSIndexSet *)indexes;

@property (nonatomic,retain) NSTableColumn *colourColumn;
@property (nonatomic, assign) LCMetricGraphController *graphController;
@end
