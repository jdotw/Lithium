//
//  LTTriggerSetTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTTriggerSetTableViewController.h"

#import "LTTriggerSet.h"
#import "LTTriggerSetList.h"
#import "LTTrigger.h"
#import "LTTriggerTableViewController.h"

@implementation LTTriggerSetTableViewController

@synthesize tset=_tset, metric=_metric;

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

        /* Enabled Switch */
        CGRect switchFrame = CGRectMake(0.0, 0.0, 94.0, 27.0);
        enabledSwitch = [[UISwitch alloc] initWithFrame:switchFrame];
        enabledSwitch.backgroundColor = [UIColor clearColor];
        [enabledSwitch addTarget:self action:@selector(enabledChanged:) forControlEvents:UIControlEventValueChanged];

        /* Object Scope Switch */
        objScopeSwitch = [[UISwitch alloc] initWithFrame:switchFrame];
        objScopeSwitch.backgroundColor = [UIColor clearColor];
        [objScopeSwitch addTarget:self action:@selector(objScopeChanged:) forControlEvents:UIControlEventValueChanged];

        /* Device Scope Switch */
        devScopeSwitch = [[UISwitch alloc] initWithFrame:switchFrame];
        devScopeSwitch.backgroundColor = [UIColor clearColor];
        [devScopeSwitch addTarget:self action:@selector(devScopeChanged:) forControlEvents:UIControlEventValueChanged];

        /* Site Scope Switch */
        siteScopeSwitch = [[UISwitch alloc] initWithFrame:switchFrame];
        siteScopeSwitch.backgroundColor = [UIColor clearColor];
        [siteScopeSwitch addTarget:self action:@selector(siteScopeChanged:) forControlEvents:UIControlEventValueChanged];

    }
    return self;
}

- (void)dealloc
{
    self.metric = nil;
    self.tset = nil;
    [tsetList release];
    [enabledSwitch release];
    [objScopeSwitch release];
    [devScopeSwitch release];
    [siteScopeSwitch release];
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void) _updateRightButtonAndScopeSelectors
{
    if ([self.tset setOrTriggersHaveChanged])
    {
        self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemSave
                                                                                                target:self
                                                                                                action:@selector(saveTouched:)] autorelease];
        
    }
    else
    {
        self.navigationItem.rightBarButtonItem = nil;
    }
    [self.tableView reloadData];
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.navigationItem.leftBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel target:self action:@selector(cancelTouched:)] autorelease];
    
    [tsetList refresh];
    self.navigationItem.title = @"Loading Triggers...";
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
    [self _updateRightButtonAndScopeSelectors];
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

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return YES;
}

#pragma mark - Notification Received

- (void) alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    [self dismissModalViewControllerAnimated:YES];
}

- (void) triggerSetListRefreshFinished:(NSNotification *)note
{
    /* Find the triggerset for our metric */
    LTTriggerSet *tset = [tsetList.childDict objectForKey:self.metric.name];
    if (tset)
    {
        /* FOund a maching triggerset */
        self.tset = tset;
    }
    else
    {
        /* No triggersets found! */
        UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"No Triggers Found"
                                                        message:@"There does not seem to be any triggers defined for this metric"
                                                       delegate:self
                                              cancelButtonTitle:@"Dismiss"
                                               otherButtonTitles: nil] autorelease];
        [alert show];
    }
}

#pragma mark - TriggerSet

- (void) setTset:(LTTriggerSet *)tset
{
    [_tset release];
    _tset = [tset retain];
    
    if (_tset)
    {
        enabledSwitch.on = _tset.applied;   
        self.navigationItem.title = [NSString stringWithFormat:@"%@ %@", self.metric.object.desc, _tset.desc];
        [self.tableView reloadData];
    }
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    if (self.tset) 
    {
        if ([self.tset setOrTriggersHaveChanged])
        { return 3; }
        else
        { return 2; }
    }
    else return 0;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    switch (section)
    {
        case 0:
            /* Enabled */
            return 1;
        case 1:
            /* Triggers */
            return self.tset.children.count;
        case 2:
            /* Scope */
            if (devScopeSwitch.on) return 3;
            else return 2;
        default:
            return 0;
    }
}

- (NSString *) tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    switch (section) {
        case 2:
            return @"Apply Changes to...";
        default:
            return nil;
    }
}

