//
//  LTEntityTableViewCell.m
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTEntityTableViewCell.h"
#import "LTEntity.h"
#import "LTEntityDescriptor.h"

@implementation LTEntityTableViewCell

@synthesize entity, showCurrentValue, showFullLocation;

- (id) initWithReuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:reuseIdentifier];

	self.detailTextLabel.font = [UIFont systemFontOfSize:11.0];
	
	/* This may be used later as an accessory view */
	valueLabel = [[UILabel alloc] initWithFrame:CGRectZero];
	valueLabel.backgroundColor = [UIColor clearColor];
	valueLabel.textColor = [UIColor whiteColor];
	valueLabel.highlightedTextColor = [UIColor whiteColor];
	valueLabel.opaque = NO;
	valueLabel.font = [UIFont systemFontOfSize:16.0];
	valueLabel.textAlignment = UITextAlignmentRight;	
    
	return self;
}

- (void)dealloc 
{
	[entity release];
	[valueLabel release];
    [super dealloc];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void) prepareForReuse
{
	self.showCurrentValue = NO;
	self.showFullLocation = NO;
	self.entityState = 0;
}

- (void) entityStateChanged:(NSNotification *)note
{
	if (self.entity.type > 0)
	{ self.entityState = entity.opState; }
}

- (void) entityValueChanged:(NSNotification *)note
{
    /* Update value label */
	valueLabel.text = self.entity.currentValue;
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bits" withString:@"b"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bytes" withString:@"B"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bit" withString:@"b"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"byte" withString:@"B"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"writes" withString:@"wr"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"reads" withString:@"rd"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"sec" withString:@"s"];
	
    /* Trigger layout update */
	CGSize stringSize = [valueLabel.text sizeWithFont:valueLabel.font];
	valueLabel.frame = CGRectMake(0., 0., stringSize.width, stringSize.height);	
    [valueLabel.superview setNeedsLayout];
}

- (void) setEntity:(LTEntity *)value
{
    /* Remove old observers */
    if (entity)
    {
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kLTEntityStateChanged object:entity];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kLTEntityValueChanged object:entity];
    }
	/* If the entity if an object with one metric, 
	 * use that metric as the entity rather than the object
	 */
	if (value.type == 5 && value.children.count == 1)
	{
		value = [value.children objectAtIndex:0];
	}

	/* Set entity */
	[entity release];
	entity = [value retain];
	
	/* Set main label (entity description) */
	NSString *desc = self.entity.desc;
	self.textLabel.text = desc;
	
	/* Trigger a status update */
	[self entityStateChanged:nil];
    
	/* Entity Type-specific handling */
	if (entity.type == 6)
	{
		/* If we're a metric, append the object name to the desc */
		if (![self.entity.entityDescriptor.objName isEqualToString:@"master"]) 
		{ desc = [NSString stringWithFormat:@"%@ %@", self.entity.entityDescriptor.objName, desc]; }
		
		/* And set showCurrentValue to YES by default */
		self.showCurrentValue = YES;
	}
    
    /* Observe changes in state and value */
    if (entity)
    {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(entityStateChanged:)
                                                     name:kLTEntityStateChanged
                                                   object:entity];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(entityValueChanged:)
                                                     name:kLTEntityValueChanged
                                                   object:entity];
    }
}

- (void) setShowCurrentValue:(BOOL)value
{
	showCurrentValue = value;
	
	if (showCurrentValue)
	{
		self.accessoryView = valueLabel;
		[self entityValueChanged:nil];
	}
	else 
	{
		self.accessoryView = nil;
	}
}

- (void) setShowFullLocation:(BOOL)value
{
	showFullLocation = value;
	
	if (showFullLocation)
	{
		NSMutableString *location = [NSMutableString stringWithFormat:@"%@ %@", self.entity.entityDescriptor.devDesc, self.entity.entityDescriptor.cntDesc];
		if (![self.entity.entityDescriptor.siteName isEqualToString:@"default"]) [location appendFormat:@" @ %@", self.entity.entityDescriptor.siteDesc];
		
		self.detailTextLabel.text = location;
	}
	else 
	{
		self.detailTextLabel.text = nil;
	}
}

@end
