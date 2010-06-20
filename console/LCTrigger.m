//
//  LCTrigger.m
//  Lithium Console
//
//  Created by James Wilson on 27/09/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTrigger.h"


@implementation LCTrigger

- (LCTrigger *) init
{
	[super init];
	
	[self.xmlTranslation setObject:@"duration" forKey:@"duration"];
	[self.xmlTranslation setObject:@"valueType" forKey:@"val_type_num"];
	[self.xmlTranslation setObject:@"triggerType" forKey:@"trg_type_num"];
	[self.xmlTranslation setObject:@"effect" forKey:@"effect_num"];
	[self.xmlTranslation setObject:@"activeTimeStamp" forKey:@"active_tstamp"];
	[self.xmlTranslation setObject:@"xValueString" forKey:@"xval"];
	[self.xmlTranslation setObject:@"yValueString" forKey:@"yval"];
	
	return self;
}

- (void) dealloc
{
	[xValueString release];
	[yValueString release];
	[super dealloc];
}

- (oneway void)release
{
	[super release];
}

@synthesize valueType;
@synthesize triggerType;

- (void) setTriggerType:(int)value
{
	triggerType = value;
	[self updateDisplayValue];
}

@synthesize effect;
@synthesize duration;
@synthesize activeTimeStamp;

@synthesize xValue;
@synthesize yValue;

@synthesize xValueString;

- (void) setXValueString:(NSString *)string
{
	[xValueString release];
	xValueString = [string copy];
	[self updateDisplayValue];
	self.xValue = [string floatValue];
}

@synthesize yValueString;

- (void) setYValueString:(NSString *)string
{
	[yValueString release];
	yValueString = [string copy];
	[self updateDisplayValue];
	self.yValue = [string floatValue];
}

- (void) updateDisplayValue
{
	NSString *string;
	switch (self.triggerType)
	{
		case 1:
			string = [NSString stringWithFormat:@"= %@", self.xValueString];
			break;
		case 2:
			string = [NSString stringWithFormat:@"< %@", self.xValueString];
			break;
		case 3:
			string = [NSString stringWithFormat:@"> %@", self.xValueString];
			break;
		case 4:
			string = [NSString stringWithFormat:@"!= %@", self.xValueString];					
			break;
		case 5:
			string = [NSString stringWithFormat:@"%@ - %@", self.xValueString, self.yValueString];					
			break;
		default:
			string = @"Unknown";
	}
	
	self.displayValue = string;
}

@end
