//
//  MBTrigger.m
//  ModuleBuilder
//
//  Created by James Wilson on 6/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBTrigger.h"

#import "MBTriggerViewController.h"

@implementation MBTrigger

- (id) init
{ 
	self = [super init];
	[self setType:[NSNumber numberWithInt:7]];
	[self setCondition:[NSNumber numberWithInt:3]];
	[self setSeverity:[NSNumber numberWithInt:1]];
	return self;
}

#pragma mark "View Controller"

- (NSViewController *) viewController
{ 
	return [MBTriggerViewController viewForTrigger:self]; 
}

- (float) rowHeight
{ 
	return 54.0;
}

- (NSNumber *) severity
{ return [properties objectForKey:@"severity"]; }
- (void) setSeverity:(NSNumber *)value
{ 
	BOOL updateDesc;
	if (![self desc]) updateDesc = YES;
	else
	{
		switch ([[self severity] intValue])
		{
			case 1:
				if ([[self desc] isEqualToString:@"Warning"]) updateDesc = YES;
				break;
			case 2:
				if ([[self desc] isEqualToString:@"Impaired"]) updateDesc = YES;
				break;
			case 3:
				if ([[self desc] isEqualToString:@"Critical"]) updateDesc = YES; 
				break;
		}
	}
	
	[[[document undoManager] prepareWithInvocationTarget:self] setSeverity:(id)[self severity]];
	[properties setObject:value forKey:@"severity"]; 
	[[document undoManager] setActionName:@"Set Trigger Severity"];
	
	if (updateDesc)
	{
		NSString *newDesc = nil;
		switch ([[self severity] intValue])
		{
			case 1:
				newDesc = @"Warning";
				break;
			case 2:
				newDesc = @"Impaired";
				break;
			case 3:
				newDesc = @"Critical";
				break;
		}				
		if (newDesc) 
		{
			[self willChangeValueForKey:@"desc"];
			[[self properties] setObject:newDesc forKey:@"desc"];
			[self didChangeValueForKey:@"desc"];
		}
	}
}

- (NSNumber *) condition
{ return [properties objectForKey:@"condition"]; }
- (void) setCondition:(NSNumber *)value
{ 
	[[[document undoManager] prepareWithInvocationTarget:self] setCondition:(id)[self condition]];
	[properties setObject:value forKey:@"condition"]; 
	[[document undoManager] setActionName:@"Set Trigger Condition"];
	if ([value intValue] == 5)
	{ [self setHasSecondValue:YES]; }
	else
	{ [self setHasSecondValue:NO]; }
}

- (NSString *) xValue 
{ return [properties objectForKey:@"xValue"]; }
- (void) setXValue:(NSString *)value
{ 
	[[[document undoManager] prepareWithInvocationTarget:self] setXValue:(id)[self xValue]];
	if (value) [properties setObject:value forKey:@"xValue"]; 
	else [properties removeObjectForKey:@"xValue"];
	[[document undoManager] setActionName:@"Set Trigger X Value"];	
}

- (NSString *) yValue 
{ return [properties objectForKey:@"yValue"]; }
- (void) setYValue:(NSString *)value
{ 
	[[[document undoManager] prepareWithInvocationTarget:self] setYValue:(id)[self yValue]];
	if (value) [properties setObject:value forKey:@"yValue"]; 
	else [properties removeObjectForKey:@"yValue"];
	[[document undoManager] setActionName:@"Set Trigger Y Value"];		
}

- (BOOL) hasSecondValue
{ return [[properties objectForKey:@"hasSecondValue"] boolValue]; }

- (void) setHasSecondValue:(BOOL)flag
{ [properties setObject:[NSNumber numberWithBool:flag] forKey:@"hasSecondValue"]; }

- (NSNumber *) duration
{ return [properties objectForKey:@"duration"]; }

- (void) setDuration:(NSNumber *)value
{
	[(MBTrigger *) [[document undoManager] prepareWithInvocationTarget:self] setDuration:(id)[self duration]];
	[properties setObject:value forKey:@"duration"]; 
	[[document undoManager] setActionName:@"Set Trigger Duration"];			
}

