//
//  LCCrashReporter.m
//  Lithium Console
//
//  Created by James Wilson on 15/10/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCCrashReporter.h"


@implementation LCCrashReporter

#pragma mark "Initialisation"

- (void) checkForCrashes
{	
	[self setUserEmail:[[NSUserDefaults standardUserDefaults] objectForKey:@"crashReportEmail"]];
	
//	NSString*		appName = [[[NSBundle mainBundle] infoDictionary] objectForKey: @"CFBundleExecutable"];
	NSString*		crashLogsFolder = [@"/Library/Logs/CrashReporter/" stringByExpandingTildeInPath];
	NSString*		crashLogName = @"lithium.crash.log";
	NSString*		crashLogPath = [crashLogsFolder stringByAppendingPathComponent: crashLogName];
	NSDictionary*	fileAttrs = [[NSFileManager defaultManager] fileAttributesAtPath: crashLogPath traverseLink: YES];
	NSDate*			lastTimeCrashLogged = (fileAttrs == nil) ? nil : [fileAttrs fileModificationDate];
	NSTimeInterval	lastCrashReportInterval = [[NSUserDefaults standardUserDefaults] floatForKey: @"UKCrashReporterLastCrashReportDate"];
	NSDate*			lastTimeCrashReported = [NSDate dateWithTimeIntervalSince1970: lastCrashReportInterval];

	if (lastTimeCrashLogged && [lastTimeCrashReported compare: lastTimeCrashLogged] == NSOrderedAscending)
	{
		/* Recent crash has occurred */
		[mainWindow makeKeyAndOrderFront:self];
		[NSApp beginSheet:reporterSheet
		   modalForWindow:mainWindow
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}			
}

#pragma mark "UI Actions"

- (IBAction) reportClicked:(id)sender
{
	NS_DURING
		// Try whether the classes we need to talk to the CGI are present:
		Class			NSMutableURLRequestClass = NSClassFromString( @"NSMutableURLRequest" );
		Class			NSURLConnectionClass = NSClassFromString( @"NSURLConnection" );
		if( NSMutableURLRequestClass == Nil || NSURLConnectionClass == Nil )
			NS_VOIDRETURN;
		
		// Fetch the newest report from the log:
//		NSString*		appName = [[[NSBundle mainBundle] infoDictionary] objectForKey: @"CFBundleExecutable"];
		NSString*		crashLogsFolder = [@"/Library/Logs/CrashReporter/" stringByExpandingTildeInPath];
		NSString*		crashLogName = @"lithium.crash.log";
		NSString*		crashLogPath = [crashLogsFolder stringByAppendingPathComponent: crashLogName];
		NSString*		crashLog = [NSString stringWithContentsOfFile: crashLogPath encoding:NSUTF8StringEncoding error:nil];
		NSArray*		separateReports = [crashLog componentsSeparatedByString: @"\n\n**********\n\n"];
		NSString*		currentReport = [separateReports count] > 0 ? [separateReports objectAtIndex: [separateReports count] -1] : @"*** Couldn't read Report ***";
		NSData*			crashReport = [currentReport dataUsingEncoding: NSUTF8StringEncoding];	// 1 since report 0 is empty (file has a delimiter at the top).
		
		// Prepare a request:
		NSMutableURLRequest *postRequest = [NSMutableURLRequestClass requestWithURL: [NSURL URLWithString:@"http://secure.lithiumcorp.com.au/reporter/osx_report.php"]];
		NSString            *boundary = @"0xKhTmLbOuNdArY";
		NSURLResponse       *response = nil;
		NSError             *error = nil;
		NSString            *contentType = [NSString stringWithFormat:@"multipart/form-data; boundary=%@",boundary];
		NSString			*agent = @"UKCrashReporter";
		
		// Add form trappings to crashReport:
		NSData*			header = [[NSString stringWithFormat:@"--%@\r\nContent-Disposition: form-data; name=\"crashlog\"\r\n\r\n", boundary] dataUsingEncoding:NSUTF8StringEncoding];
		NSMutableData*	formData = [[header mutableCopy] autorelease];
		[formData appendData:[[NSString stringWithFormat:@"Users Email: %@\r\n", userEmail] dataUsingEncoding:NSUTF8StringEncoding]];
		[formData appendData:[[NSString stringWithFormat:@"Users Notes: %@\r\n\r\n", userDescription] dataUsingEncoding:NSUTF8StringEncoding]];
		[formData appendData: crashReport];
		[formData appendData:[[NSString stringWithFormat:@"\r\n--%@--\r\n",boundary] dataUsingEncoding:NSUTF8StringEncoding]];
		
		// setting the headers:
		[postRequest setHTTPMethod: @"POST"];
		[postRequest setValue: contentType forHTTPHeaderField: @"Content-Type"];
		[postRequest setValue: agent forHTTPHeaderField: @"User-Agent"];
		[postRequest setHTTPBody: formData];
		
		[NSURLConnectionClass sendSynchronousRequest: postRequest returningResponse: &response error: &error];
		
		// Remember we just reported a crash, so we don't ask twice:
		[[NSUserDefaults standardUserDefaults] setFloat: [[NSDate date] timeIntervalSince1970] forKey: @"UKCrashReporterLastCrashReportDate"];
		[[NSUserDefaults standardUserDefaults] synchronize];
	NS_HANDLER
		NSLog(@"Exception during check for crash: %@",localException);
	NS_ENDHANDLER		
	
	[NSApp endSheet:reporterSheet];
	[reporterSheet close];
}

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:reporterSheet];
	[reporterSheet close];	
}

#pragma mark "Accessors"

- (NSString *) userDescription
{ return userDescription; }

- (void) setUserDescription:(NSString *)string
{
	[userDescription release];
	userDescription = [string retain];
}

- (NSString *) userEmail
{ return userEmail; }

- (void) setUserEmail:(NSString *)string
{
	[userEmail release];
	userEmail = [string retain];
	[[NSUserDefaults standardUserDefaults] setObject:userEmail forKey:@"crashReportEmail"];
}

@end
