//
//  LCVRackCable.h
//  Lithium Console
//
//  Created by James Wilson on 11/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCVRackInterface.h"
#import "LCVRackCableGroup.h"
#import "LCXMLObject.h"

@interface LCVRackCable : LCXMLObject 
{
	/* Properties */
	NSString *uuid;
	NSColor *colour;
	NSString *vlans;
	NSString *notes;
	LCEntity *aEndDevice;
	LCEntity *aEndInterface;
	LCEntity *bEndDevice;
	LCEntity *bEndInterface;
	BOOL highlighted;	
	BOOL locked;
	
	/* Cable group */
	LCVRackCableGroup *cableGroup;
	
	/* Flags */
	BOOL orphaned; 
	
	/* Speed indictator */
	float abSpeedIndicatorT;
	int abSpeedIndex;
	float baSpeedIndicatorT;
	int baSpeedIndex;
}

#pragma mark Constructors
- (id) initWithAEnd:(LCEntity *)initAEnd bEnd:(LCEntity *)initBEnd;

#pragma mark "Color"
@property (copy) NSString *uuid;
@property (copy) NSColor *colour;
- (NSColor *) colourWithAlpha:(float)alpha;
- (NSColor *) opStateColourWithAlpha:(float)alpha;

#pragma mark "User-entered Notes"
@property (copy) NSString *vlans;
@property (copy) NSString *notes;

#pragma mark Accessors
@property (retain) LCEntity *aEndDevice;
@property (retain) LCEntity *aEndInterface;
@property (retain) LCEntity *bEndDevice;
@property (retain) LCEntity *bEndInterface;
@property (assign) BOOL highlighted;
@property (retain) LCVRackCableGroup *cableGroup;
@property (assign) BOOL locked;
@property (assign) BOOL orphaned;
@property (assign) float abSpeedIndicatorT;
- (void) incrementAbSpeedIndicatorT;
@property (assign) int abSpeedIndex;
@property (assign) float baSpeedIndicatorT;
- (void) incrementBaSpeedIndicatorT;
@property (assign) int baSpeedIndex;

@end
