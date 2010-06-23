//
//  MBMetricTextField.h
//  ModuleBuilder
//
//  Created by James Wilson on 7/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MBMetricTextField : NSTextField 
{
	IBOutlet NSViewController *viewController;
	NSString *metricProperty;
}

@property (assign) NSString *metricProperty;

@end
