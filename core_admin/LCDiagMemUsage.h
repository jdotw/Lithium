//
//  LCDiagMemUsage.h
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDiagTest.h"

@interface LCDiagMemUsage : LCDiagTest 
{
	NSTimer *refreshTimer;
	int runCount;
	float groupMemAverage;
	NSMutableArray *highList;	
}

@end
