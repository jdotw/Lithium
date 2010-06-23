//
//  MBContainerViewController.h
//  ModuleBuilder
//
//  Created by James Wilson on 7/09/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBContainer.h"
#import "ModuleDocument.h"
#import "MBDocumentWindowController.h"
#import "MBOutlineViewController.h"

@interface MBContainerViewController : MBOutlineViewController 
{

}

#pragma mark "Constructors"
+ (MBContainerViewController *) viewForContainer:(MBContainer *)initContainer;
- (MBContainerViewController *) initWithContainer:(MBContainer *)initContainer;
- (void) dealloc;

#pragma mark "UI Actions"
- (IBAction) deleteContainerClicked:(id)sender;

#pragma mark Accessors
- (MBContainer *) container;
- (ModuleDocument *) document;
- (MBDocumentWindowController *) mainWindowController;

@end
