//
//  LTDeviceEntityTableViewCell.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 4/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTDeviceEntityTableViewCell.h"


@implementation LTDeviceEntityTableViewCell

- (void) sparkle:(NSTimer *)timer
{
    [super sparkle:timer];
    
    switch (arc4random()%30) 
    {
        case 1:
            bottomRightVentGlow.image = [UIImage imageNamed:@"DeviceVentRedGlow1.png"];
            break;        
        case 2:
            bottomRightVentGlow.image = [UIImage imageNamed:@"DeviceVentBlueGlow1.png"];
            break;
        case 3:
            bottomRightVentGlow.image = [UIImage imageNamed:@"DeviceVentBlueGlow2.png"];
            break;
        default:
            bottomRightVentGlow.image = nil;
            break;
    }

    switch (arc4random()%20) 
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            bottomLeftVentGlow.image = [UIImage imageNamed:@"DeviceVentRedGlow1.png"];
            break;        
        case 10:
            bottomLeftVentGlow.image = [UIImage imageNamed:@"DeviceVentBlueGlow2.png"];
            break;
        default:
            bottomLeftVentGlow.image = nil;
            break;
    }
}

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:UITableViewCellStyleDefault reuseIdentifier:reuseIdentifier];
    if (self) 
	{
		/* Create Vent Image Views */
        topLeftVentInlay = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		topLeftVentInlay.image = [UIImage imageNamed:@"DeviceVent.png"];
        topLeftVentInlay.alpha = 0.8;
		[self addSubview:topLeftVentInlay];
        topRightVentInlay = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		topRightVentInlay.image = [UIImage imageNamed:@"DeviceVent.png"];
        topRightVentInlay.alpha = 0.8;
		[self addSubview:topRightVentInlay];
        bottomLeftVentInlay = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		bottomLeftVentInlay.image = [UIImage imageNamed:@"DeviceVent.png"];
        bottomLeftVentInlay.alpha = 0.8;
		[self addSubview:bottomLeftVentInlay];
        bottomRightVentInlay = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		bottomRightVentInlay.image = [UIImage imageNamed:@"DeviceVent.png"];
        bottomRightVentInlay.alpha = 0.8;
		[self addSubview:bottomRightVentInlay];
		
		/* Create Vent Mesh Image Views */
        topLeftVentMesh = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		topLeftVentMesh.image = [UIImage imageNamed:@"DeviceVentMesh.png"];
		[self addSubview:topLeftVentMesh];
        topRightVentMesh = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		topRightVentMesh.image = [UIImage imageNamed:@"DeviceVentMesh.png"];
		[self addSubview:topRightVentMesh];
        bottomLeftVentMesh = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		bottomLeftVentMesh.image = [UIImage imageNamed:@"DeviceVentMesh.png"];
		[self addSubview:bottomLeftVentMesh];
        bottomRightVentMesh = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
		bottomRightVentMesh.image = [UIImage imageNamed:@"DeviceVentMesh.png"];
		[self addSubview:bottomRightVentMesh];
        
        /* Create Glow Image Views */
        bottomRightVentGlow = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
        bottomRightVentGlow.alpha = 0.3;
        [self insertSubview:bottomRightVentGlow belowSubview:bottomRightVentMesh];
        bottomLeftVentGlow = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
        bottomLeftVentGlow.alpha = 0.3;
        [self insertSubview:bottomLeftVentGlow belowSubview:bottomLeftVentMesh];

        /* Configure label */
        self.textLabel.font = [UIFont boldSystemFontOfSize:16.0];
        self.textLabel.textColor = [UIColor whiteColor];
        self.textLabel.shadowColor = [UIColor blackColor];
        self.textLabel.shadowOffset = CGSizeMake(0.0, -1.0);
        self.textLabel.minimumFontSize = 12.0;
        self.textLabel.adjustsFontSizeToFitWidth = YES;

    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    
    [super setSelected:selected animated:animated];
    
    // Configure the view for the selected state.
}


- (void)dealloc 
{
    [super dealloc];
}

- (void) layoutSubviews
{
	[super layoutSubviews];
	
	/* Layout Vents and Mesh -- Vents are always 82x13 */
	CGRect ventRect = {{0., 0.}, {82., 13.}};
	
	/* Top Left Vent and Mesh */
	ventRect.origin.y = 2.;
	ventRect.origin.x = 133.;
	topLeftVentInlay.frame = ventRect;
	topLeftVentMesh.frame = ventRect;

	/* Top Right Vent and Mesh */
	ventRect.origin.x += ventRect.size.width + 4.0;
	topRightVentInlay.frame = ventRect;
	topRightVentMesh.frame = ventRect;

	/* Bottom Left Vent and Mesh */
	ventRect.origin.y = CGRectGetMaxY(self.bounds)-ventRect.size.height-2.;
	ventRect.origin.x = 133.;
	bottomLeftVentInlay.frame = ventRect;
	bottomLeftVentMesh.frame = ventRect;
    bottomLeftVentGlow.frame = ventRect;
	
	/* Bottom Right Vent and Mesh */
	ventRect.origin.y = CGRectGetMaxY(self.bounds)-ventRect.size.height-2.;
	ventRect.origin.x += ventRect.size.width + 4.0;
	bottomRightVentInlay.frame = ventRect;
	bottomRightVentMesh.frame = ventRect;
    bottomRightVentGlow.frame = ventRect;
}


@end
