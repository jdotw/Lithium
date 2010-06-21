//
//  LCFeedbackController.h
//  Lithium Console
//
//  Created by James Wilson on 26/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
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

@property (nonatomic, copy) NSString *subject;
@property (nonatomic, copy) NSAttributedString *feedback;
@property (nonatomic, copy) NSString *feedbackEmailAddress;
@property (nonatomic, copy) NSString *feedbackStatus;
@property (nonatomic, assign) BOOL feedbackTransmissionInProgress;


@end
