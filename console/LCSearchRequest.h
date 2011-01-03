//
//  LCSearchRequest.h
//  Lithium Console
//
//  Created by James Wilson on 24/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#define SEARCH_ANY 0
#define SEARCH_ALL 1

@class LCCustomer, LCXMLRequest, LCSearchResult;

@interface LCSearchRequest : NSObject 
{
	LCCustomer *customer;		/* Customer resource */
	NSArray *keywords;			/* List of individual keywords */
	int searchOperator;			/* 0=Any 1=All */
	BOOL useRegex;
	int maximumType;			/* The highest type of integer to include in results */
	
	NSString *searchString;		/* The raw search string */	
	
	LCXMLRequest *xmlRequest;
	BOOL searchInProgress;
	
	NSMutableArray *results;
	NSMutableDictionary *resultDict;	/* Keyed by entity address string */
}

#pragma mark "XML Operations"
- (void) search;

#pragma mark "Properties"
@property (nonatomic,retain) LCCustomer *customer;
@property (nonatomic,copy) NSArray *keywords;
@property (nonatomic,assign) int searchOperator;
@property (nonatomic,assign) BOOL useRegex;
@property (nonatomic,assign) int maximumType;
@property (nonatomic,copy) NSString *searchString;
@property (nonatomic,assign) BOOL searchInProgress;
@property (nonatomic,readonly) NSMutableArray *results;
@property (nonatomic,readonly) NSMutableDictionary *resultDict;
- (void) insertObject:(LCSearchResult *)result inResultsAtIndex:(unsigned int)index;
- (void) removeObjectFromResultsAtIndex:(unsigned int)index;


@end
