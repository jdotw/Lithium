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
	valueLabel.text = [self valueMetricForEntity:self.entity].currentValue;
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bits" withString:@"b"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bytes" withString:@"B"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bit" withString:@"b"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"byte" withString:@"B"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"writes" withString:@"wr"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"reads" withString:@"rd"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"sec" withString:@"s"];
	
	CGSize stringSize = [valueLabel.text sizeWithFont:valueLabel.font];
	valueLabel.frame = CGRectMake(0., 0., stringSize.width, stringSize.height);	
}

- (void) setShowCurrentValue:(BOOL)value
{
	showCurrentValue = value;
	
	if (showCurrentValue)
	{
		self.accessoryView = valueLabel;
		[self updateCurrentValue:nil];
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

- (void)layoutSubviews
{
    /* Call super-class layout to handle textLabel and detailTextLabel layout */
    [super layoutSubviews];
    
    /* Then, if necessary, adjust layout to fit in the current value */
    if (self.showCurrentValue)
    {
        CGRect labelRect = self.textLabel.frame;
        labelRect.size.width *= 0.7;  // 70/30 split between label and value 
        self.textLabel.frame = labelRect;
        
        CGRect detailRect = self.detailTextLabel.frame;
        
        
        CGRect valueRect = CGRectMake(CGRectGetMinX(self.textLabel.frame)+(CGRectGetWidth(self.textLabel.frame)*0, <#CGFloat y#>, <#CGFloat width#>, <#CGFloat height#>)
    }
}

@end
