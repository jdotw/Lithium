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
@property (nonatomic, assign) int documentID;
@property (nonatomic,copy) NSString *type;
@property (nonatomic,copy) NSString *desc; 
@property (nonatomic,copy) NSString *displayString;
@property (nonatomic, assign) int state;
@property (nonatomic,copy) NSString *editor;
@property (nonatomic, assign) int version; 
@property (nonatomic,copy) NSDate *refreshVersion;
@property (nonatomic, assign) id customer;
@property (nonatomic, assign) BOOL editing;


@end
