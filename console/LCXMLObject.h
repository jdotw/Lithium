//
//  LCXMLObject.h
//  Lithium Console
//
//  Created by James Wilson on 6/10/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"
#import "LCXMLNode.h"

@interface LCXMLObject : NSObject <NSCopying, NSMutableCopying>
{
	id xmlEntity;				/* The host of the XML object */

	LCXMLRequest *refreshXmlRequest;
	LCXMLRequest *addXmlRequest;
	LCXMLRequest *updateXmlRequest;
	LCXMLRequest *deleteXmlRequest;
	NSMutableString *xmlString;
	
	BOOL xmlRefreshInProgress;
	BOOL xmlAddInProgress;
	BOOL xmlUpdateInProgress;
	BOOL xmlDeleteInProgress;
	
	BOOL xmlOperationInProgress;
	NSString *xmlStatusString;
	BOOL xmlOperationSuccess;
	
	BOOL xmlUpdatingValues;
	
	BOOL debug;
	
	NSMutableDictionary *valueTypeCache;
	
	id delegate;
	
	NSString *sortString;
}

#pragma mark "Sub-Class Methods and Properties"
- (NSString *) xmlRootElement;

#pragma mark "XML Property Setting"
+ (NSDictionary *) xmlTranslationForClass:(Class)class;
- (void) setXmlValueUsingProperties:(NSMutableDictionary *)valueProperties;
- (void) setXmlValuesUsingXmlNode:(LCXMLNode *)node;
- (void) setXmlValue:(id)value forKey:(NSString *)key;
- (void) copyXmlPropertiesToObject:(id)obj;
- (void) copyXmlPropertiesFromObject:(id)obj;
- (NSDictionary *) xmlPropertiesDictionary;

#pragma mark "XML Document Creation"
- (NSXMLDocument *) xmlDocument;
- (NSXMLElement *) xmlNode;

#pragma mark "XML Operations"
- (void) performXmlRefresh;
- (void) performXmlAdd;
- (void) performXmlUpdate;
- (void) performXmlDelete;

#pragma mark "XML Parser Delegate Methods"
- (void) xmlParserDidFinish:(LCXMLNode *)rootNode;
- (void) XMLRequestFinished:(LCXMLRequest *)sender;

#pragma mark "Property Methods"
@property (retain) id xmlEntity;
@property (copy) NSMutableDictionary *xmlTranslation;
@property (assign) BOOL xmlRefreshInProgress;
@property (assign) BOOL xmlAddInProgress;
@property (assign) BOOL xmlUpdateInProgress;
@property (assign) BOOL xmlDeleteInProgress;
@property (assign) BOOL xmlOperationInProgress;
@property (assign) BOOL xmlOperationSuccess;
@property (copy) NSString *xmlStatusString;
- (void) updateXmlInProgressFlag;
@property (assign) id delegate;
@property (assign) BOOL xmlUpdatingValues;
@property (assign) BOOL debug;
@property (copy) NSString *sortString;

@end
