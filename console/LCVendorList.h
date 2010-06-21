//
//  LCVendorList.h
//  Lithium Console
//
//  Created by James Wilson on 7/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"

@interface LCVendorList : NSObject 
{
	/* Related Objects */
	id customer;
	
	/* Vendors */
	NSMutableArray *vendors;
	NSMutableDictionary *vendorDictionary;
	
	/* Refresh variables */
	LCXMLRequest *refreshXMLRequest;
	NSMutableDictionary *curXMLDictionary;
	NSMutableString *curXMLString;
	BOOL refreshInProgress;		
}

#pragma mark Constructors
+ (LCVendorList *) vendorListWithCustomer:(id)initCustomer;
- (LCVendorList *) initWithCustomer:(id)initCustomer;
- (LCVendorList *) init;
- (void) dealloc;

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) XMLRequestPreParse:(id)sender;
- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict ;
- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string;
- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname;
- (void) XMLRequestFinished:(LCXMLRequest *)sender;

#pragma mark "Vendor Accessor Methods"
- (NSArray *) vendors;
- (void) insertObject:(id)vendor inVendorsAtIndex:(unsigned int)index;
- (void) removeObjectFromVendorsAtIndex:(unsigned int) index;
- (NSDictionary *) vendorDictionary;

#pragma mark "General Accessor Methods"
@property (nonatomic, assign) BOOL refreshInProgress;

@end
