//
//  LCFeedbackController.m
//  Lithium Console
//
//  Created by James Wilson on 26/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCFeedbackController.h"
#import <AddressBook/AddressBook.h>


@implementation LCFeedbackController

- (id) initForWindow:(NSWindow *)initParentWindow
{
	self = [super initWithWindowNibName:@"FeedbackWindow"];
	if (self)
	{
		parentWindow = initParentWindow;
		
		NSString *preferredEmailAddress = [[NSUserDefaults standardUserDefaults] stringForKey:@"LCFeedbackControllerPreferredEmail"];
		if (preferredEmailAddress)
		{
			self.feedbackEmailAddress = preferredEmailAddress;
		}
		else
		{
			ABMultiValue *emailAddresses = [[[ABAddressBook sharedAddressBook] me] valueForProperty: kABEmailProperty];
			if (emailAddresses)
			{
				NSString *defaultKey = [emailAddresses primaryIdentifier];
				if (defaultKey)
				{
					unsigned int defaultIndex = [emailAddresses indexForIdentifier:defaultKey];
					if (defaultIndex >= 0) self.feedbackEmailAddress = [emailAddresses valueAtIndex: defaultIndex];
				}
			}
		}
		
		if (parentWindow)
		{
			[NSApp beginSheet:[self window]
			   modalForWindow:parentWindow
				modalDelegate:self
			   didEndSelector:nil
				  contextInfo:nil];
		}
		else
		{
			[[self window] makeKeyAndOrderFront:self];
		}
	}
	return self;
}

- (void) dealloc
{
	[feedback release];
	[feedbackStatus release];
	[feedbackEmailAddress release];
	[super dealloc];
}

- (void) windowWillClose:(NSNotification *)notification
{
	/* Autorelease */
	[objectController setContent:nil];
	[self autorelease];
}

- (IBAction) feedbackSheetSendClicked:(id)sender
{
	if ([subject length] < 1)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Subject Required"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"Please enter a Subject for your feedback"];
		[alert beginSheetModalForWindow:[self window]
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
		return;
	}
	
	[[NSUserDefaults standardUserDefaults] setObject:self.feedbackEmailAddress forKey:@"LCFeedbackControllerPreferredEmail"];
	[[NSUserDefaults standardUserDefaults] synchronize];
	
	NSString            *boundary = @"0xKhTmLbOuNdArY";
	NSMutableString*	crashReportString = [NSMutableString string];
	[crashReportString appendString: [feedback string]];
	[crashReportString appendString: @"\n\n"];
	[crashReportString appendString: [NSString stringWithFormat:@"Email: %@", feedbackEmailAddress]];
	[crashReportString replaceOccurrencesOfString: boundary withString: @"USED_TO_BE_KHTMLBOUNDARY" options: 0 range: NSMakeRange(0, [crashReportString length])];
	NSData*				crashReport = [crashReportString dataUsingEncoding: NSUTF8StringEncoding];
	
	// Prepare a request:
	NSString *urlString = [NSString stringWithFormat:@"http://lithium5.com/tech/reporter/console_feedback.php?from=%@&subject=%@", 
						   [feedbackEmailAddress stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding], 
						   [subject stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
	NSMutableURLRequest *postRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]];
	NSString            *contentType = [NSString stringWithFormat:@"multipart/form-data; boundary=%@",boundary];
	NSString			*agent = @"UKCrashReporter";
	
	// Add form trappings to crashReport:
	NSData*			header = [[NSString stringWithFormat:@"--%@\r\nContent-Disposition: form-data; name=\"crashlog\"\r\n\r\n", boundary] dataUsingEncoding:NSUTF8StringEncoding];
	NSMutableData*	formData = [[header mutableCopy] autorelease];
	[formData appendData: crashReport];
	[formData appendData:[[NSString stringWithFormat:@"\r\n--%@--\r\n",boundary] dataUsingEncoding:NSUTF8StringEncoding]];
	
	// setting the headers:
	[postRequest setHTTPMethod: @"POST"];
	[postRequest setValue: contentType forHTTPHeaderField: @"Content-Type"];
	[postRequest setValue: agent forHTTPHeaderField: @"User-Agent"];
	NSString *contentLength = [NSString stringWithFormat:@"%lu", [formData length]];
	[postRequest setValue: contentLength forHTTPHeaderField: @"Content-Length"];
	[postRequest setHTTPBody: formData];
	
	// Go into progress mode and kick off the HTTP post:
	self.feedbackTransmissionInProgress = YES;
	self.feedbackStatus = @"Sending feedback...";
	connection = [[NSURLConnection connectionWithRequest: postRequest delegate: self] retain];
}

-(void)	connectionDidFinishLoading:(NSURLConnection *)conn
{
	[connection release];
	connection = nil;

	self.feedbackTransmissionInProgress = NO;
	self.feedbackStatus = @"Done";

	if (parentWindow)
	{ [NSApp endSheet:[self window]]; }
	[[self window] close];
}

-(void)	connection:(NSURLConnection *)conn didFailWithError:(NSError *)error
{
	[connection release];
	connection = nil;

	self.feedbackTransmissionInProgress = NO;
	self.feedbackStatus = @"Failed to send feedback.";
}

- (IBAction) feedbackSheetCancelClicked:(id)sender
{
	if (connection)
	{
		[connection cancel];
		[connection release];
		connection = nil;
		
		self.feedbackTransmissionInProgress = NO;
		self.feedbackStatus = @"Cancelled";
	}
	
	if (parentWindow)
	{ [NSApp endSheet:[self window]]; }
	[[self window] close];
}

@synthesize subject;
@synthesize feedback;
@synthesize feedbackEmailAddress;
@synthesize feedbackStatus;
@synthesize feedbackTransmissionInProgress;

@end
