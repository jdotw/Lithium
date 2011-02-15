//
//  LTDeviceEntityTableViewCell.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 4/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTHardwareEntityTableViewCell.h"

@interface LTDeviceEntityTableViewCell : LTHardwareEntityTableViewCell 
{
	UIImageView *topLeftVentInlay;
	UIImageView *topLeftVentMesh;
	UIImageView *topRightVentInlay;
	UIImageView *topRightVentMesh;
	UIImageView *bottomLeftVentInlay;
	UIImageView *bottomLeftVentMesh;
    UIImageView *bottomLeftVentGlow;
	UIImageView *bottomRightVentInlay;
	UIImageView *bottomRightVentMesh;
    UIImageView *bottomRightVentGlow;
}

@end