- (NSString *) tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
{
    if (section == 2)
    {
        NSMutableString *scopeString = [NSMutableString stringWithFormat:@"The changes will be applied to"];

        if (objScopeSwitch.on)
        {
            [scopeString appendFormat:@" all %@ objects", self.metric.container.desc];
        }
        else
        {
            [scopeString appendFormat:@" the '%@' object", self.metric.object.desc];
        }
        
        if (devScopeSwitch.on)
        {
            [scopeString appendFormat:@", on all devices"];
        }
        else
        {
            [scopeString appendFormat:@", on the '%@' device", self.metric.device.desc];
        }
        
        if (siteScopeSwitch.on)
        {
            [scopeString appendFormat:@", at all locations"];
        }
        else
        {
            [scopeString appendFormat:@", at the '%@' location", self.metric.site.desc];
        }
        
        return scopeString;
    }
    else return nil;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier;
    CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
    {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell...
    if (indexPath.section == 0)
    {
        /* Enabled switch */
        cell.textLabel.text = @"Enabled";
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.accessoryView = enabledSwitch;
    }
    else if (indexPath.section == 1)
    {
        /* Trigger List */
        LTTrigger *trg = [self.tset.children objectAtIndex:indexPath.row];
        cell.textLabel.text = trg.desc;
        cell.detailTextLabel.text = trg.conditionString;
        cell.accessoryView = nil;
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }
    else if (indexPath.section == 2)
    {
        /* Scope */
        switch (indexPath.row)
        {
            case 0:
                cell.textLabel.text = [NSString stringWithFormat:@"Apply to all %@", self.metric.container.desc];
                if (![self.metric.container.desc hasSuffix:@"s"]) 
                { cell.textLabel.text = [NSString stringWithFormat:@"%@ Objects", cell.textLabel.text]; } // Add Objects
                cell.accessoryView = objScopeSwitch;
                break;
            case 1:
                cell.textLabel.text = @"On All Devices";
                cell.accessoryView = devScopeSwitch;
                break;
            case 2:
                cell.textLabel.text = @"At All Locations";
                cell.accessoryView = siteScopeSwitch;
                break;
        }
    }
    
    return cell;
}

#pragma mark - Table view delegate

- (NSIndexPath *) tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == 1)
    {
        /* Trigger Selection Allowed */
        return indexPath;
    }
    else
    {
        /* Deny other selection */
        return nil;
    }
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == 1)
    {
        /* Trigger Selected */
        LTTrigger *trg = [self.tset.children objectAtIndex:indexPath.row];
        LTTriggerTableViewController *vc = [[[LTTriggerTableViewController alloc] initWithTrigger:trg] autorelease];
        [self.navigationController pushViewController:vc animated:YES];
    }
}

#pragma mark - UI Actions

- (void) enabledChanged:(id)sender
{
    self.tset.applied = enabledSwitch.on;
    [self.tableView reloadData];

    [self _updateRightButtonAndScopeSelectors];
}

- (void) objScopeChanged:(id)sender
{
    [self.tableView reloadData];    // Updates the scope statement in the footer
}

- (void) devScopeChanged:(id)sender
{
    [self.tableView reloadData];    // Updates the scope statement in the footer    
}

- (void) siteScopeChanged:(id)sender
{
    [self.tableView reloadData];    // Updates the scope statement in the footer
}

- (void) saveTouched:(id)sender
{
    if (self.tset.setOrTriggersHaveChanged)
    {
        /* Changes have happened */
        NSString *siteName = nil;
        NSString *devName = nil;
        NSString *objName = nil;
        if (!siteScopeSwitch.on) siteName = self.tset.site.name;
        if (!devScopeSwitch.on) devName = self.tset.device.name;
        if (!objScopeSwitch.on) objName = self.tset.object.name;
        self.tset.delegate = self;
        [self.tset sendRuleUpdatesForScopeObject:objName
                                          device:devName
                                            site:siteName];
        if (self.tset.ruleUpdateInProgress)
        {
            /* Rule update is in progress */
            NSLog(@"Starting Update!!");
        }
        else
        {
            /* Rule update did not start */
            UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Update Failed"
                                                             message:@"The Trigger update failed, please check your connection and try again."
                                                            delegate:nil cancelButtonTitle:@"Dismiss" otherButtonTitles:nil] autorelease];
            [alert show];
        }
    }
    else
    {
        /* No changes to make */
        NSLog(@"No changes to make, dismissing");
        [self dismissModalViewControllerAnimated:YES];
    }
}

- (void) cancelTouched:(id)sender
{
    [self dismissModalViewControllerAnimated:YES];
}

#pragma mark - TriggerSet Delegate

- (void) triggerSetUpdateDidFail:(LTTriggerSet *)tset
{
    /* Update failed! */
    UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Update Failed"
                                                    message:@"The Trigger update failed, please check your connection and try again."
                                                   delegate:nil cancelButtonTitle:@"Dismiss" otherButtonTitles:nil] autorelease];
    [alert show];
}

- (void) triggerSetUpdateDidFinish:(LTTriggerSet *)tset
{
    /* Update finished */
    NSLog (@"Finished update");
    [self dismissModalViewControllerAnimated:YES];
    [self.metric.device forceRefresh];
}

@end
