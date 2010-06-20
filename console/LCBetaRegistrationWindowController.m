//
//  LCBetaRegistrationWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 19/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBetaRegistrationWindowController.h"


@implementation LCBetaRegistrationWindowController

#pragma mark "Initialisation"

- (id) initForBrowserWindow:(NSWindow *)windowForSheet
{
	/* Load NIB */
	self = [super initWithWindowNibName:@"BetaRegistrationWindow"];	
	if (self)
	{
		/* Check Registration */
		NSString *registeredVersionString = [[NSUserDefaults standardUserDefaults] stringForKey:@"L5BetaRegisteredVersion"];
		NSString *currentVersionString = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"];
		if (registeredVersionString && ![registeredVersionString isEqualToString:currentVersionString])
		{
			/* User has registered, but has upgraded */
			self.topTabTag = @"upgrade";
//			self.shouldShow = YES;
			self.shouldShow = NO;
		}
		else if (!registeredVersionString)
		{
			/* User has not registered */
			self.topTabTag = @"install";
			self.shouldShow = YES;
		}
		else
		{
			/* No need to display */
			self.shouldShow = NO;
			[self autorelease];
			return self;
		}

		/* Set Variables */
		self.firstname = [[NSUserDefaults standardUserDefaults] stringForKey:@"L5BetaFirstName"];
		self.lastname = [[NSUserDefaults standardUserDefaults] stringForKey:@"L5BetaLastName"];
		self.email = [[NSUserDefaults standardUserDefaults] stringForKey:@"L5BetaEmail"];
		self.confirmEmail = [[NSUserDefaults standardUserDefaults] stringForKey:@"L5BetaConfirmEmail"];
		self.company = [[NSUserDefaults standardUserDefaults] stringForKey:@"L5BetaCompany"];
		self.hasiPhone = [[NSUserDefaults standardUserDefaults] boolForKey:@"L5BetaHasiPhone"];
		self.iphoneid = [[NSUserDefaults standardUserDefaults] stringForKey:@"L5BetaiPhoneID"];
		self.hasLicense = [[NSUserDefaults standardUserDefaults] boolForKey:@"L5BetaHasLicense"];		
	}
	
	return self;
}

#pragma mark "UI Actions"

