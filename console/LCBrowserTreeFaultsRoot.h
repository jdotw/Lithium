//
//  LCBrowserTreeFaultsRoot.h
//  Lithium Console
//
//  Created by James Wilson on 26/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserTreeItemRoot.h"
#import "LCBrowserTreeIncidents.h"
#import "LCBrowserTreeCases.h"

@interface LCBrowserTreeFaultsRoot : LCBrowserTreeItemRoot 
{
	LCBrowserTreeIncidents *incidents;
	LCBrowserTreeCases *cases;
}

@property (retain) LCBrowserTreeIncidents *incidents;
@property (retain) LCBrowserTreeCases *cases;


@end
