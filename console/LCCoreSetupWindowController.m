//
//  LCCoreSetupWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 29/03/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LCCoreSetupWindowController.h"

#import "LCXMLRequest.h"
#import "LCLicense.h"
#import "LCDemoLicense.h"
#import "LCCustomer.h"
#import <AddressBook/AddressBook.h>

@interface LCCoreSetupWindowController (Private)
- (void) validateLicenseSelection;
- (void) showAuthSetupTab;
@end

@implementation LCCoreSetupWindowController

#pragma mark Initialization

- (id) initWithCustomer:(LCCustomer *)initCustomer
{
	self = [super initWithWindowNibName:@"CoreSetupWindow" owner:self];
	if (!self) return nil;
	
	self.customer = initCustomer;
	self.authRequired = YES;
	
	return self;
}

- (void) dealloc
{
	NSLog (@"%@ DEALLOC", self);
	[super dealloc];
}

#pragma mark Properties

@synthesize status, operationInProgress;
@synthesize licenseTypeIndex, demoName, demoEmail, demoCompany, licenseKey;
@synthesize licName, licCompany, licEmail;
@synthesize authTypeIndex, authUsername, authPassword, authPasswordConfirm;
@synthesize canGoBack, canGoNext; 
@synthesize customer;
@synthesize usingExistingLicense;
@synthesize authRequired;

- (void) setLicenseTypeIndex:(int)value
{
	licenseTypeIndex = value;
	if (licenseTypeIndex == 0)
	{
		/* Demo */
		self.usingExistingLicense = NO;
	}
	else 
	{
		/* Using Exisring */
		self.usingExistingLicense = YES;
	}
}

- (void) setAuthTypeIndex:(int)value
{
	authTypeIndex = value;
	if (authTypeIndex == 0)
	{
		/* Supplied Auth */
		self.authRequired = YES;
	}
	else
	{
		/* No Auth */
		self.authRequired = NO;
	}
}

#pragma mark Navigation

- (void) showLicenseTab
{
	[tabView selectTabViewItemWithIdentifier:@"license"];
	self.canGoBack = NO;
	self.canGoNext = YES;
}

- (void) showAuthSetupTab
{
	[tabView selectTabViewItemWithIdentifier:@"auth"];
	self.canGoBack = NO;	// They can't go back to license after it's done
	self.canGoNext = YES;
}

- (void) showLicenseActivationTab
{
	[tabView selectTabViewItemWithIdentifier:@"activation"];
	self.canGoBack = YES;
	self.canGoNext = YES;
}

- (void) showProgressTab
{
	[tabView selectTabViewItemWithIdentifier:@"progress"];
	self.canGoBack = YES;
	self.canGoNext = NO;
}

- (void) showDoneTab
{
	[tabView selectTabViewItemWithIdentifier:@"done"];
	self.canGoBack = NO;
	self.canGoNext = YES;
	[nextButton setTitle:@"Close"];
	[cancelButton setHidden:YES];
	[backButton setHidden:YES];
}

#pragma mark Window Delegate

- (void) windowDidLoad
{
	/* Attempt to get user info from address book */
	ABPerson *me = [[ABAddressBook sharedAddressBook] me];
	if (me) 
	{
		self.demoName = [NSString stringWithFormat:@"%@ %@", [me valueForProperty:kABFirstNameProperty], [me valueForProperty:kABLastNameProperty]];
		self.licName = self.demoName;
		self.demoCompany = [me valueForProperty:kABOrganizationProperty];
		self.licCompany = self.demoCompany;
		ABMutableMultiValue *emailAddresses = [me valueForProperty:kABEmailProperty];
		NSLog(@"email: %@", emailAddresses);
		unsigned             addyIndex      = 0, addyCount = [emailAddresses count];
		if (addyCount) 
		{
			for (; addyIndex < addyCount; addyIndex++) {
				[demoEmailComboBox addItemWithObjectValue:[emailAddresses valueAtIndex:addyIndex]];
				[licEmailComboBox addItemWithObjectValue:[emailAddresses valueAtIndex:addyIndex]];
			}
			[demoEmailComboBox selectItemAtIndex:0];
			[licEmailComboBox selectItemAtIndex:0];
		}
	}
	
	/* Move to first tab */
	[self showLicenseTab];
}

- (void) windowWillClose:(NSNotification *)notification
{
	NSLog (@"%@ Window will close", self);
	[controllerAlias setContent:nil];
}

#pragma mark Demo License Delegate

