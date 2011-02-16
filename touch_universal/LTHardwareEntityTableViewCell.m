//
//  LTHardwareEntityTableViewCell.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 4/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTHardwareEntityTableViewCell.h"
#import "LTEntity.h"
#import "LTSubDeviceTableViewCellBackgroundView.h"

@implementation LTHardwareEntityTableViewCell

- (void) sparkle:(NSTimer *)timer
{
    CGFloat alpha = 1.0 - ((float)(arc4random()%15)/100.);
    self.imageView.alpha = alpha;
}

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) 
	{
        // Initialization code.
		self.drawEntityStateBackgroundColor = NO;	// Don't show entity color in background
		
		/* Screw Indicators */
		topLeftScrewImageView = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		topLeftScrewImageView.image = [UIImage imageNamed:@"ScrewHole.png"];
		[self addSubview:topLeftScrewImageView];
		topRightScrewImageView = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		topRightScrewImageView.image = [UIImage imageNamed:@"ScrewHole.png"];
		[self addSubview:topRightScrewImageView];
		bottomLeftScrewImageView = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		bottomLeftScrewImageView.image = [UIImage imageNamed:@"ScrewHole.png"];
		[self addSubview:bottomLeftScrewImageView];
		bottomRightScrewImageView = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		bottomRightScrewImageView.image = [UIImage imageNamed:@"ScrewHole.png"];
		[self addSubview:bottomRightScrewImageView];
        
        self.detailTextLabel.text = nil;
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    
    [super setSelected:selected animated:animated];
    
    // Configure the view for the selected state.
}

- (void) layoutSubviews
{
	[super layoutSubviews];

	/* 
	 * Layout screws -- Image is always 9x9 
	 */
	CGRect screwRect = {{0., 0.}, {8., 8.}};
	CGFloat xOffset = 5.;
	CGFloat yOffset = 4.;
	
	/* Top Left */
	screwRect.origin.x = xOffset;
	screwRect.origin.y = yOffset;
	topLeftScrewImageView.frame = screwRect;
//    topLeftScrewImageView.alpha = 0.6;

	/* Top Right */
	screwRect.origin.x = CGRectGetMaxX(self.bounds)-xOffset-screwRect.size.width;
	screwRect.origin.y = yOffset;
	topRightScrewImageView.frame = screwRect;
//    topRightScrewImageView.alpha = 0.6;

	/* Bottom Left */
	screwRect.origin.x = xOffset;
	screwRect.origin.y = CGRectGetMaxY(self.bounds)-yOffset-screwRect.size.height-1.0;
	bottomLeftScrewImageView.frame = screwRect;
//    bottomLeftScrewImageView.alpha = 0.6;

	/* Bottom Right */
	screwRect.origin.x = CGRectGetMaxX(self.bounds)-xOffset-screwRect.size.width;
	screwRect.origin.y = CGRectGetMaxY(self.bounds)-yOffset-screwRect.size.height-1.0;
	bottomRightScrewImageView.frame = screwRect;
//    bottomRightScrewImageView.alpha = 0.6;
    
    if (self.entity.type > 0)
    {
        /* Layout LED and Label 
         * 
         * By default, the LED and Label are too far to the right, 
         * we bring them in further left here 
         */
        
        CGFloat ledXOffset = 2. + (self.indentationLevel * self.indentationWidth);
        CGRect ledRect = self.imageView.frame;
        ledRect.origin.x = ledXOffset;
        self.imageView.frame = ledRect;
        
        CGFloat labelXOffset = CGRectGetMaxX(ledRect) + 0.;
        CGRect labelRect = self.textLabel.frame;
        labelRect.origin.x = labelXOffset;
        self.textLabel.frame = labelRect;    
        
        CGRect locationRect = self.locationLabel.frame;
        locationRect.origin.x = labelXOffset;
        self.locationLabel.frame = locationRect;
    }
    else
    {
        /* Core Layout -- Shift labels away from screws */
        CGRect labelRect = self.textLabel.frame;
        labelRect.origin.x = 20.;
        self.textLabel.frame = labelRect;

        CGRect detailLabelRect = self.detailTextLabel.frame;
        detailLabelRect.origin.x = labelRect.origin.x;
        detailLabelRect.origin.y -= 1.;
        self.detailTextLabel.frame = detailLabelRect;
    }
}

- (void) setEntity:(LTEntity *)value
{
    [super setEntity:value];
    
    if (value.type > ENT_DEVICE)
    {
        /* Configure sub-device (flat) cell style */
        self.backgroundView = [[LTSubDeviceTableViewCellBackgroundView alloc] initWithFrame:CGRectZero];
        self.textLabel.font = [UIFont boldSystemFontOfSize:14.0];
        self.textLabel.shadowColor = [UIColor blackColor];
        self.textLabel.shadowOffset = CGSizeMake(0.0, -1.0);
        self.textLabel.minimumFontSize = 12.0;
        self.textLabel.adjustsFontSizeToFitWidth = YES;
        self.detailTextLabel.font = [UIFont systemFontOfSize:14.0];
        self.detailTextLabel.shadowColor = [UIColor blackColor];
        self.detailTextLabel.shadowOffset = CGSizeMake(0.0, -1.0);
        self.detailTextLabel.minimumFontSize = 12.0;
        self.detailTextLabel.adjustsFontSizeToFitWidth = YES;        
    }
}

- (void) setEntityState:(int)state
{
	[super setEntityState:state];
    if (self.entity.type > 0)
    {
        UIImage *ledImage = nil;
        switch (self.entityState)
        {
            case 3:
                ledImage = [UIImage imageNamed:@"LED-Red.png"];
                break;
            case 2:
                ledImage = [UIImage imageNamed:@"LED-Orange.png"];
                break;
            case 1:
                ledImage = [UIImage imageNamed:@"LED-Yellow.png"];
                break;
            case 0:
                ledImage = [UIImage imageNamed:@"LED-Green.png"];
                break;
            case -2:
                ledImage = [UIImage imageNamed:@"LED-Blue.png"];
                break;
            default:
                ledImage = [UIImage imageNamed:@"LED-Off.png"];
                break;
        }
        self.imageView.image = ledImage;    
    }
}

- (void)setDrawAsRack:(BOOL)value
{
    [super setDrawAsRack:value];
    topLeftScrewImageView.hidden = !value;
    topRightScrewImageView.hidden = !value;
    bottomLeftScrewImageView.hidden = !value;
    bottomRightScrewImageView.hidden = !value;
}

@end
