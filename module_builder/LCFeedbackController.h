//
//  LCFeedbackController.h
//  Lithium Console
//
//  Created by James Wilson on 26/04/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCFeedbackController : NSWindowController 
{
	/* Feedback */
	IBOutlet NSObjectController *objectController;
	NSString *subject;
	NSAttributedString *feedback;
	NSString *feedbackEmailAddress;
	NSString *feedbackStatus;
	BOOL feedbackTransmissionInProgress;
	
	/* Parent Window */
	NSWindow *parentWindow;

	/* Transmit */
	NSURLConnection *connection;
}

- (id) initForWindow:(NSWindow *)parentWindow;
- (IBAction) feedbackSheetSendClicked:(id)sender;
- (IBAction) feedbackSheetCancelClicked:(id)sender;

@property (copy) NSString *subject;
@property (copy) NSAttributedString *feedback;
@property (copy) NSString *feedbackEmailAddress;
@property (copy) NSString *feedbackStatus;
@property (assign) BOOL feedbackTransmissionInProgress;


@end