- (void) demoLicenseGranted:(LCDemoLicense *)license
{
	signedLicenseKey = [license.signedKey copy];
	if (license == demoLicense)
	{
		[demoLicense autorelease];
		demoLicense = nil;
	}

	self.operationInProgress = NO;
	
	NSLog (@"GOT KEY: %@", signedLicenseKey);

	[self showAuthSetupTab];
}

- (void) demoLicenseNotGranted:(LCDemoLicense *)license error:(NSError *)error
{
	if (license == demoLicense)
	{
		[demoLicense autorelease];
		demoLicense = nil;
	}
	
	self.operationInProgress = NO;
	
	NSAlert *alert = [NSAlert alertWithMessageText:@"Failed to request a Demo License"
									 defaultButton:@"OK"
								   alternateButton:nil
									   otherButton:nil
						 informativeTextWithFormat:@"Lithium was unable to request and download a demo license. Please contact support@lithiumcorp.com for assistance."];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert beginSheetModalForWindow:[self window]
					  modalDelegate:nil
					 didEndSelector:nil
						contextInfo:nil];
}

#pragma mark License Key Delgate

- (void) licenseActivated:(LCLicense *)license
{
	signedLicenseKey = [license.signedKey copy];
	if (license == existingLicense)
	{
		[existingLicense autorelease];
		existingLicense = nil;
	}
	
	self.operationInProgress = NO;
	
	[self showAuthSetupTab];
}

- (void) licenseNotActivated:(LCLicense *)license error:(NSError *)error
{
	if (license == existingLicense)
	{
		[existingLicense autorelease];
		existingLicense = nil;
	}

	self.operationInProgress = NO;

	NSAlert *alert = [NSAlert alertWithMessageText:@"Failed to request a Demo License"
									 defaultButton:@"OK"
								   alternateButton:nil
									   otherButton:nil
						 informativeTextWithFormat:@"Lithium was unable to request and download a demo license. Please contact support@lithiumcorp.com for assistance."];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert beginSheetModalForWindow:[self window]
					  modalDelegate:nil
					 didEndSelector:nil
						contextInfo:nil];
}	

#pragma mark Core Setup XML Delegate

- (void) XMLRequestFinished:(LCXMLRequest *)request
{
	if (request == coreSetupRequest)
	{
		[coreSetupRequest autorelease];
		coreSetupRequest = nil;
	}
	
	self.operationInProgress = NO;

	if (request.success)
	{
		self.customer.isConfigured = YES;	// Update the flag because the next refresh may need auth
		[self showDoneTab];
	}
	else 
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Failed to setup Lithium Core"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"The setup of the Lithium Core deployment failed. Please contact support@lithiumcorp.com for assistance."];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:[self window]
						  modalDelegate:nil
						 didEndSelector:nil
							contextInfo:nil];
		
		[self showLicenseTab];
	}	
}

#pragma mark Actions

- (void) _activateLicense
{
	existingLicense.name = self.licName;
	existingLicense.company = self.licCompany;
	existingLicense.email = self.licEmail;
	existingLicense.delegate = self;
	[existingLicense activateLicenseForCustomer:self.customer];
}	

- (void) _performSetup
{
	/* Create XML */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"coresetup"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	[rootNode addChild:[NSXMLNode elementWithName:@"key" stringValue:signedLicenseKey]]; 
	if (authTypeIndex == 0)
	{
		/* Username and password specified */
		[rootNode addChild:[NSXMLNode elementWithName:@"auth_username" stringValue:self.authUsername]]; 
		[rootNode addChild:[NSXMLNode elementWithName:@"auth_password" stringValue:self.authPassword]]; 
	}
	
	/* Send the license key and auth info to LithiumCore */
	coreSetupRequest = [[LCXMLRequest requestWithCriteria:self.customer
												 resource:[self.customer resourceAddress]
												   entity:[self.customer entityAddress]
												  xmlname:@"coresetup"
												   refsec:0
												   xmlout:xmlDoc] retain];
	[coreSetupRequest setDelegate:self];
	[coreSetupRequest setXMLDelegate:self];
	[coreSetupRequest setPriority:XMLREQ_PRIO_HIGH];	
	[coreSetupRequest setDebug:YES];
	self.operationInProgress = YES;
	self.status = @"Sending setup information to Lithium Core";
	[coreSetupRequest performAsyncRequest];
}

