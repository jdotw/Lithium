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
	self.view.backgroundColor = selected ? [UIColor blueColor] : [UIColor clearColor];
	[self.view setNeedsDisplay];
}

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

- (void)viewDidLoad 
{
    [super viewDidLoad];
	
//	UITapGestureRecognizer *tapRecog = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(iconTapped:)];
//	tapRecog.numberOfTapsRequired = 1;
//	self.view.gestureRecognizers = [NSArray arrayWithObject:tapRecog];
//	[tapRecog release];
}

- (IBAction) iconTapped:(id)sender
{
	if ([delegate respondsToSelector:@selector(setSelectedContainer:)])
	{
		[delegate performSelector:@selector(setSelectedContainer:) withObject:self.container];
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
