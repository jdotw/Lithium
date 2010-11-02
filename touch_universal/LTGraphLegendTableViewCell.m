//
//  LTGraphLegendTableViewCell.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 8/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTGraphLegendTableViewCell.h"
#import "LTGraphLegendEntityView.h"
#import "LTEntity.h"

@implementation LTGraphLegendTableViewCell

@synthesize entities;

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier 
{
    if ((self = [super initWithStyle:style reuseIdentifier:reuseIdentifier])) 
	{
        // Initialization code
		entityViews = [[NSMutableArray array] retain];
		entityViewDict = [[NSMutableDictionary dictionary] retain];
    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
    [super setSelected:selected animated:animated];
    // Configure the view for the selected state
}


- (void)dealloc 
{
	[entities release];
	[entityViews release];
	[entityViewDict release];
    [super dealloc];
}

- (void) setEntities:(NSArray *)value
{
	[entities release];
	entities = [value retain];
	
	/* Re-provision the underlying views */
	for (LTGraphLegendEntityView *entityView in entityViews)
	{
		[entityView removeFromSuperview];
	}
	[entityViews removeAllObjects];
	[entityViewDict removeAllObjects];
	for (LTEntity *entity in entities)
	{
		LTGraphLegendEntityView *entityView = [[LTGraphLegendEntityView alloc] initWithFrame:CGRectZero];
		entityView.entity = entity;
		[self addSubview:entityView];
		[entityViews addObject:entityView];
		[entityViewDict setObject:entityView forKey:entity.entityAddress];
		[entityView release];
	}
	[self setNeedsLayout];
}

- (void) setSwatchColor:(UIColor *)color forEntityAtIndex:(unsigned int)index
{
	if (index < entityViews.count)
	{
		LTGraphLegendEntityView *entityView = [entityViews objectAtIndex:index];
		entityView.swatchColor = color;
	}
}

- (void) layoutSubviews
{
	CGFloat viewWidth = CGRectGetWidth(self.bounds) / (CGFloat) entities.count;
	for (LTGraphLegendEntityView *entityView in entityViews)
	{
		entityView.frame = CGRectMake(CGRectGetMinX(self.bounds) + ([entityViews indexOfObject:entityView] * viewWidth), 
									  CGRectGetMinY(self.bounds), 
									  viewWidth, 
									  CGRectGetHeight(self.bounds));
		[entityView setNeedsLayout];
	}
}

- (LTGraphLegendEntityView *) viewForEntity:(LTEntity *)entity
{
	return [entityViewDict objectForKey:entity.entityAddress];
}

@end
