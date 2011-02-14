//
//  LTAuthenticationTableViewController.h
//  Lithium
//
//  Created by James Wilson on 30/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntity.h"
#import "LTCustomer.h"
#import "LTTableViewController.h"

@interface LTAuthenticationTableViewController : LTTableViewController 
{
	UITextField *usernameTextField;
	UITextField *passwordTextField;
	UISwitch *rememberSwitch;
	
	LTCustomer *curCustomer;
	NSMutableArray *connections;
	BOOL authRequestInProgress;
}

- (UITextField *)createTextField;
- (UISwitch *) createSwitchControl;

- (void) requestAuthForCustomer:(LTEntity *)customer 
						 entity:(LTEntity *)entity 
					  challenge:(NSURLAuthenticationChallenge *)challenge 
					 connection:(NSURLConnection *)connection 
		   previousFailureCount:(int)previousFailureCount;
- (void) forgetCredentialsForCustomer:(LTEntity *)forgetCustomer;

- (void) performNextRequest;

@end
