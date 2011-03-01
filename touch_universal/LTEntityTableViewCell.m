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

@synthesize entity, drawAsRack=_drawAsRack, showLocation, showAlias;

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];

	return self;
}

- (void)dealloc 
{
	[entity release];
    [super dealloc];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void) layoutSubviews
{
    [super layoutSubviews];
    
    /* Location */
    if (self.showLocation)
    {
        /* Shift text label up */
        CGRect textRect = self.textLabel.frame;
        textRect.origin.y -= 12.0;
        self.textLabel.frame = textRect;
        
        CGRect locationRect = textRect;
        locationRect.origin.y += 20.0;
        locationRect.size.width = CGRectGetWidth(self.bounds)-(2.*CGRectGetMinX(locationRect));
        self.locationLabel.frame = locationRect;
    }
    else
    {
        self.locationLabel.frame = CGRectZero;
    }
}

- (void) prepareForReuse
{
	self.entityState = 0;
}

- (void) entityStateChanged:(NSNotification *)note
{
	if (self.entity.type > 0)
	{ self.entityState = entity.opState; }
}

- (void) entityValueChanged:(NSNotification *)note
{
}

- (void) setEntity:(LTEntity *)value
{
    /* Remove old observers */
    if (entity)
    {
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kLTEntityStateChanged object:entity];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kLTEntityValueChanged object:entity];
    }

	/* Set entity */
	[entity release];
	entity = [value retain];
	
	/* Set main label (entity description) */
	NSString *desc = self.entity.desc;
	self.textLabel.text = desc;
	
	/* Trigger a status update */
	[self entityStateChanged:nil];
    
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

- (void) setShowLocation:(BOOL)value
{
    showLocation = value;
    
    self.locationLabel.text = self.entity.longLocationString;
    
    [self setNeedsLayout];
}

- (void) setShowAlias:(BOOL)value
{
    showLocation = YES;
    self.locationLabel.text = self.entity.alias;
    [self setNeedsLayout];
}

@end
