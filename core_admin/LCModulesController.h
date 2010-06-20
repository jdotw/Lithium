//
//  LCModulesController.h
//  LCAdminTools
//
//  Created by James Wilson on 15/03/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCModulesController : NSObject 
{
	NSArray *_modules;
	IBOutlet NSArrayController *moduleArrayController;
	IBOutlet NSWindow *windowForSheet;
}

- (IBAction) refreshModules:(id)sender;
- (IBAction) installModuleClicked:(id)sender;
- (IBAction) deleteModuleClicked:(id)sender;

@property (nonatomic, copy) NSArray *modules;

@end
