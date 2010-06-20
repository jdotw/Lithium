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
@property (assign) int entryID;
@property (assign) int caseID;
@property (assign) int type;
@property (copy) NSString *typeString;
@property (copy) NSString *author;
@property (copy) NSDate *timestamp;
@property (assign) long timestampSeconds;
@property (assign) long timespentSeconds;
@property (copy) NSString *entry;
@property (copy) NSString *displayString;
- (void) updateDisplayString;

@end
