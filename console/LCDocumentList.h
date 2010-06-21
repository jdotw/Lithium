//
//  LCDocumentList.h
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDocument.h"
#import "LCXMLRequest.h"

@interface LCDocumentList : NSObject 
{
	/* Customer */
	id customer;
	
	/* Documents */
	NSMutableArray *documents;
	NSMutableDictionary *documentDictionary;
	
	/* XML Variables */
	LCXMLRequest *refreshXMLRequest;
	BOOL refreshInProgress;
}

#pragma mark "Constructors"
- (id) initWithCustomer:(id)initCustomer;

#pragma mark "Refresh"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Properties"
@property (nonatomic,retain) id customer;
@property (readonly) NSMutableArray *documents;
- (void) insertObject:(LCDocument *)entity inDocumentsAtIndex:(unsigned int)index;
- (void) removeObjectFromDocumentsAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *documentDictionary;
@property (nonatomic, assign) BOOL refreshInProgress;

@end