- (void) validateLicenseSelection
{
	/* If the user has asked for a trial, attempt to 
	 * get a demo license. Or, if a license was supplied 
	 * that requires activation present the activation. 
	 */

	if (licenseTypeIndex == 0)
	{
		/* User has asked for a demo license */
		demoLicense = [LCDemoLicense new];
		demoLicense.name = self.demoName;
		demoLicense.email = self.demoEmail;
		demoLicense.company = self.demoCompany;
		demoLicense.delegate = self;
		[demoLicense requestLicenseForCustomer:self.customer];
		self.status = @"Requesting demo license key...";
		self.operationInProgress = YES;
	}
	else if (licenseTypeIndex == 1)
	{
		/* User has supplied an existing key */
		existingLicense = [LCLicense new];
		existingLicense.key = self.licenseKey;
		if (existingLicense.requiresActivation)
		{
			/* License Requires Activation */
			[self showLicenseActivationTab];	// Request auth
		}
		else if (existingLicense.isValid)
		{
			/* License is already activated */
			[self showAuthSetupTab]; //	Move on to auth
		}
		else 
		{
			/* Supplied key looks invalid */
			NSAlert *alert = [NSAlert alertWithMessageText:@"Invalid License Key"
											 defaultButton:@"OK"
										   alternateButton:nil
											   otherButton:nil
								 informativeTextWithFormat:@"The license key you have entered does not appear to be valid. Please double check the license key and enter it exactly as you received it. Contact support@lithiumcorp.com if you need assistance."];
			[alert setAlertStyle:NSCriticalAlertStyle];
			[alert beginSheetModalForWindow:[self window]
							  modalDelegate:nil
							 didEndSelector:nil
								contextInfo:nil];
		}

	}
}

- (void) validateLicenseActivation
{
	/* Validate the users input */
	NSAlert *alert = nil;
	if ([self.licName length] < 1)
	{
		alert = [NSAlert alertWithMessageText:@"Name Required"
								defaultButton:@"OK"
							  alternateButton:nil
								  otherButton:nil
					informativeTextWithFormat:@"Please enter your Full Name to activate the license"];
	}
	else if ([self.licEmail length] < 1)
	{
		alert = [NSAlert alertWithMessageText:@"Email Required"
								defaultButton:@"OK"
							  alternateButton:nil
								  otherButton:nil
					informativeTextWithFormat:@"Please enter your Email Address to activate the license"];
	}		
	
	if (alert)
	{
		/* Problem found */
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:[self window]
						  modalDelegate:nil
						 didEndSelector:nil
							contextInfo:nil];
	}
	else 
	{
		/* Valid! */
		[self _activateLicense];
	}

}

- (void) validateAuthSelection
{
	/* If the user has elected to use a username and password
	 * ensure both a username and password is specified and that
	 * the password fields match
	 */
	
	if (authTypeIndex == 0 &&
		([self.authUsername length] < 1 ||
		 [self.authPassword length] < 1))
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Username and Password Required"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"Please enter a Username and Password that will be used to restrict access to the Lithium Core deployment"];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:[self window] 
						  modalDelegate:nil
						 didEndSelector:nil
							contextInfo:nil];
	}
	else if (authTypeIndex == 0 && ![self.authPassword isEqualToString:self.authPasswordConfirm])
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Passwords Do Not Match"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"The passwords do not match. Please re-enter the passwords."];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:[self window] 
						  modalDelegate:nil
						 didEndSelector:nil
							contextInfo:nil];		
	}
	else 
	{
		/* Valid! */
		[self _performSetup];
		[self showProgressTab];
	}
}

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close];
}

- (IBAction) backClicked:(id)sender
{
	/* Will only be allowed if canGoBack is TRUE and operatingInProgress = NO */
	NSInteger tabIndex = [tabView indexOfTabViewItem:[tabView selectedTabViewItem]];
	[tabView selectTabViewItemAtIndex:tabIndex-1];
}

- (IBAction) nextClicked:(id)sender
{
	NSInteger tabIndex = [tabView indexOfTabViewItem:[tabView selectedTabViewItem]];
	switch (tabIndex)
	{
		case 0:
			/* At license -- validate license */
			[self validateLicenseSelection];
			break;
		case 1:
			/* At license activation -- perform activation */
			[self validateLicenseActivation];
			break;
		case 2:
			/* At auth -- valid username and password */
			[self validateAuthSelection];
			break;
		case 4:
			/* At done -- close */
			[NSApp endSheet:[self window]];
			[[self window] close];
			break;
		default:
			break;
	}	
}

@end
