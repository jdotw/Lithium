//
//  LCInspRelatedIncidentsItem.h
//  Lithium Console
//
//  Created by James Wilson on 10/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorItem.h"
#import "LCIncidentList.h"

@interface LCInspRelatedIncidentsItem : LCInspectorItem 
{
	LCIncidentList *incidentList;
	
}

@property (nonatomic,retain) LCIncidentList *incidentList;

@end
