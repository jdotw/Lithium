//
//  LTHardwareEntityTableViewCell.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 4/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

/* This is the base cell used to represent a hardware element
 * including devices, containers, objects and metrics
 *
 * The view renders a gradient background, screws and the LED status indicator
 */

#import <UIKit/UIKit.h>

#import "LTEntityTableViewCell.h"

@interface LTHardwareEntityTableViewCell : LTEntityTableViewCell 
{
	/* Screws */
	UIImageView *topLeftScrewImageView;
	UIImageView *topRightScrewImageView;
	UIImageView *bottomLeftScrewImageView;
	UIImageView *bottomRightScrewImageView;
    
    /* Sparkle */
    NSTimer *sparkleTimer;
}

- (void) sparkle:(NSTimer *)timer;

@end
