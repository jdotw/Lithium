//
//  LCSearchRequest.h
//  Lithium Console
//
//  Created by James Wilson on 24/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"

#define SEARCH_ANY 0
#define SEARCH_ALL 1

@interface LCSearchRequest : NSObject 
{
	LCEntity *entity;			/* The entity to be queried */
	NSArray *keywords;			/* List of individual keywords */
	int searchOperator;			/* 0=Any 1=All */
	BOOL useRegex;
	int maximumType;			/* The highest type of integer to include in results */
	
	NSString *searchString;		/* The raw search string */	
	
	LCXMLRequest *xmlRequest;
	BOOL searchInProgress;
	
	NSMutableArray *results;
	NSMutableDictionary *resultDict;
}

#pragma mark "XML Operations"
- (void) search;

#pragma mark "Properties"
@property (retain) LCEntity *entity;
@property (copy) NSArray *keywords;
@property (assign) int searchOperator;
@property (assign) BOOL useRegex;
@property (assign) int maximumType;
@property (copy) NSString *searchString;
@property (assign) BOOL searchInProgress;
@property (readonly) NSMutableArray *results;
@property (readonly) NSMutableDictionary *resultDict;
- (void) insertObject:(LCEntity *)entity inResultsAtIndex:(unsigned int)index;
- (void) removeObjectFromResultsAtIndex:(unsigned int)index;


@end
