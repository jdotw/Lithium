//
//  LTTriggerSetListTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTTriggerSetListTableViewController.h"
#import "LTEntity.h"
#import "LTTriggerSetList.h"
#import "LTTriggerSet.h"
#import "LTTriggerSetTableViewController.h"

@implementation LTTriggerSetListTableViewController

@synthesize  metric=_metric;

#pragma mark -
#pragma mark Initialization

- (id)initWithMetric:(LTEntity *)metric
{
    self = [super initWithStyle:UITableViewStyleGrouped];
    if (self) 
    {
        self.metric = metric;
        
        tsetList = [[LTTriggerSetList alloc] initWithMetric:self.metric];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(triggerSetListRefreshFinished:)
                                                     name:kLTTriggerSetListRefreshFinished
                                                   object:tsetList];
    }
    return self;
}

- (void)dealloc
{
    self.metric = nil;
    
    /* Remove tset list notifier and release list */
    [[NSNotificationCenter defaultCenter] removeObserver:self name:kLTTriggerSetListRefreshFinished object:tsetList];
    [tsetList release];
    
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - Notification Received

- (void) triggerSetListRefreshFinished:(NSNotification *)note
{
    [[self tableView] reloadData];
    self.navigationItem.titleView = nil;
    self.navigationItem.title = @"Trigger Sets";
    
    NSLog (@"Refresh finisherd and has %@", tsetList.children);
}

#pragma mark - Refresh

- (void) refresh
{
    self.navigationItem.title = @"Loading Trigger Sets...";
    [tsetList refresh];
}

#pragma mark - View Delegate

- (void)viewDidLoad
{
    [super viewDidLoad];

    /* Add cancel button */
    self.navigationItem.leftBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel
                                                                                           target:self
                                                                                           action:@selector(cancelTouched:)] autorelease];
    
    /* Refresh the list */
    [self refresh];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
}

#pragma mark - UI Actions

- (void) cancelTouched:(id)sender
{
    [self dismissModalViewControllerAnimated:YES];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return tsetList.children.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Get tset 
    LTTriggerSet *tset = [tsetList.children objectAtIndex:indexPath.row];
    
    // Configure the cell...
    cell.textLabel.text = tset.desc;
    if (tset.adminState == 0) cell.detailTextLabel.text = @"Enabled";
    else cell.detailTextLabel.text = @"Disabled";
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    
    return cell;
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    LTTriggerSet *tset = [tsetList.children objectAtIndex:indexPath.row];
    LTTriggerSetTableViewController *vc = [[[LTTriggerSetTableViewController alloc] initWithTriggerSet:tset] autorelease];
    [self.navigationController pushViewController:vc animated:YES];
}

@end
