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

@implementation LTGraphLegendTableViewController

@synthesize entities, entitiesPerRow;

#pragma mark -
#pragma mark View lifecycle

- (void) awakeFromNib
{
	entitiesPerRow = 2;
}

- (void) setEntities:(NSArray *)value
{
	[entities release];
	entities = [value retain];
	
	[self.tableView reloadData];
}

/*
- (void)viewDidLoad {
    [super viewDidLoad];

    // Uncomment the following line to preserve selection between presentations.
    self.clearsSelectionOnViewWillAppear = NO;
 
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}
*/

/*
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}
*/
/*
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}
*/
/*
- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
}
*/
/*
- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
}
*/


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
	NSLog (@"%@ rows is %i", self, rows);
	return rows;
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
    static NSString *CellIdentifier = @"Cell";
    
    LTGraphLegendTableViewCell *cell = (LTGraphLegendTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
		NSLog (@"Created new cell");
        cell = [[[LTGraphLegendTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell..
	int entitiesInThisRow = entitiesPerRow;
	if (((entitiesPerRow * indexPath.row) + entitiesPerRow) >= entities.count)
	{ entitiesInThisRow = entities.count - (entitiesPerRow * indexPath.row); }
	NSArray *rowMetrics = [entities subarrayWithRange:NSMakeRange(entitiesPerRow * indexPath.row, entitiesInThisRow)];
	cell.entities = rowMetrics;
	for (LTEntity *entity in rowMetrics)
	{
		NSString *minColour;
		NSString *avgColour;
		NSString *maxColour;
		UIColor *swatchColor = [UIColor whiteColor];
		switch ([entities indexOfObject:entity])
		{
			case 0:		/* Blue */
				minColour = @"0e1869";
				avgColour = @"031bc5";
				swatchColor = [UIColor colorWithRed:(CGFloat)0x03/0xff green:(CGFloat)0x1b/0xff blue:(CGFloat)0xc5/0xff alpha:1.0];
				maxColour = @"0000ff";
				break;
			case 1:		/* Green */
				minColour = @"006b00";
				avgColour = @"009c00";
				swatchColor = [UIColor colorWithRed:(CGFloat)0x00/0xff green:(CGFloat)0x9c/0xff blue:(CGFloat)0x00/0xff alpha:1.0];
				maxColour = @"00ed00";
				break;
			case 2:		/* Red */
				minColour = @"6b0000";
				avgColour = @"9c0000";
				swatchColor = [UIColor colorWithRed:(CGFloat)0x9c/0xff green:(CGFloat)0x00/0xff blue:(CGFloat)0x00/0xff alpha:1.0];
				maxColour = @"ed0000";
				break;
			case 3:		/* Purple */
				minColour = @"620585";
				avgColour = @"8e0abf";
				swatchColor = [UIColor colorWithRed:(CGFloat)0x8e/0xff green:(CGFloat)0x0a/0xff blue:(CGFloat)0xbf/0xff alpha:1.0];
				maxColour = @"ba00ff";
				break;
			case 4:		/* Yellow */
				minColour = @"6f7304";
				avgColour = @"a7ad04";
				swatchColor = [UIColor colorWithRed:(CGFloat)0xa7/0xff green:(CGFloat)0xad/0xff blue:(CGFloat)0x04/0xff alpha:1.0];
				maxColour = @"f6ff00";
				break;
			case 5:		/* Cyan */
				minColour = @"038a8a";
				avgColour = @"01bcbc";
				swatchColor = [UIColor colorWithRed:(CGFloat)0x01/0xff green:(CGFloat)0xbc/0xff blue:(CGFloat)0xbc/0xff alpha:1.0];
				maxColour = @"00ffff";
				break;
			case 6:		/* Orange */
				minColour = @"8a5f03";
				avgColour = @"c38501";
				swatchColor = [UIColor colorWithRed:(CGFloat)0xc3/0xff green:(CGFloat)0x85/0xff blue:(CGFloat)0x01/0xff alpha:1.0];
				maxColour = @"ffae00";
				break;
			case 7:		/* Pink */
				minColour = @"790352";
				avgColour = @"a51c78";
				swatchColor = [UIColor colorWithRed:(CGFloat)0xa5/0xff green:(CGFloat)0x1c/0xff blue:(CGFloat)0x78/0xff alpha:1.0];
				maxColour = @"f80baa";
				break;				
		}				
		[cell setSwatchColor:swatchColor forEntityAtIndex:[rowMetrics indexOfObject:entity]];
	}
    
    return cell;
}


/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/


/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:YES];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/


/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/


/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/


#pragma mark -
#pragma mark Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    // Navigation logic may go here. Create and push another view controller.
	/*
	 <#DetailViewController#> *detailViewController = [[<#DetailViewController#> alloc] initWithNibName:@"<#Nib name#>" bundle:nil];
     // ...
     // Pass the selected object to the new view controller.
	 [self.navigationController pushViewController:detailViewController animated:YES];
	 [detailViewController release];
	 */
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


@end

