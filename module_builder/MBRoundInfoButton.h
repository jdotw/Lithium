//
//  MBRoundInfoButton.h
//  ModuleBuilder
//
//  Created by James Wilson on 8/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBRoundButton.h"

#import "MBOutlineViewController.h"

@interface MBRoundInfoButton : MBRoundButton 
{
	IBOutlet MBOutlineViewController *viewController;
}

@end
