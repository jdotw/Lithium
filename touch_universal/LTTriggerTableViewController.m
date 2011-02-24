//
//  LTTriggerTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTTriggerTableViewController.h"
#import "LTTrigger.h"

@implementation LTTriggerTableViewController

@synthesize trg=_trg;

- (id)initWithTrigger:(LTTrigger *)trg
{
    self = [super initWithStyle:UITableViewStyleGrouped];
    if (self) 
    {
        _trg = [trg retain];
        
        /* Enabled Switch */
        CGRect switchFrame = CGRectMake(0.0, 0.0, 94.0, 27.0);
        enabledSwitch = [[UISwitch alloc] initWithFrame:switchFrame];
        enabledSwitch.backgroundColor = [UIColor clearColor];
        if (trg.adminState == 0) enabledSwitch.on = YES;
        else enabledSwitch.on = NO;
        [enabledSwitch addTarget:self action:@selector(enabledChanged:) forControlEvents:UIControlEventValueChanged];
        
        /* Condition Segment */
        conditionSegment = [[UISegmentedControl alloc] initWithItems:[NSArray arrayWithObjects:@"=", @"<", @">", @"!=", @"> <", nil]];
        conditionSegment.backgroundColor = [UIColor clearColor];
        conditionSegment.selectedSegmentIndex = self.trg.triggerType-1;
        [conditionSegment addTarget:self action:@selector(conditionChanged:) forControlEvents:UIControlEventValueChanged];
        
        
        /* xValue Text Field */
        CGRect fieldRect = CGRectMake(0.0, 0.0, 160, 23);
        xValueField = [[UITextField alloc] initWithFrame:fieldRect];
        xValueField.backgroundColor = [UIColor clearColor];
        xValueField.textAlignment = UITextAlignmentRight;
        xValueField.clearButtonMode = UITextFieldViewModeWhileEditing;
        xValueField.delegate = self;
        
        /* yValue Text Field */
        yValueField = [[UITextField alloc] initWithFrame:fieldRect];
        yValueField.backgroundColor = [UIColor clearColor];
        yValueField.textAlignment = UITextAlignmentRight;
        yValueField.clearButtonMode = UITextFieldViewModeWhileEditing;
        yValueField.delegate = self;
        
        /* Duration Text Field */
        durationField = [[UITextField alloc] initWithFrame:fieldRect];
        durationField.backgroundColor = [UIColor clearColor];
        durationField.textAlignment = UITextAlignmentRight;
        durationField.clearButtonMode = UITextFieldViewModeWhileEditing;
        durationField.keyboardType = UIKeyboardTypeNumberPad;
        durationField.delegate = self;

        /* Set keyboard type */
        if  (self.trg.valueType != VALTYPE_STRING)
        {
            /* Set to numeric keypad for non-string values */
            xValueField.keyboardType = UIKeyboardTypeDecimalPad;
            yValueField.keyboardType = UIKeyboardTypeDecimalPad;
        }

    }
    return self;
}

- (void)dealloc
{
    self.trg = nil;
    [enabledSwitch release];
    [conditionSegment release];
    [xValueField release];
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
 
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
    
    self.navigationItem.title = self.trg.desc;
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

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    if (self.trg.adminState == 0) return 4;
    else return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    switch (section)
    {
        case 0:
            /* Enabled */
            return 1;
        case 1:
            /* Condition */
            return 1;
        case 2:
            /* Value */
            if (self.trg.triggerType == TRGTYPE_RANGE) return 2;
            else return 1;
        case 3:
            /* Duration */
            return 1;
        default:
            return 0;
    }
}

- (NSString *) tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return nil;
}

- (NSString *) tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
{
    if (section == 3)
    {
        /* Duration */
        return @"The amount of time the condition must be present before Lithium will raise an Incident";
    }
    else return nil;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell...
    if (indexPath.section == 0)
    {
        /* Enabled */
        cell.textLabel.text = @"Enabled";
        cell.accessoryView = enabledSwitch;
        cell.accessoryType = UITableViewCellAccessoryNone;
    }
    else if (indexPath.section == 1)
    {
        /* Condition */
        cell.accessoryView = conditionSegment;
        cell.accessoryType = UITableViewCellAccessoryNone;        
        cell.textLabel.text = @"Condition";
    }
    else if (indexPath.section == 2)
    {
        if (indexPath.row == 0)
        {
            /* X Value */
            if (self.trg.triggerType == TRGTYPE_RANGE)
            { cell.textLabel.text = @"Min Value"; }
            else 
            { cell.textLabel.text = @"Value"; }
            cell.accessoryView = xValueField;
            xValueField.text = self.trg.xValue;
        }
        else if (indexPath.row == 1)
        {
            /* Y Value */
            cell.textLabel.text = @"Max Value";
            cell.accessoryView = yValueField;
            yValueField.text = self.trg.yValue;
        }
    }
    else if (indexPath.section == 3)
    {
        /* Duration */
        cell.textLabel.text = @"Duration";
        cell.accessoryView = durationField;
        durationField.text = [NSString stringWithFormat:@"%i", self.trg.duration];
    }
    
    return cell;
}

- (void) tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == 1)
    {
        /* Condition */
    }
}

#pragma mark - Table view delegate

- (NSIndexPath *) tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    return nil;  // Deny all selection
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
}

#pragma mark - UI Actions

- (void) enabledChanged:(id)sender
{
    if (enabledSwitch.on)
    {
        self.trg.adminState = 0;    // Enabled
    }
    else if (!enabledSwitch.on)
    {
        self.trg.adminState = 1;    // Disabled
    }
    [self.tableView reloadData];    
}

- (void) conditionChanged:(id)sender
{
    self.trg.triggerType = conditionSegment.selectedSegmentIndex+1;
    [self.tableView reloadData];
}

- (BOOL) textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
    if (textField == xValueField)
    {
        NSLog (@"X Changed to %@", [xValueField.text stringByReplacingCharactersInRange:range withString:string]);
        self.trg.xValue = [xValueField.text stringByReplacingCharactersInRange:range withString:string];
    }
    else if (textField == yValueField)
    {
        NSLog (@"Y Changed to %@", [yValueField.text stringByReplacingCharactersInRange:range withString:string]);
        self.trg.yValue = [yValueField.text stringByReplacingCharactersInRange:range withString:string];
    }
    else if (textField == durationField)
    {
        self.trg.duration = [[durationField.text stringByReplacingCharactersInRange:range withString:string] intValue];
        NSLog (@"Duraton changed to %i", self.trg.duration);
    }
    return YES;
}

@end