- (IBAction) registerClicked:(id)sender
{
	/* 
	 * Validate 
	 */
	
	NSColor *errorColor = [NSColor colorWithCalibratedRed:1.0 green:0.4 blue:0.4 alpha:1.0];
	
	if ([[firstnameTextField stringValue] length] < 1)
	{ 
		[firstnameTextField setBackgroundColor:errorColor]; 
		self.status = @"Please enter your First Name";
		return;
	}
	else
	{ [firstnameTextField setBackgroundColor:[NSColor whiteColor]]; }
	
	if ([[lastnameTextField stringValue] length] < 1)
	{ 
		[lastnameTextField setBackgroundColor:errorColor]; 
		self.status = @"Please enter your Last Name";
		return;
	}
	else
	{ [lastnameTextField setBackgroundColor:[NSColor whiteColor]]; }
	
	if ([[emailTextField stringValue] length] < 1)
	{ 
		[emailTextField setBackgroundColor:errorColor]; 
		self.status = @"Please enter a valid email address"; 
		return;
	}
	else if (!strstr([[emailTextField stringValue] cStringUsingEncoding:NSUTF8StringEncoding], "@"))
	{ 
		[emailTextField setBackgroundColor:errorColor]; 
		self.status = @"Please enter a valid email address"; 
		return;		
	}
	else if (![email isEqualToString:confirmEmail])
	{
		[emailTextField setBackgroundColor:errorColor]; 
		[confirmEmailTextField setBackgroundColor:errorColor]; 
		self.status = @"Email addresses do not match"; 
		return;
	}
	else
	{ 
		[emailTextField setBackgroundColor:[NSColor whiteColor]]; 
		[confirmEmailTextField setBackgroundColor:[NSColor whiteColor]]; 
	}
	
	/* Check validity */
	self.status = @"";
	
	/* Set Variables */
	[[NSUserDefaults standardUserDefaults] setObject:self.firstname forKey:@"L5BetaFirstName"];
	[[NSUserDefaults standardUserDefaults] setObject:self.lastname forKey:@"L5BetaLastName"];
	[[NSUserDefaults standardUserDefaults] setObject:self.email forKey:@"L5BetaEmail"];
	[[NSUserDefaults standardUserDefaults] setObject:self.confirmEmail forKey:@"L5BetaConfirmEmail"];
	[[NSUserDefaults standardUserDefaults] setObject:self.company forKey:@"L5BetaCompany"];
	[[NSUserDefaults standardUserDefaults] setBool:self.hasiPhone forKey:@"L5BetaHasiPhone"];
	[[NSUserDefaults standardUserDefaults] setObject:self.iphoneid forKey:@"L5BetaiPhoneID"];
	[[NSUserDefaults standardUserDefaults] setBool:self.hasLicense forKey:@"L5BetaHasLicense"];
	[[NSUserDefaults standardUserDefaults] setObject:[[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"] forKey:@"L5BetaRegisteredVersion"];
	[[NSUserDefaults standardUserDefaults] synchronize];
	
	/* Close window */
	[NSApp stopModalWithCode:NSOKButton];
	[[self window] close];
	[controllerAlias setContent:nil];
	
	/* Send rego */
	[self sendRegistration];
}

- (IBAction) cancelClicked:(id)sender
{
	/* Close */
	[NSApp stopModalWithCode:NSCancelButton];
	[[self window] close];
	[controllerAlias setContent:nil];
	[self autorelease];
}

- (void) dealloc
{
	[firstname release];
	[lastname release];
	[company release];
	[email release];
	[confirmEmail release];
	[iphoneid release];
	[status release];
	[super dealloc];
}

#pragma mark "Send Registration"

- (void) sendRegistration
{
	/* Create URL String */
	NSString *urlString = [NSString stringWithFormat:@"https://secure.lithiumcorp.com.au/vince/register_beta.php?firstname=%@&lastname=%@&company=%@&email=%@&consoleversion=%@&iphone=%i&haslicense=%i", 
						   [firstname stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [lastname stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [company stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [email stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [[[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   hasiPhone, hasLicense];
	
	/* Setup URL request */
	NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															  cachePolicy:NSURLRequestReloadIgnoringCacheData
														  timeoutInterval:30.0];
	/* Perform request */
	urlConn = [[NSURLConnection connectionWithRequest:urlRequest delegate:self] retain];
	if (urlConn)
	{
		/* Connection proceeding */
		receivedData = [[NSMutableData data] retain];
		[self retain];
	}
	else
	{ 
		/* Connection failed */
		[self connection:nil didFailWithError:nil];
	}	
}

#pragma mark HTTP Handling

- (void) connection:(NSURLConnection *)connection 
 didReceiveResponse:(NSURLResponse *)response
{
	/* Received non-data response */
	[receivedData setLength:0];
}

-(void)	connection:(NSURLConnection *) connection
	didReceiveData:(NSData *) data
{
	/* Received the actual data */
	[receivedData appendData:data];
}

-(NSURLRequest *)	connection:(NSURLConnection *)connection
			 willSendRequest:(NSURLRequest *)request
			redirectResponse:(NSURLResponse *)redirectResponse
{
	/* Allow redirects */
    return request;
}

-(void) connectionDidFinishLoading:(NSURLConnection *) connection
{
	/* Registration Successful */
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];	

	/* Finished */
	[self autorelease];
}	

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	/* Registration Failed */
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];
	
	/* Finished */
	[self autorelease];	
}

#pragma mark "Accessors"

@synthesize firstname;
@synthesize lastname;
@synthesize company;
@synthesize email;
@synthesize confirmEmail;
@synthesize hasiPhone;
@synthesize iphoneid;
@synthesize hasLicense;
@synthesize status;
@synthesize topTabTag;
@synthesize shouldShow;

@end
