//
//  LCCaseSearchController.h
//  Lithium Console
//
//  Created by James Wilson on 10/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomerList.h"
#import "LCCase.h"
#import "LCCaseList.h"
#import "LCViewTableView.h"

@interface LCCaseSearchController : NSWindowController 
{
	/* NIB Elements */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet LCViewTableView *tableView;
	
	/* Criteria */
	LCCustomer *customer;
	NSString *customerName;
	BOOL caseIDChecked;
	BOOL stateChecked;
	BOOL headlineChecked;
	BOOL ownerChecked;
	BOOL requesterChecked;
	BOOL openedBetweenChecked;
	BOOL closedBetweenChecked;
	int stateInteger;
	NSString *caseID;
	NSString *headline;
	NSString *requester;
	NSString *caseOwner;		/* Be careful: 'owner' is a property to the NSWindowController class */
	NSDate *startLower; 
	NSDate *startUpper;
	NSDate *endLower;
	NSDate *endUpper;
		
	/* Sorting */
	NSMutableArray *sortItems;
	NSArray *sortDescriptorArray;
	
	/* Case List */
	LCCaseList *caseList;
}

#pragma mark "UI Action Methods"
- (IBAction) searchClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;
- (IBAction) caseTableViewDoubleClicked:(NSArray *)selectedObjects;

#pragma mark "Accessor Methods"
@property (readonly) LCCustomerList *customerList;
@property (nonatomic, retain) LCCustomer *customer;
@property (assign) BOOL caseIDChecked;
@property (assign) BOOL stateChecked;
@property (assign) BOOL headlineChecked;
@property (assign) BOOL ownerChecked;
@property (assign) BOOL requesterChecked;
@property (assign) BOOL openedBetweenChecked;
@property (assign) BOOL closedBetweenChecked;
@property (readonly) NSMutableArray *sortItems;
@property (retain) NSArray *sortDescriptorArray;
@property (readonly) LCCaseList *caseList;

@property (nonatomic, assign) NSString *caseID;
@property (nonatomic, assign) int stateInteger;
@property (nonatomic, copy) NSString *headline;
@property (nonatomic, copy) NSString *requester;
@property (nonatomic, copy) NSString *caseOwner;
@property (nonatomic, copy) NSDate *startLower; 
@property (nonatomic, copy) NSDate *startUpper;
@property (nonatomic, copy) NSDate *endLower;
@property (nonatomic, copy) NSDate *endUpper;


@end
