//
//  LCVRackCablePopupView.h
//  Lithium Console
//
//  Created by James Wilson on 3/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCPopupView.h"
#import "LCShadowTextField.h"

@interface LCVRackCablePopupView : LCPopupView 
{
	LCShadowTextField *aEndDevLabel;
	LCShadowTextField *aEndIntLabel;
	LCShadowTextField *bEndDevLabel;
	LCShadowTextField *bEndIntLabel;
}

@property (readonly) LCShadowTextField *aEndDevLabel;
@property (readonly) LCShadowTextField *aEndIntLabel;
@property (readonly) LCShadowTextField *bEndDevLabel;
@property (readonly) LCShadowTextField *bEndIntLabel;


@end
