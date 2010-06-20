//
//  LCDiagController.h
//  LCAdminTools
//
//  Created by James Wilson on 27/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDiagTest.h"
#import "LCDiagTestMarshall.h"
#import "LCTransparentOutlineView.h"

@interface LCDiagController : NSObject 
{
	LCDiagTestMarshall *testMarshall;

	IBOutlet NSTreeController *testTreeController;
	IBOutlet LCTransparentOutlineView *testOutlineView;
	
	BOOL testInProgress;
}

- (IBAction) runDiagnosticClicked:(id)sender;
- (IBAction) emailResultClicked:(id)sender;

- (LCDiagTestMarshall *) testMarshall;
- (void) setTestMarshall:(LCDiagTestMarshall *)marshall;

- (BOOL) testInProgress;
- (void) setTestInProgress:(BOOL)flag;

- (void) expandAll;

@end
