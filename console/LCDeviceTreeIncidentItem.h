//
//  LCDeviceTreeIncidentItem.h
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCIncident.h"
#import "LCDeviceTreeItem.h"

@interface LCDeviceTreeIncidentItem : LCDeviceTreeItem 
{
	LCIncident *incident;
}

- (id) initWithIncident:(LCIncident *)initIncident;
@property (retain) LCIncident *incident;

@end
