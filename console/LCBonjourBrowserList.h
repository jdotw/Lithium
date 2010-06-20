//
//  LCBonjourBrowserList.h
//  Lithium Console
//
//  Created by James Wilson on 30/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"
#import "LCEntityDescriptor.h"
#import "LCBonjourBrowser.h"
#import "LCBonjourService.h"

@interface LCBonjourBrowserList : NSObject 
{
	/* Customer */
	id customer;
	NSString *displayString;
	
	/* Delegate */
	id delegate;
	
	/* Volumes */
	NSMutableArray *children;
	NSMutableDictionary *childDictionary;	
	
	/* XML Variables */
	LCXMLRequest *refreshXMLRequest;
	LCBonjourBrowser *curBrowser;
	LCBonjourService *curService;
	NSMutableString *curXMLString;
	BOOL refreshInProgress;		
}

#pragma mark "Constructors"
+ (LCBonjourBrowserList *) bonjourBrowserListForCustomer:(id)initCustomer;
- (id) initForCustomer:(id)initCustomer;

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Properties"
@property (assign) id delegate;
@property (retain) id customer;
@property (copy) NSString *displayString;
- (void) updateDisplayString;
@property (readonly) NSMutableArray *children;
- (void) insertObject:(LCBonjourBrowser *)browser inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *childDictionary;
@property (assign) BOOL refreshInProgress;
- (NSString *) uniqueIdentifier;

@end
