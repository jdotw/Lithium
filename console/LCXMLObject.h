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
@property (nonatomic,retain) id xmlEntity;
@property (nonatomic,copy) NSMutableDictionary *xmlTranslation;
@property (nonatomic, assign) BOOL xmlRefreshInProgress;
@property (nonatomic, assign) BOOL xmlAddInProgress;
@property (nonatomic, assign) BOOL xmlUpdateInProgress;
@property (nonatomic, assign) BOOL xmlDeleteInProgress;
@property (nonatomic, assign) BOOL xmlOperationInProgress;
@property (nonatomic, assign) BOOL xmlOperationSuccess;
@property (nonatomic,copy) NSString *xmlStatusString;
- (void) updateXmlInProgressFlag;
@property (nonatomic, assign) id delegate;
@property (nonatomic, assign) BOOL xmlUpdatingValues;
@property (nonatomic, assign) BOOL debug;
@property (nonatomic,copy) NSString *sortString;

@end
