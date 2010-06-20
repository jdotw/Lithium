//
//  LCObjectTreeMetricViewController.m
//  Lithium Console
//
//  Created by James Wilson on 28/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCObjectTreeMetricViewController.h"

#import "LCBrowser2Controller.h"
#import "LCMetricGraphDocument.h"
#import "LCMetricAnalysisWindowController.h"
#import "LCCaseController.h"
#import "LCFaultHistoryController.h"
#import "LCTriggerTuningWindowController.h"
#import "LCMetricHistoryWindowController.h"


@interface LCObjectTreeMetricViewController (private)

- (void) adjustValueFieldPosition;

@end

@implementation LCObjectTreeMetricViewController

#pragma mark "Constructors"

- (id) initWithMetric:(LCMetric *)initMetric
{
	self = [super initWithNibName:@"ObjectTreeMetricView" bundle:nil];
	
	if (self)
	{
		self.metric = initMetric;
	}
	
	return self;
}

- (void) dealloc
{
	[metric removeObserver:self forKeyPath:@"hasTriggers"];
	[metric release];
	[super dealloc];
}

#pragma mark "Geometry"

- (void) adjustValueFieldPosition
{
	NSRect newFrame = [metricValueField frame];
	if (metric.hasTriggers && !dotShown)
	{
		newFrame.origin.x = newFrame.origin.x + 13.0;
		dotShown = YES;
	}
	else if (!metric.hasTriggers && dotShown)
	{
		newFrame.origin.x = newFrame.origin.x - 13.0;
		dotShown = NO;
	}
	[metricValueField setFrame:newFrame];	
}

#pragma mark "KVO Observeration"

- (void) observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if ([keyPath isEqualToString:@"hasTriggers"])
	{
		[self adjustValueFieldPosition];
	}
}

#pragma mark "Context Menu"

- (void)menuWillOpen:(NSMenu *)menu
{
	NSMutableString *string = [NSMutableString stringWithFormat:@"%@ %@", metric.displayString, [metric.object displayString]];
	if (metric.displayValue) [string appendFormat:@" (%@)", metric.displayValue];
	[menuTitleItem setTitle:string];
}

- (void)menuDidClose:(NSMenu *)menu
{
	[[self view] setNeedsDisplay:YES];
}

- (IBAction) refreshDeviceClicked:(id)sender
{
	[metric.device highPriorityRefresh];
}

- (IBAction) openMetricInNewWindowClicked:(id)sender
{
	[[LCBrowser2Controller alloc] initWithEntity:metric];
}

- (IBAction) metricHistoryClicked:(id)sender
{
	[[LCMetricHistoryWindowController alloc] initWithMetric:self.metric];
}

- (IBAction) graphMetricClicked:(id)sender
{
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntity = self.metric;
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];			
}

- (IBAction) trendAnalysisClicked:(id)sender
{
	[[LCMetricAnalysisWindowController alloc] initWithObject:metric.object];
}

- (IBAction) openCaseForMetricClicked:(id)sender
{
	[[LCCaseController alloc] initForNewCaseWithEntityList:[NSArray arrayWithObject:self.metric]];
}

- (IBAction) faultHistoryClicked:(id)sender
{
	[[LCFaultHistoryController alloc] initForEntity:self.metric];
}

- (IBAction) adjustTriggersClicked:(id)sender
{
	LCTriggerTuningWindowController *controller = [(LCTriggerTuningWindowController *)[LCTriggerTuningWindowController alloc] initWithObject:metric.object];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[[self view] window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	
	
}


#pragma mark "Properties"

- (void)setView:(NSView *)view
{
	[super setView:view];
	metricValueField.metric = metric;
	dotShown = YES;
	[self adjustValueFieldPosition];
}

@synthesize metric;
- (void) setMetric:(LCMetric *)value
{
	[metric removeObserver:self forKeyPath:@"hasTriggers"];

	[metric release];
	metric = [value retain];
	
	[metric addObserver:self
			 forKeyPath:@"hasTriggers"
				options:NSKeyValueObservingOptionNew
				context:nil];
}

	

@end
