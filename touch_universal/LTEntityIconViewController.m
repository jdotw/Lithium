    //
//  LTEntityIconViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 3/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTEntityIconViewController.h"
#import "LTEntity.h"
#import "LTEntityTableViewController.h"

@implementation LTEntityIconViewController

@synthesize entity=_entity, selected, delegate;

- (id) initWithNibName:(NSString *)nibName entity:(LTEntity *)entity
{
	self = [super initWithNibName:nibName bundle:nil];
	if (!self) return nil;
	
	self.entity = entity;
	
	UILongPressGestureRecognizer *longRecog = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(iconHeldLong:)];
	[longRecog setCancelsTouchesInView:YES];
	[self.view addGestureRecognizer:longRecog];
	[longRecog release];
	
	UITapGestureRecognizer *tapRecog = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(iconTapped:)];
	[tapRecog setCancelsTouchesInView:YES];
	[self.view addGestureRecognizer:tapRecog];	
	[tapRecog release];
		
	return self;
}

- (void) setEntity:(LTEntity *)entity
{
	[self loadView];
	
	[_entity release];
	_entity = [entity retain];
	
	label.text = self.entity.desc;
	imageView.image = self.entity.icon;
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

- (void) iconTapped:(UIGestureRecognizer *)recog
{
}

- (void) iconHeldLong:(UIGestureRecognizer *)recog
{
	/* Default handling is to present a pop-over from our view */
	if (recog.state == UIGestureRecognizerStateBegan)
	{
		CGPoint touchPoint = [recog locationOfTouch:0 inView:self.view];
		LTEntityTableViewController *vc = [[LTEntityTableViewController alloc] initWitEntity:self.entity];
		UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:vc];
		UIPopoverController *pop = [[UIPopoverController alloc] initWithContentViewController:nav];
		[pop presentPopoverFromRect:CGRectMake(touchPoint.x, touchPoint.y, 1.0, 1.0)
							 inView:self.view
		   permittedArrowDirections:UIPopoverArrowDirectionAny
						   animated:YES];
		[vc release];
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
	[_entity release];
    [super dealloc];
}


@end
