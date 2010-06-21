//
//  LCCaseLogEntry.h
//  Lithium Console
//
//  Created by James Wilson on 25/07/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCAuthenticator.h"
#import "LCXMLObject.h"

@interface LCCaseLogEntry : LCXMLObject 
{
	/* Properties */
	int entryID;
	int caseID;
	int type;
	NSString *typeString;
	NSString *author;
	NSDate *timestamp;
	long timestampSeconds;
	long timespentSeconds;
	NSString *entry;
	
	/* Dynamic Properties */
	NSString *displayString;

	/* Op variables */
	LCXMLRequest *recordXMLReq;
}

#pragma mark "Initialisation"
- (LCCaseLogEntry *) initWithString:(NSString *)text cas:(id)cas timespent:(time_t)inittimespent;

#pragma mark "Recording Methods"
- (void) record:(id)cas;

#pragma mark "Accessor Methods"
@property (nonatomic, assign) int entryID;
@property (nonatomic, assign) int caseID;
@property (nonatomic, assign) int type;
@property (nonatomic,copy) NSString *typeString;
@property (nonatomic,copy) NSString *author;
@property (nonatomic,copy) NSDate *timestamp;
@property (nonatomic, assign) long timestampSeconds;
@property (nonatomic, assign) long timespentSeconds;
@property (nonatomic,copy) NSString *entry;
@property (nonatomic,copy) NSString *displayString;
- (void) updateDisplayString;

@end
