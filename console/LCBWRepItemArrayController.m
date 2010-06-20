//
//  LCBWRepItemArrayController.m
//  Lithium Console
//
//  Created by James Wilson on 30/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBWRepItemArrayController.h"


@implementation LCBWRepItemArrayController

#pragma mark "NIB Methods"

- (void)awakeFromNib
{
	[tableView registerForDraggedTypes:[NSArray arrayWithObjects:@"LCEntityDescriptor", nil]];
	
	/* Set browser cell */
	id cell = [[NSBrowserCell alloc] init];
	[cell setFont:[NSFont fontWithDescriptor:[[cell font] fontDescriptor] size:10.0]];
	[descColumn setDataCell:cell];
	[cell release];
	
	/* Call super */
	[super awakeFromNib];
}

#pragma mark "Delegate methods"

- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	if (tableColumn == descColumn)
	{
		NSBrowserCell *browserCell = cell;
		[browserCell setImage:[NSImage imageNamed:@"Interface13px.tiff"]]; 
		[browserCell setLeaf:YES];
	}
}

@synthesize tableView;
@synthesize descColumn;
@end
