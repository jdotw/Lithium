//
//  LTDeviceEntityTableViewCell.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 4/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTDeviceEntityTableViewCell.h"


@implementation LTDeviceEntityTableViewCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:UITableViewCellStyleDefault reuseIdentifier:reuseIdentifier];
    if (self) 
	{
		/* Create Vent Image Views */
        topLeftVentInlay = [[UIImageView alloc] initWithFrame:CGRectZero];
		topLeftVentInlay.image = [UIImage imageNamed:@"DeviceVent.png"];
		[self addSubview:topLeftVentInlay];
        topRightVentInlay = [[UIImageView alloc] initWithFrame:CGRectZero];
		topRightVentInlay.image = [UIImage imageNamed:@"DeviceVent.png"];
		[self addSubview:topRightVentInlay];
        bottomLeftVentInlay = [[UIImageView alloc] initWithFrame:CGRectZero];
		bottomLeftVentInlay.image = [UIImage imageNamed:@"DeviceVent.png"];
		[self addSubview:bottomLeftVentInlay];
        bottomRightVentInlay = [[UIImageView alloc] initWithFrame:CGRectZero];
		bottomRightVentInlay.image = [UIImage imageNamed:@"DeviceVent.png"];
		[self addSubview:bottomRightVentInlay];
		
		/* Create Vent Mesh Image Views */
        topLeftVentMesh = [[UIImageView alloc] initWithFrame:CGRectZero];
		topLeftVentMesh.image = [UIImage imageNamed:@"DeviceVentMesh.png"];
		[self addSubview:topLeftVentMesh];
        topRightVentMesh = [[UIImageView alloc] initWithFrame:CGRectZero];
		topRightVentMesh.image = [UIImage imageNamed:@"DeviceVentMesh.png"];
		[self addSubview:topRightVentMesh];
        bottomLeftVentMesh = [[UIImageView alloc] initWithFrame:CGRectZero];
		bottomLeftVentMesh.image = [UIImage imageNamed:@"DeviceVentMesh.png"];
		[self addSubview:bottomLeftVentMesh];
        bottomRightVentMesh = [[UIImageView alloc] initWithFrame:CGRectZero];
		bottomRightVentMesh.image = [UIImage imageNamed:@"DeviceVentMesh.png"];
		[self addSubview:bottomRightVentMesh];
    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    
    [super setSelected:selected animated:animated];
    
    // Configure the view for the selected state.
}


- (void)dealloc 
{
	[topLeftVentInlay release];
	[topRightVentInlay release];
	[bottomLeftVentInlay release];
	[bottomRightVentInlay release];
	[topLeftVentMesh release];
	[topRightVentMesh release];
	[bottomLeftVentMesh release];
	[bottomRightVentMesh release];
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
	
	/* Bottom Right Vent and Mesh */
	ventRect.origin.y = CGRectGetMaxY(self.bounds)-ventRect.size.height-2.;
	ventRect.origin.x += ventRect.size.width + 4.0;
	bottomRightVentInlay.frame = ventRect;
	bottomRightVentMesh.frame = ventRect;
}


@end
