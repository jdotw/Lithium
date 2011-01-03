//
//  LCSearchSourceItem.h
//  Lithium Console
//
//  Created by James Wilson on 25/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class LCCustomer, LCSearchResult;

@interface LCSearchSourceItem : NSObject 
{
	LCCustomer *customer;
	NSArray *searchRequests;
	
	NSString *searchString;
	BOOL searchInProgress;
	
	NSString *displayString;	
	
	NSMutableArray *results;
}

#pragma mark "Search Methods"
- (void) search;

#pragma mark "Properties"
@property (nonatomic,retain) LCCustomer *customer;
@property (nonatomic,copy) NSArray *searchRequests;
@property (nonatomic,copy) NSString *displayString;
@property (nonatomic,copy) NSString *searchString;
@property (nonatomic,readonly) NSMutableArray *results;
- (void) insertObject:(LCSearchResult *)result inResultsAtIndex:(unsigned int)index;
- (void) removeObjectFromResultsAtIndex:(unsigned int)index;

@end
