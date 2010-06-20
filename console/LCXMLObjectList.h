//
//  LCXMLObjectList.h
//  Lithium Console
//
//  Created by James Wilson on 11/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"

@interface LCXMLObjectList : NSObject 
{
	/* Source */
	LCEntity *source;
	NSString *xmlName;
	
	/* Configuration */
	Class objectClass;
	NSString *indexKey;				/* The property name */
	NSString *xmlIndexKey;			/* The XML element name */
	BOOL allowCancelOfExistingRefresh;
	
	/* Objects */
	NSMutableArray *objects;
	NSMutableDictionary *objectDict;
	
	/* XML Operation */
	LCXMLRequest *refreshXMLRequest;
	BOOL refreshInProgress;
}

#pragma mark "Constructors"
- (id) initWithSource:(LCEntity *)initSource
			  xmlName:(NSString *)initXmlName
			 forClass:(Class)initClass
		usingIndexKey:(NSString *)initKey;

#pragma mark "XML Document Creation (to be overridden by sub-classes)"
- (NSXMLDocument *) criteriaXmlDocument;
- (NSXMLElement *) criteriaXmlNode;

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;
- (void) xmlParserDidFinish:(LCXMLNode *)rootNode;
- (void) XMLRequestFinished:(LCXMLRequest *)sender;

#pragma mark "Object Properties"
@property (readonly) NSMutableArray *objects;
- (void) insertObject:(id)obj inObjectsAtIndex:(unsigned int)index;
- (void) removeObjectFromObjectsAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *objectDict;

#pragma mark "Properties"
@property (retain) LCEntity *source;
@property (copy) NSString *xmlName;
@property (assign) Class objectClass;
@property (copy) NSString *indexKey;
@property (copy) NSString *xmlIndexKey;
@property (assign) BOOL refreshInProgress;
@property (assign) BOOL allowCancelOfExistingRefresh;

@end
