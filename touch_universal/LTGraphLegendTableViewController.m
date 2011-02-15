//
//  LTGraphLegendTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 8/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTGraphLegendTableViewController.h"
#import "LTGraphLegendTableViewCell.h"
#import "LTEntity.h"
#import "LTGraphLegendEntityView.h"
#import "LTGraphLegendTableViewCell.h"
#import "LTGraphView.h"

@implementation LTGraphLegendTableViewController

@synthesize entities, entitiesPerRow, graphView;

#pragma mark -
#pragma mark View lifecycle

- (void) awakeFromNib
{
	entitiesPerRow = 2;
}

- (void) setEntities:(NSArray *)value
{
	[entities release];
	entities = [[value sortedArrayUsingComparator:
				^(id a, id b)
				{
					/* Sort entities based on their index in the graphViews list of entities
					 * This ensures graphed metrics are shown at the top of the list 
					 */
					NSInteger aIndex = [graphView.metrics indexOfObject:a];
					NSInteger bIndex = [graphView.metrics indexOfObject:b];
					if (aIndex > bIndex) return (NSComparisonResult)NSOrderedDescending;
					else if (aIndex < bIndex) return (NSComparisonResult)NSOrderedAscending;
					else return (NSComparisonResult)NSOrderedSame;
				}] retain];
	
	[self.tableView reloadData];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Override to allow orientations other than the default portrait orientation.
    return YES;
}

#pragma mark -
#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    // Return the number of sections.
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
    // Return the number of rows in the section.
	int rows = entities.count / entitiesPerRow;
	if (entities.count % entitiesPerRow) rows++;
	return rows;
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
    static NSString *CellIdentifier = @"Cell";
    
    LTGraphLegendTableViewCell *cell = (LTGraphLegendTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
        cell = [[[LTGraphLegendTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell..
	int entitiesInThisRow = entitiesPerRow;
	if (((entitiesPerRow * indexPath.row) + entitiesPerRow) >= entities.count)
	{ entitiesInThisRow = entities.count - (entitiesPerRow * indexPath.row); }
	NSArray *rowMetrics = [entities subarrayWithRange:NSMakeRange(entitiesPerRow * indexPath.row, entitiesInThisRow)];
	cell.entities = rowMetrics;
	cell.selectionStyle = UITableViewCellSelectionStyleNone;
	for (LTEntity *entity in rowMetrics)
	{
		UIColor *swatchColor = [UIColor whiteColor];
		
		switch ([graphView.metrics indexOfObject:entity])
		{
			case 0:		/* Blue */
				swatchColor = [UIColor colorWithRed:(CGFloat)0x03/0xff green:(CGFloat)0x1b/0xff blue:(CGFloat)0xc5/0xff alpha:1.0];
				break;
			case 1:		/* Green */
				swatchColor = [UIColor colorWithRed:(CGFloat)0x00/0xff green:(CGFloat)0x9c/0xff blue:(CGFloat)0x00/0xff alpha:1.0];
				break;
			case 2:		/* Red */
				swatchColor = [UIColor colorWithRed:(CGFloat)0x9c/0xff green:(CGFloat)0x00/0xff blue:(CGFloat)0x00/0xff alpha:1.0];
				break;
			case 3:		/* Purple */
				swatchColor = [UIColor colorWithRed:(CGFloat)0x8e/0xff green:(CGFloat)0x0a/0xff blue:(CGFloat)0xbf/0xff alpha:1.0];
				break;
			case 4:		/* Yellow */
				swatchColor = [UIColor colorWithRed:(CGFloat)0xa7/0xff green:(CGFloat)0xad/0xff blue:(CGFloat)0x04/0xff alpha:1.0];
				break;
			case 5:		/* Cyan */
				swatchColor = [UIColor colorWithRed:(CGFloat)0x01/0xff green:(CGFloat)0xbc/0xff blue:(CGFloat)0xbc/0xff alpha:1.0];
				break;
			case 6:		/* Orange */
				swatchColor = [UIColor colorWithRed:(CGFloat)0xc3/0xff green:(CGFloat)0x85/0xff blue:(CGFloat)0x01/0xff alpha:1.0];
				break;
			case 7:		/* Pink */
				swatchColor = [UIColor colorWithRed:(CGFloat)0xa5/0xff green:(CGFloat)0x1c/0xff blue:(CGFloat)0x78/0xff alpha:1.0];
				break;		
			case NSNotFound:
				swatchColor = [UIColor clearColor];
				break;
		}				
		[cell setSwatchColor:swatchColor forEntityAtIndex:[rowMetrics indexOfObject:entity]];
	}
    
    return cell;
}

#pragma mark -
#pragma mark Table view delegate

- (NSIndexPath *) tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	/* Do not allow any selection */
	return nil;
}

#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Relinquish ownership any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    // Relinquish ownership of anything that can be recreated in viewDidLoad or on demand.
    // For example: self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}

#pragma mark -
#pragma mark Entity Highlight

- (UIPopoverController *) highlightEntity:(LTEntity *)entity
{
	/* Scrolls the tableview to the row, then shows a pop-over
	 * for the entity with the entity drilled down
	 */
	
	/* First we need to find the parent of the specified entity
	 * that is a peer to the level of the entities being
	 * used in the legend. For example, containers may be 
	 * used in the legend but the entity may be a metric.
	 *
	 * We assume here that all entities in 'self.entities' are of the same type
	 */
	
	if (entities.count == 0) return nil;
	LTEntity *legendPeer = [entities objectAtIndex:0];
	if (!legendPeer) return nil;
	LTEntity *legendParent = [entity parentOfType:legendPeer.type];
	if (!legendParent) return nil;
	
	/* Row */
	NSIndexPath *rowIndexPath = [NSIndexPath indexPathForRow:([entities indexOfObject:legendParent] / entitiesPerRow) inSection:0];
	
	/* Scroll to row */
	[self.tableView scrollToRowAtIndexPath:rowIndexPath
						  atScrollPosition:UITableViewScrollPositionMiddle
								  animated:NO];
	
	/* Create pop-over */
	LTGraphLegendTableViewCell *cell = (LTGraphLegendTableViewCell *) [self.tableView cellForRowAtIndexPath:rowIndexPath];
	LTGraphLegendEntityView *entityView = [cell viewForEntity:legendParent];
	return [entityView presentPopoverForEntityFromRect:entityView.bounds];
}


@end


