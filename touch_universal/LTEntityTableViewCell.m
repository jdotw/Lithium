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

@synthesize entity, showCurrentValue, showFullLocation, drawAsRack=_drawAsRack;

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];

	return self;
}

- (void)dealloc 
{
	[entity release];
	[self.detailTextLabel release];
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

- (LTEntity *) valueMetricForEntity:(LTEntity *)parentEntity
{
    /* Returns the 'best' or most appropriate metric entity
     * that can be used to show a value for the given entity
     */
    if (parentEntity.type == 6) return parentEntity;
    
    /* Locate an object */
    LTEntity *obj = nil;
    if (parentEntity.type == 4 && parentEntity.children.count == 1) 
    { obj = [parentEntity.children objectAtIndex:0]; }
    else if (parentEntity.type == 5) obj = parentEntity;
    
    /* Iterate through metrics */
    LTEntity *valueMetric = nil;
    for (LTEntity *metric in obj.children)
    {
        /* Prefer metrics that are a percentage, and have triggers */
        if (metric.hasTriggers && metric.isPercentage) 
        {
            /* Best match found */
            valueMetric = metric;
            break;
        }
        else if (!valueMetric && metric.isPercentage)
        {
            /* First percentage */
            valueMetric = metric;
            break;
        }
        else if (!valueMetric && metric.hasTriggers)
        {
            /* First non-percentage with triggers */
            valueMetric = metric;
            break;
        }
    }
    return valueMetric;
}

- (void) setEntity:(LTEntity *)value
{
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
	if ([self valueMetricForEntity:value])
	{
		/* And set showCurrentValue to YES by default */
		self.showCurrentValue = YES;
	}
}

- (void) updateCurrentValue:(NSNotification *)note
{
	self.detailTextLabel.text = [self valueMetricForEntity:self.entity].currentValue;
    if (self.detailTextLabel.text)
    {
        self.detailTextLabel.text = [self.detailTextLabel.text stringByReplacingOccurrencesOfString:@"bits" withString:@"b"];
        self.detailTextLabel.text = [self.detailTextLabel.text stringByReplacingOccurrencesOfString:@"bytes" withString:@"B"];
        self.detailTextLabel.text = [self.detailTextLabel.text stringByReplacingOccurrencesOfString:@"bit" withString:@"b"];
        self.detailTextLabel.text = [self.detailTextLabel.text stringByReplacingOccurrencesOfString:@"byte" withString:@"B"];
        self.detailTextLabel.text = [self.detailTextLabel.text stringByReplacingOccurrencesOfString:@"writes" withString:@"wr"];
        self.detailTextLabel.text = [self.detailTextLabel.text stringByReplacingOccurrencesOfString:@"reads" withString:@"rd"];
        self.detailTextLabel.text = [self.detailTextLabel.text stringByReplacingOccurrencesOfString:@"sec" withString:@"s"];
    }
}

- (void) setShowCurrentValue:(BOOL)value
{
	showCurrentValue = value;
	[self updateCurrentValue:nil];
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
