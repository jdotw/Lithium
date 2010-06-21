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
@property (nonatomic,copy) NSString *uuid;
@property (nonatomic,copy) NSColor *colour;
- (NSColor *) colourWithAlpha:(float)alpha;
- (NSColor *) opStateColourWithAlpha:(float)alpha;

#pragma mark "User-entered Notes"
@property (nonatomic,copy) NSString *vlans;
@property (nonatomic,copy) NSString *notes;

#pragma mark Accessors
@property (nonatomic,retain) LCEntity *aEndDevice;
@property (nonatomic,retain) LCEntity *aEndInterface;
@property (nonatomic,retain) LCEntity *bEndDevice;
@property (nonatomic,retain) LCEntity *bEndInterface;
@property (nonatomic, assign) BOOL highlighted;
@property (nonatomic,retain) LCVRackCableGroup *cableGroup;
@property (nonatomic, assign) BOOL locked;
@property (nonatomic, assign) BOOL orphaned;
@property (nonatomic, assign) float abSpeedIndicatorT;
- (void) incrementAbSpeedIndicatorT;
@property (nonatomic, assign) int abSpeedIndex;
@property (nonatomic, assign) float baSpeedIndicatorT;
- (void) incrementBaSpeedIndicatorT;
@property (nonatomic, assign) int baSpeedIndex;

@end
