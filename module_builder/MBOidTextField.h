//
//  MBOidTextField.h
//  ModuleBuilder
//
//  Created by James Wilson on 15/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MBOidTextField : NSTextField 
{
	IBOutlet NSViewController *viewController;
}

#pragma mark "Awake From NIB"
- (void) awakeFromNib;

#pragma mark "Drag and Drop"
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)info;
- (BOOL)performDragOperation:(id <NSDraggingInfo>)info;

@end
