//
//  LTContainerIconViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTContainerIconViewController.h"
#import "LTEntity.h"

@implementation LTContainerIconViewController

@synthesize container=_container, selected, delegate;

- (id) initWithContainer:(LTEntity *)container
{
	self = [[LTContainerIconViewController alloc] initWithNibName:@"LTContainerIconViewController" bundle:nil];
	if (!self) return nil;
	
	self.container = container;
	
	UITapGestureRecognizer *longRecog = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(iconHeldLong:)];
	[longRecog setCancelsTouchesInView:YES];
	[self.view addGestureRecognizer:longRecog];
	
	return self;
}

- (void) setContainer:(LTEntity *)entity
{
	[self loadView];
	
	[_container release];
	_container = [entity retain];
	
	label.text = self.container.desc;
	imageView.image = self.container.icon;
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
	if ([delegate respondsToSelector:@selector(setSelectedContainer:)])
	{
		[delegate performSelector:@selector(setSelectedContainer:) withObject:self.container];
	}
}

- (void) iconHeldLong:(UIGestureRecognizer *)recog
{
	NSLog (@"Long Held!");
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
