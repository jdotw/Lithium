//
//  MBModuleBuilderController.m
//  ModuleBuilder
//
//  Created by James Wilson on 26/04/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBModuleBuilderController.h"

#import "LCFeedbackController.h"

@implementation MBModuleBuilderController

- (IBAction) provideFeedbackClicked:(id)sender
{
	[[LCFeedbackController alloc] initForWindow:nil];
}

@end
