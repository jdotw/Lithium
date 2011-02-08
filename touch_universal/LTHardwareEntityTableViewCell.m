//
//  LTHardwareEntityTableViewCell.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 4/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTHardwareEntityTableViewCell.h"


@implementation LTHardwareEntityTableViewCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) 
	{
        // Initialization code.
		self.drawEntityStateBackgroundColor = NO;	// Don't show entity color in background
		
		/* Screw Indicators */
		topLeftScrewImageView = [[UIImageView alloc] initWithFrame:CGRectZero];
		topLeftScrewImageView.image = [UIImage imageNamed:@"ScrewHole.png"];
		[self addSubview:topLeftScrewImageView];
		topRightScrewImageView = [[UIImageView alloc] initWithFrame:CGRectZero];
		topRightScrewImageView.image = [UIImage imageNamed:@"ScrewHole.png"];
		[self addSubview:topRightScrewImageView];
		bottomLeftScrewImageView = [[UIImageView alloc] initWithFrame:CGRectZero];
		bottomLeftScrewImageView.image = [UIImage imageNamed:@"ScrewHole.png"];
		[self addSubview:bottomLeftScrewImageView];
		bottomRightScrewImageView = [[UIImageView alloc] initWithFrame:CGRectZero];
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

- (void)dealloc {
    [super dealloc];
}

- (void) WRONGlayoutSubviews
{
	[super layoutSubviews];

	/* 
	 * Layout screws -- Image is always 9x9 
	 */
	CGRect screwRect = {{0., 0.}, {9., 9.}};
	CGFloat xOffset = 3.;
	CGFloat yOffset = 3.;
	
	/* Top Left */
	screwRect.origin.x = xOffset;
	screwRect.origin.y = yOffset;
	topLeftScrewImageView.frame = screwRect;

	/* Top Right */
	screwRect.origin.x = CGRectGetMaxX(self.bounds)-xOffset-screwRect.size.width;
	screwRect.origin.y = yOffset;
	topRightScrewImageView.frame = screwRect;

	/* Bottom Left */
	screwRect.origin.x = xOffset;
	screwRect.origin.y = CGRectGetMaxY(self.bounds)-yOffset-screwRect.size.height-1.0;
	bottomLeftScrewImageView.frame = screwRect;

	/* Bottom Right */
	screwRect.origin.x = CGRectGetMaxX(self.bounds)-xOffset-screwRect.size.width;
	screwRect.origin.y = CGRectGetMaxY(self.bounds)-yOffset-screwRect.size.height-1.0;
	bottomRightScrewImageView.frame = screwRect;
}

- (void) setEntityState:(int)state
{
	[super setEntityState:state];
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

- (void)setDrawAsRack:(BOOL)value
{
    [super setDrawAsRack:value];
    topLeftScrewImageView.hidden = !value;
    topRightScrewImageView.hidden = !value;
    bottomLeftScrewImageView.hidden = !value;
    bottomRightScrewImageView.hidden = !value;
}

@end
