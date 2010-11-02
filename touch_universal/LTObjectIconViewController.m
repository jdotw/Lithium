    //
//  LTObjectIconViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 2/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTObjectIconViewController.h"
#import "LTEntity.h"

@implementation LTObjectIconViewController

@synthesize object=_object, selected, delegate;

- (id) initWithObject:(LTEntity *)object
{
	self = [[LTObjectIconViewController alloc] initWithNibName:@"LTObjectIconViewController" bundle:nil];
	if (!self) return nil;
	
	self.object = object;
	
	return self;
}

- (void) setObject:(LTEntity *)value
{
	[self loadView];
	
	[_object release];
	_object = [value retain];
	
	label.text = self.object.desc;
	imageView.image = self.object.icon;
}

- (void) setSelected:(BOOL)value
{
	selected = value;
	self.view.backgroundColor = selected ? [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.2] : [UIColor clearColor];
	[self.view setNeedsDisplay];
}

- (void)viewDidLoad 
{
    [super viewDidLoad];
}

- (IBAction) iconTapped:(id)sender
{
	if ([delegate respondsToSelector:@selector(setSelectedObject:)])
	{
		[delegate performSelector:@selector(setSelectedObject:) withObject:self.object];
	}
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return YES;
}


- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)dealloc {
    [super dealloc];
}


@end
