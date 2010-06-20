//
//  LCDocument.h
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCXMLObject.h"

@interface LCDocument : LCXMLObject 
{
	int documentID;
	NSString *type;
	NSString *desc;
	NSString *displayString;
	int state;
	NSString *editor;
	int version;
	
	NSDate *refreshVersion;	
	id customer;
	
	BOOL editing;			/* Set when the local client is editing */
}

#pragma mark "Constructors"
- (id) copyDocument;

#pragma mark "XML Operations"
- (void) getDocument;

#pragma mark "Properties"
@property (assign) int documentID;
@property (copy) NSString *type;
@property (copy) NSString *desc; 
@property (copy) NSString *displayString;
@property (assign) int state;
@property (copy) NSString *editor;
@property (assign) int version; 
@property (copy) NSDate *refreshVersion;
@property (assign) id customer;
@property (assign) BOOL editing;


@end
