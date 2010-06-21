//
//  LCGroupEditWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 10/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCGroup.h"
#import "LCXMLRequest.h"

@interface LCGroupEditWindowController : NSWindowController 
{
	/* UI Elements */
	IBOutlet NSTextField *titleTextField;
	IBOutlet NSButton *okButton;
	
	/* Document properties */
	NSString *desc;
	
	/* Related Objects */
	LCGroup *group;
	LCGroup *parent;
	LCGroup *previousParent;
	LCCustomer *customer;
	NSWindow *windowForSheet;
	LCEntity *entity;
	
	/* XML Properties */
	LCXMLRequest *xmlRequest;
	BOOL xmlOperationInProgress;
	NSString *status;
	BOOL shouldClose;	
	BOOL sheetShown;
	BOOL addGroup;
	BOOL removeGroup;
}

#pragma mark "Constructors"
- (id) initForNewGroupUnderParent:(LCGroup *)parent customer:(LCCustomer *)initCustomer windowForSheet:(NSWindow *)initWindow;
- (id) initWithGroupToEdit:(LCGroup *)initGroup customer:(LCCustomer *)initCustomer windowForSheet:(NSWindow *)initWindow;
- (id) initWithGroupToMove:(LCGroup *)initGroup toParent:(LCGroup *)initParent windowForSheet:(NSWindow *)initWindow;
- (id) initWithGroupToDelete:(LCGroup *)initGroup windowForSheet:(NSWindow *)initWindow;
- (id) initWithEntity:(LCEntity *)initEntity toAddToGroup:(LCGroup *)initGroup windowForSheet:(NSWindow *)initWindow;
- (id) initWithEntity:(LCEntity *)initEntity toMoveFromGroup:(LCGroup *)initPreviousGroup toGroup:(LCGroup *)initGroup windowForSheet:(NSWindow *)initWindow;
- (id) initWithEntity:(LCEntity *)initEntity toRemoveFromGroup:(LCGroup *)initGroup windowForSheet:(NSWindow *)initWindow;

#pragma mark "UI Actions"
- (IBAction) cancelClicked:(id)sender;
- (IBAction) saveClicked:(id)sender;

#pragma mark "Properties"
@property (nonatomic,copy) NSString *desc;
@property (nonatomic,retain) LCGroup *group;
@property (nonatomic,retain) LCGroup *parent;
@property (nonatomic,retain) LCGroup *previousParent;
@property (nonatomic,retain) LCCustomer *customer;
@property (nonatomic,retain) LCEntity *entity;
@property (nonatomic, assign) BOOL xmlOperationInProgress;
@property (nonatomic,copy) NSString *status;
@property (nonatomic,retain) NSWindow *windowForSheet;


@end