- (NSString *) conditionMatchString
{
	switch ([[self condition] intValue] == 5)
	{
		case 1:
			return [NSString stringWithFormat:@"Equal to %@", [self xValue]];
		case 2:
			return [NSString stringWithFormat:@"Less than %@", [self xValue]];
		case 3:
			return [NSString stringWithFormat:@"Greater than %@", [self xValue]];
		case 4:
			return [NSString stringWithFormat:@"Not equal to %@", [self xValue]];
		case 5:
			return [NSString stringWithFormat:@"Between %@ and %@", [self xValue], [self yValue]];
		default:
			return nil;
	}
}

#pragma mark "Overlap Handling"

- (MBTrigger *) checkForOverlap
{
	/* Check for overlap */
	for (MBTrigger *trg in [parent children])
	{
		/* Greater-Than */
		if ([[self condition] intValue] == 3)
		{
			/* Check for greater than overlap */
			if ([[trg condition] intValue] == 3 && [[trg xValue] floatValue] <= [[self xValue] floatValue])
			{ return trg; }
			
			/* Check for less than overlap */
			if ([[trg condition] intValue] == 2 && [[trg xValue] floatValue] > [[self xValue] floatValue])
			{ return trg; }
			
			/* Check for equal to overlap */
			if ([[trg condition] intValue] == 1 && [[trg xValue] floatValue] >= [[self xValue] floatValue])
			{ return trg; }
			
			/* Check for range overlap */
			if ([[trg condition] intValue] == 5 && [[trg yValue] floatValue] >= [[self xValue] floatValue])
			{ return trg; }
		}
		
		/* Less-Than */
		if ([[self condition] intValue] == 2)
		{
			/* Check for greater than overlap */
			if ([[trg condition] intValue] == 3 && [[trg xValue] floatValue] <= [[self xValue] floatValue])
			{ return trg; }
			
			/* Check for less than overlap */
			if ([[trg condition] intValue] == 2 && [[trg xValue] floatValue] > [[self xValue] floatValue])
			{ return trg; }
			
			/* Check for equal to overlap */
			if ([[trg condition] intValue] == 1 && [[trg xValue] floatValue] <= [[self xValue] floatValue])
			{ return trg; }
			
			/* Check for range overlap */
			if ([[trg condition] intValue] == 5 && [[trg xValue] floatValue] < [[self xValue] floatValue])
			{ return trg; }
		}
		
		/* Equal-To */
		if ([[self condition] intValue] == 1)
		{
			/* Check for greater than overlap */
			if ([[trg condition] intValue] == 3 && [[trg xValue] floatValue] <= [[self xValue] floatValue])
			{ return trg; }
			
			/* Check for less than overlap */
			if ([[trg condition] intValue] == 2 && [[trg xValue] floatValue] > [[self xValue] floatValue])
			{ return trg; }
			
			/* Check for equal to overlap */
			if ([[trg condition] intValue] == 1 && [[trg xValue] floatValue] == [[self xValue] floatValue])
			{ return trg; }
			
			/* Check for range overlap */
			if ([[trg condition] intValue] == 5)
			{
				if ([[trg xValue] floatValue] <= [[self xValue] floatValue] && [[trg yValue] floatValue] > [[self xValue] floatValue])
				{ return trg; }
			}
		}
		
		/* Range */
		if ([[self condition] intValue] == 5)
		{
			/* Check for greater than overlap */
			if ([[trg condition] intValue] == 3 && [[trg xValue] floatValue] < [[self yValue] floatValue])
			{ return trg; }
			
			/* Check for less than overlap */
			if ([[trg condition] intValue] == 2 && [[trg xValue] floatValue] > [[self xValue] floatValue])
			{ return trg; }
			
			/* Check for equal to overlap */
			if ([[trg condition] intValue] == 1)
			{
				if ([[trg xValue] floatValue] >= [[self xValue] floatValue] && [[trg xValue] floatValue] < [[self yValue] floatValue])
				{ return trg; }
			}
			
			/* Check for range overlap */
			if ([[trg condition] intValue] == 5)
			{
				if ([[self xValue] floatValue] >= [[trg xValue] floatValue] && [[self xValue] floatValue] < [[trg yValue] floatValue])
				{ return trg; }
				if ([[self yValue] floatValue] >= [[trg xValue] floatValue] && [[self yValue] floatValue] < [[trg yValue] floatValue])
				{ return trg; }
			}
		}
	}
	
	return nil;
}

@end
