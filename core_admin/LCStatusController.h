//
//  LCAdminToolsController.h
//  LCAdminTools
//
//  Created by Liam Elliott on 18/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "LCProcessList.h"

@interface LCStatusController : NSObject 
{
	LCProcessList *procList;
	NSTimer *refreshTimer;
}

-(void)awakeFromNib;
-(void)dealloc;

-(IBAction) startLithium:(id)sender;
-(IBAction) stopLithium:(id)sender;

-(IBAction) startPostgres:(id)sender;
-(IBAction) stopPostgres:(id)sender;

-(void)refreshList:(NSTimer *)timer;

-(LCProcessList *)procList;
-(void)setProcList:(LCProcessList *)newList;

-(IBAction) startApache:(id)sender;
-(IBAction) stopApache:(id)sender;

@end
