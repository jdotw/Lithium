    //
//  LTModalProgressViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 8/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTModalProgressViewController.h"
#import "LTEntity.h"

@implementation LTModalProgressViewController

@synthesize entity;

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

- (void) entityRefreshFinished:(NSNotification *)note
{
	NSLog (@"%@ got RefreshFinished on thread %@ (%@)", self, [NSThread currentThread], [NSThread mainThread]);
	progressLabel.text = @"Finished.";
//	NSLog (@"Dismissing %@ from %@", self, self.navigationController);
//	[self dismissModalViewControllerAnimated:YES];
}

- (void) entityRefreshStatusUpdated:(NSNotification *)note
{
	NSLog (@"%@ got Status updated", self);
//	progressLabel.text = entity.xmlStatus;
}

- (void) setEntity:(LTEntity *)value
{
	[entity release];
	entity = [value retain];
	
	NSLog (@"Listening for RefreshFinished on %@", entity);
	
//	[[NSNotificationCenter defaultCenter] addObserver:self
//											 selector:@selector(entityRefreshFinished:)
//												 name:@"RefreshFinished" object:entity];
//	[[NSNotificationCenter defaultCenter] addObserver:self
//											 selector:@selector(entityRefreshStatusUpdated:)
//												 name:@"LTEntityXmlStatusChanged" object:entity];	
	
}

- (void) viewDidAppear:(BOOL)animated
{
	[self entityRefreshStatusUpdated:nil];	
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
	if (entity)
	{
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"RefreshFinished" object:entity];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTEntityXmlStatusChanged" object:entity];
		[entity release];
	}
    [super dealloc];
}


@end
