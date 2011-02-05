//
//  LTRackTableViewHeaderView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 5/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTRackTableViewHeaderView.h"
#import <QuartzCore/QuartzCore.h>

@implementation LTRackTableViewHeaderView

@synthesize  textLabel;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) 
    {
        /* Set background -- dark with bevel */
        self.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"RackHeaderViewBackTile.png"]];
        
        /* Add tape and label views with rotation */
        tapeImageView = [[UIImageView alloc] initWithFrame:CGRectZero];
        tapeImageView.image = [UIImage imageNamed:@"HeaderViewWhiteTape1.png"];
        [self addSubview:tapeImageView];
        textLabel = [[UILabel alloc] initWithFrame:CGRectZero];        
        textLabel.textColor = [[UIColor blueColor] colorWithAlphaComponent:0.8];
        textLabel.font = [UIFont fontWithName:@"Chalkduster" size:12.0];
        textLabel.backgroundColor = [UIColor clearColor];
        textLabel.transform = CGAffineTransformMakeRotation(-1. * M_PI / 180);
        [tapeImageView addSubview:textLabel];
        
        /* Add Screw ImageViews */
        tlScrew = [[UIImageView alloc] initWithFrame:CGRectZero];
        tlScrew.image = [UIImage imageNamed:@"DarkScrewHole.png"];
        [self addSubview:tlScrew];
        trScrew = [[UIImageView alloc] initWithFrame:CGRectZero];
        trScrew.image = [UIImage imageNamed:@"DarkScrewHole.png"];
        [self addSubview:trScrew];
        blScrew = [[UIImageView alloc] initWithFrame:CGRectZero];
        blScrew.image = [UIImage imageNamed:@"DarkScrewHole.png"];
        [self addSubview:blScrew];
        brScrew = [[UIImageView alloc] initWithFrame:CGRectZero];
        brScrew.image = [UIImage imageNamed:@"DarkScrewHole.png"];
        [self addSubview:brScrew];
    }
    return self;
}

- (void)layoutSubviews
{
    /* Layout the Tape Image -- Image is always 174x22 */
    CGRect tapeRect = {{18., 2.},{174.,22.}};
    tapeImageView.frame = tapeRect;
    
    /* Layout the label inside the tape image */
    CGFloat labelXOffset = 12.;
    CGRect labelRect = {{labelXOffset, 4.},{tapeRect.size.width-labelXOffset,14.}};
    textLabel.frame = labelRect;
    
    /* 
	 * Layout screws -- Image is always 9x9 
	 */
	CGRect screwRect = {{0., 0.}, {9., 9.}};
	CGFloat xOffset = 2.;
	CGFloat yOffset = 2.;
	
	/* Top Left */
	screwRect.origin.x = xOffset;
	screwRect.origin.y = yOffset;
	tlScrew.frame = screwRect;
    
	/* Top Right */
	screwRect.origin.x = CGRectGetMaxX(self.bounds)-xOffset-screwRect.size.width;
	screwRect.origin.y = yOffset;
	trScrew.frame = screwRect;
    
	/* Bottom Left */
	screwRect.origin.x = xOffset;
	screwRect.origin.y = CGRectGetMaxY(self.bounds)-yOffset-screwRect.size.height;
	blScrew.frame = screwRect;
    
	/* Bottom Right */
	screwRect.origin.x = CGRectGetMaxX(self.bounds)-xOffset-screwRect.size.width;
	screwRect.origin.y = CGRectGetMaxY(self.bounds)-yOffset-screwRect.size.height;
	brScrew.frame = screwRect;

}

- (void)dealloc
{
    [super dealloc];
}

@end
