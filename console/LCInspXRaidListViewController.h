//
//  LCInspXRaidListViewController.h
//  Lithium Console
//
//  Created by James Wilson on 17/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCInspXRaidMiniView.h"

@interface LCInspXRaidListViewController : LCInspectorViewController 
{
	IBOutlet LCInspXRaidMiniView *raidView;
}

- (LCInspXRaidListViewController *) initWithRaid1Arrays:(NSArray *)raid1Arrays raid2Arrays:(NSArray *)raid2Arrays metadataArrays:(NSArray *)metadataArrays;
- (void) setNumUnits:(int)num;
- (void) updateImages;

@end
