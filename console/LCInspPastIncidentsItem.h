//
//  LCInspPastIncidentsItem.h
//  Lithium Console
//
//  Created by James Wilson on 15/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorItem.h"
#import "LCIncidentList.h"

@interface LCInspPastIncidentsItem : LCInspectorItem 
{
	LCIncidentList *incidentList;
}

@property (nonatomic,retain) LCIncidentList *incidentList;

@end
