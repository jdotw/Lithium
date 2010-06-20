#import "LCConsoleController.h"
#import "LCAuthenticator.h"
#import "LCCaseController.h"
#import "LCActivityList.h"
#import "LCActivityController.h"
#import "LCIntervalValueTransformer.h"
#import "LCZeroIsNilTransformer.h"
#import "LCColourDotTransformer.h"
#import "LCColourDotTransformer2.h"
#import "LCIncidentBoundDotTransformer.h"
#import "LCCaseOwnedDotTransformer.h"
#import "LCCaseStatusDotTransformer.h"
#import "LCActivityPriorityTransformer.h"
#import "LCActivityPriorityDotTransformer.h"
#import "LCEpochTimeTransformer.h"
#import "LCNilIsAllTransformer.h"
#import "LCXMLRequestQueue.h"
#import "LCUpperCaseTransformer.h"
#import "LCPreferencesController.h"
#import "LCCaseSearchController.h"
#import "LCCaseActiveDotTransformer.h"
#import "LCWatcherWindowController.h"
#import "LCMetricGraphDocument.h"
#import "LCEntityTypeTransformer.h"
#import "LCEntityRefreshTransformer.h"
#import "LCVRackDocument.h"
#import "LCMultiGraphDocument.h"
#import "LCNanIsNullTransformer.h"
#import "LCMetricHistoryValueTransformer.h"
#import "LCSSceneDocument.h"
#import "LCTriggersetAppliedTransformer.h"
#import "LCTriggersetTrgEnabledTransformer.h"
#import "LCTriggersetNullIsAll.h"
#import "LCTriggersetRuleEffect.h"
#import "LCBrowser2Controller.h"
#import "LCYesIsRedDotTransformer.h"
#import "LCMetricAnalysisDateTransformer.h"
#import "LC255IsTrueTransformer.h"
#import "LC255IsFalseTransformer.h"
#import "LCBlueDotTransformer.h"
#import "LCXServiceDotTransformer.h"
#import "LCDeploymentWizardController.h"
#import "LCHasActionsTransformer.h"
#import "LCActionHasRunTransformer.h"
#import "LCZeroIsUnlimitedTransformer.h"
#import "LCErrorLogWindowController.h"
#import "LCErrorLog.h"
#import "LCBetaRegistrationWindowController.h"
#import "UKCrashReporter.h"
#import "LCFeedbackController.h"
#import "LCDateAgoTranformer.h"

@implementation LCConsoleController

#pragma mark "Class Methods"

static LCConsoleController *masterController = NULL;

+ (LCConsoleController *) masterController
{ return masterController; }

+ (void) setMasterController:(LCConsoleController *)controller
{ masterController = controller; }

#pragma mark "Initialisation"

- (void) dealloc
{
	[persistentWindows release];
	[super dealloc];
}

#pragma mask "Awake From NIB"

- (void) awakeFromNib
{	
	/* Setup NSURL Cache */
	[[NSURLCache sharedURLCache] setMemoryCapacity:0];
	[[NSURLCache sharedURLCache] setDiskCapacity:0];
	
	/* Create operation queue */
	self.operationQueue = [NSOperationQueue new];
	
	/* Create browser list */
	browsers = [[NSMutableArray array] retain];
	
	/* Install URL Handle */
	[[NSAppleEventManager sharedAppleEventManager] setEventHandler:self 
													   andSelector:@selector(handleGetURLEvent:withReplyEvent:) 
													 forEventClass:kInternetEventClass 
														andEventID:kAEGetURL];
	
	/* Check for local core */
	BOOL localCoreConfiguring = NO;
	if ([[NSFileManager defaultManager] fileExistsAtPath:@"/Library/Lithium/LithiumCore.app"])
	{
		/* Local Core install found, check if it is configurd */
		if (![[NSFileManager defaultManager] fileExistsAtPath:@"/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/profile.php"] &&
			![[NSUserDefaults standardUserDefaults] boolForKey:@"IgnoreLocalCoreInstallState"])
		{
			/* Local Core is unconfigured! */
			NSAlert *alert = [NSAlert alertWithMessageText:@"Lithium Core installation is not configured"
											 defaultButton:@"Configure"
										   alternateButton:@"Always Ignore"
											   otherButton:@"Ignore"
								 informativeTextWithFormat:@"A installation of Lithium Core was found on the local host that has no configuration.\n\nBefore using Lithium Console, Lithium Core must be configured using the Lithium Core Admin application.\n"];
			int button = [alert runModal];
			if (button == 1)
			{
				/* Configure */
				[[NSWorkspace sharedWorkspace] launchApplication:@"Lithium Core Admin"];
				localCoreConfiguring = YES;
				[NSApp terminate:self];
			}
			else if (button == -1)
			{
				/* Ignore */
				
			}
			else if (button == 0)
			{
				/* Always Ignore */
				[[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"IgnoreLocalCoreInstallState"];
				[[NSUserDefaults standardUserDefaults] synchronize];
			}
		}
	}
	
	/* Beta Rego */
//	LCBetaRegistrationWindowController *betaRego = [[LCBetaRegistrationWindowController alloc] initForBrowserWindow:nil];
//	if (betaRego.shouldShow && !localCoreConfiguring)
//	{ 
//		[NSApp runModalForWindow:[betaRego window]]; 
//	}	
	
	/* Set master */
	[LCConsoleController setMasterController:self];
	
	/* Create cache path */
	[[NSFileManager defaultManager] createDirectoryAtPath:[@"~/Library/Caches/com.lithiumcorp.Console" stringByExpandingTildeInPath]
											   attributes:nil];
	
	/* Register preference defaults */
	[LCPreferencesController registerDefaults];
	
	/* Initialise Error Log */
	[LCErrorLog masterInit];
	
	/* Initialise master activity list */
	[LCActivityList masterInit];

	/* Initialise XML Request Queue */
	[LCXMLRequestQueue masterInit];

	/*
	 * Value transformers
	 */
	
	LCIntervalValueTransformer *intervalTransformer = [[[LCIntervalValueTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:intervalTransformer forName:@"LCIntervalValueTransformer"];
	
	LCZeroIsNilTransformer *zeroTransformer = [[[LCZeroIsNilTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:zeroTransformer forName:@"LCZeroIsNilTransformer"];

	LCColourDotTransformer *colourDotTransformer = [[[LCColourDotTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:colourDotTransformer forName:@"LCColourDotTransformer"];

	LCColourDotTransformer *colourDotTransformer2 = [[[LCColourDotTransformer2 alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:colourDotTransformer2 forName:@"LCColourDotTransformer2"];
	
	LCIncidentBoundDotTransformer *incidentBoundDotTransformer = [[[LCIncidentBoundDotTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:incidentBoundDotTransformer forName:@"LCIncidentBoundDotTransformer"];

	LCCaseOwnedDotTransformer *caseOwnedDotTransformer = [[[LCCaseOwnedDotTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:caseOwnedDotTransformer forName:@"LCCaseOwnedDotTransformer"];

	LCCaseStatusDotTransformer *caseStatusDotTransformer = [[[LCCaseStatusDotTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:caseStatusDotTransformer forName:@"LCCaseStatusDotTransformer"];

	LCActivityPriorityDotTransformer *activityPriorityDotTransformer = [[[LCActivityPriorityDotTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:activityPriorityDotTransformer forName:@"LCActivityPriorityDotTransformer"];

	LCActivityPriorityTransformer *activityPriorityTransformer = [[[LCActivityPriorityTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:activityPriorityTransformer forName:@"LCActivityPriorityTransformer"];

	LCNilIsAllTransformer *nilIsAllTransformer = [[[LCNilIsAllTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:nilIsAllTransformer forName:@"LCNilIsAllTransformer"];
	
	LCUpperCaseTransformer *uppercaseTransformer = [[[LCUpperCaseTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:uppercaseTransformer forName:@"LCUpperCaseTransformer"];
	
	LCCaseActiveDotTransformer *caseActiveTransformer = [[[LCCaseActiveDotTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:caseActiveTransformer forName:@"LCCaseActiveDotTransformer"];

	LCEpochTimeTransformer *epochTimeTransformer = [[[LCEpochTimeTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:epochTimeTransformer forName:@"LCEpochTimeTransformer"];

	LCEntityTypeTransformer	*entityTypeTransformer = [[[LCEntityTypeTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:entityTypeTransformer forName:@"LCEntityTypeTransformer"];

	LCEntityRefreshTransformer	*entityRefreshTransformer = [[[LCEntityRefreshTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:entityRefreshTransformer forName:@"LCEntityRefreshTransformer"];

	LCNanIsNullTransformer	*nanIsNullTransformer = [[[LCNanIsNullTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:nanIsNullTransformer forName:@"LCNanIsNullTransformer"];
	
	LCMetricHistoryValueTransformer *historyValueTransformer = [[[LCMetricHistoryValueTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:historyValueTransformer forName:@"LCMetricHistoryValueTransformer"];
	
	LCTriggersetAppliedTransformer *tsetAppliedTransformer = [[[LCTriggersetAppliedTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:tsetAppliedTransformer forName:@"LCTriggersetAppliedTransformer"];
	
	LCTriggersetTrgEnabledTransformer *trgAdminTransformer = [[[LCTriggersetTrgEnabledTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:trgAdminTransformer forName:@"LCTriggersetTrgEnabledTransformer"];
	
	LCTriggersetNullIsAll *nullIsAllTransformer = [[[LCTriggersetNullIsAll alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:nullIsAllTransformer forName:@"LCTriggersetNullIsAll"];
	
	LCTriggersetRuleEffect *ruleEffectTransformer = [[[LCTriggersetRuleEffect alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:ruleEffectTransformer forName:@"LCTriggersetRuleEffect"];
	
	LCYesIsRedDotTransformer *yesIsRedTransformer = [[[LCYesIsRedDotTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:yesIsRedTransformer forName:@"LCYesIsRedDotTransformer"];
	
	LCMetricAnalysisDateTransformer *metricAnalysisDateTransformer = [[[LCMetricAnalysisDateTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:metricAnalysisDateTransformer forName:@"LCMetricAnalysisDateTransformer"];
	
	LC255IsTrueTransformer *twofivefiveTransformer = [[[LC255IsTrueTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:twofivefiveTransformer forName:@"LC255IsTrueTransformer"];

	LC255IsFalseTransformer *twofivefiveFalseTransformer = [[[LC255IsFalseTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:twofivefiveFalseTransformer forName:@"LC255IsFalseTransformer"];

	LCBlueDotTransformer *blueDotTransformer = [[[LCBlueDotTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:blueDotTransformer forName:@"LCBlueDotTransformer"];
	
	LCXServiceDotTransformer *xserviceTranformer = [[[LCXServiceDotTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:xserviceTranformer forName:@"LCXServiceDotTransformer"];

	LCHasActionsTransformer *hasActionsTransformer = [[[LCHasActionsTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:hasActionsTransformer forName:@"LCHasActionsTransformer"];

	LCActionHasRunTransformer *actionHasRunTransformer = [[[LCActionHasRunTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:actionHasRunTransformer forName:@"LCActionHasRunTransformer"];
	
	LCZeroIsUnlimitedTransformer *zeroUnlimitedTransformer = [[[LCZeroIsUnlimitedTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:zeroUnlimitedTransformer forName:@"LCZeroIsUnlimitedTransformer"];
	
	[NSValueTransformer setValueTransformer:[[[LCDateAgoTranformer alloc] init] autorelease]
									forName:@"LCDateAgoTransformer"];
	
	/* Load/Refrsh the customer list */
	[LCCustomerList masterInit];	

	/* Show browser */
	[[[LCBrowser2Controller alloc] init] autorelease];
}

- (BOOL) showingAssistant
{ return showingAssistant; }

- (void) setShowingAssistant:(BOOL)flag
{ showingAssistant = flag; }

- (void) hideNotificationWindow
{
}

- (void) thawPersistentWindows
{
	/* 
	 * FIX -- Disabled for now
	 */
	
	/* Load persistent windows */
//	NSData *windowData = [[NSUserDefaults standardUserDefaults] objectForKey:@"persistentWindowsGeneric"];
//	if (windowData)
//	{ persistentWindows = [[NSKeyedUnarchiver unarchiveObjectWithData:windowData] retain]; }
//	if (!persistentWindows)
//	{ persistentWindows = [[NSMutableArray array] retain]; }
//	[[NSUserDefaults standardUserDefaults] removeObjectForKey:@"persistentWindowsGeneric"];
//	NSEnumerator *windowEnum = [persistentWindows objectEnumerator];
//	NSWindowController *windowController;
//	while (windowController = [windowEnum nextObject])
//	{
//		[windowController retain];
//	}
	
	/* Show browser */
//	if (!browserOpened)
//	{ [[LCBrowser2Controller alloc] init]; }	f
}

- (BOOL) browserOpened
{ return browserOpened; }

- (void) setBrowserOpened:(BOOL)flag
{ browserOpened = flag; }

#pragma mark "Persistent Windows"

- (NSMutableArray *) persistentWindows
{ return persistentWindows; }

- (void) insertObject:(NSWindowController *)windowController inPersistentWindowsAtIndex:(unsigned int)index
{
//	if ([persistentWindows containsObject:windowController]) return;
//	[persistentWindows insertObject:windowController atIndex:index];
}

- (void) removeObjectFromPersistentWindowsAtIndex:(unsigned int)index
{
//	[persistentWindows removeObjectAtIndex:index];
}

- (void) removePersistentWindow:(NSWindowController *)windowController
{
//	if ([persistentWindows containsObject:windowController])
//	{ [self removeObjectFromPersistentWindowsAtIndex:[persistentWindows indexOfObject:windowController]]; }
}

#pragma mark "Application Delegates"

- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender
{
	return NO;
}

- (BOOL)applicationShouldHandleReopen:(NSApplication *)theApplication hasVisibleWindows:(BOOL)flag
{
	/* This is fired when the user clicks on the dock */
	if ([[LCBrowser2Controller activeControllers] count] > 0)
	{
		int minimisedCount = 0;
		NSEnumerator *controllerEnum = [[LCBrowser2Controller activeControllers] objectEnumerator];
		LCBrowser2Controller *controller;
		LCBrowser2Controller *lastMinimised = nil;
		while (controller = [controllerEnum nextObject])
		{
			if ([[controller window] isMiniaturized]) 
			{ 
				minimisedCount++;
				lastMinimised = controller;
			}
		}
		if (minimisedCount == [[LCBrowser2Controller activeControllers] count])
		{
			[[lastMinimised window] makeKeyAndOrderFront:self];
		}
	}
	else
	{
		[[[LCBrowser2Controller alloc] init] autorelease];
	}
	
	return NO;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	for (LCBrowser2Controller *browser in browsers)
	{
		if (browser.contentViewController && ![browser.contentViewController treeSelectionCanChangeToRepresent:nil])
		{ return NSTerminateCancel; }
	}
	
	[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:persistentWindows] forKey:@"persistentWindowsGeneric"];
	[[NSUserDefaults standardUserDefaults] synchronize];
	return NSTerminateNow;
}

#pragma mark "Browser Windows"

@synthesize browsers;

- (void) newBrowserDidOpen:(LCBrowser2Controller *)browser
{
	/* Add to list */
	[self insertObject:browser inBrowsersAtIndex:browsers.count];
}

- (void) browserDidClose:(LCBrowser2Controller *)browser
{
	/* Remove from list */
	if ([browsers containsObject:browser])
	{ [self removeObjectFromBrowsersAtIndex:[browsers indexOfObject:browser]]; }
}

- (void) browserDidBecomeMainWindow:(LCBrowser2Controller *)browser
{
	/* Bump it to the top of the list */
	if ([browsers containsObject:browser])
	{ [self removeObjectFromBrowsersAtIndex:[browsers indexOfObject:browser]]; }
	[self insertObject:browser inBrowsersAtIndex:0];
}

- (void) insertObject:(LCBrowser2Controller *)browser inBrowsersAtIndex:(unsigned int)index
{
	[browsers insertObject:browser atIndex:index];
}

- (void) removeObjectFromBrowsersAtIndex:(unsigned int)index
{
	[browsers removeObjectAtIndex:index];
}

- (LCBrowser2Controller *) browserForSheet
{
	/* Returns a browser, or creates a new one and
	 * makes that browser the main window for use
	 * with a modal sheet
	 */
	
	LCBrowser2Controller *browser = nil;
	if (browsers.count > 0)
	{ 
		browser = [browsers objectAtIndex:0]; 
	}
	else
	{ 
		browser = [[[LCBrowser2Controller alloc] init] autorelease];	/* The browser init will add it to the browser list */
	}
	[[browser window] makeMainWindow];
	
	return browser;
}

#pragma mark "Event Delegates"

- (void)handleGetURLEvent:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent
{
	/* Get URL */
	NSURL *url = [NSURL URLWithString:[[event paramDescriptorForKeyword:keyDirectObject] stringValue]];
	NSString *path = [url path];
	NSArray *components = [path pathComponents];
	NSString *context = [components objectAtIndex:1];

	/* Process URL */
	if ([context isEqualToString:@"entity"])
	{
		/* Get Entity Properties from URL */
		NSMutableDictionary *entityProperties = [NSMutableDictionary dictionary];
		[entityProperties setObject:[NSString stringWithFormat:@"%i", ([components count]-2)] forKey:@"type_num"];
		for (NSString *component in components)
		{
			switch ([components indexOfObject:component])
			{
				case 2:
					[entityProperties setObject:component forKey:@"cust_name"];
					[entityProperties setObject:component forKey:@"cust_desc"];
					break;
				case 3:
					[entityProperties setObject:component forKey:@"site_name"];
					[entityProperties setObject:component forKey:@"site_desc"];
					break;
				case 4:
					[entityProperties setObject:component forKey:@"dev_name"];
					[entityProperties setObject:component forKey:@"dev_desc"];
					break;
				case 5:
					[entityProperties setObject:component forKey:@"cnt_name"];
					[entityProperties setObject:component forKey:@"cnt_desc"];
					break;
				case 6:
					[entityProperties setObject:component forKey:@"obj_name"];
					[entityProperties setObject:component forKey:@"obj_desc"];
					break;
				case 7:
					[entityProperties setObject:component forKey:@"met_name"];
					[entityProperties setObject:component forKey:@"met_desc"];
					break;
				case 8:
					[entityProperties setObject:component forKey:@"trg_name"];
					[entityProperties setObject:component forKey:@"trg_desc"];
					break;
			}
			[entityProperties setObject:component forKey:@"name"];
			[entityProperties setObject:component forKey:@"desc"];
		}
		
		/* Create decriptor and locate entity */
		LCEntityDescriptor *descriptor = [LCEntityDescriptor descriptorWithProperties:entityProperties];
		LCEntity *entity = [descriptor locateEntity:YES];
		
		/* Load Browser */
		[[[LCBrowser2Controller alloc] initWithEntity:entity] autorelease];
	}
}

#pragma mark "Operation Queue"

@synthesize operationQueue;

#pragma mark "User Interface Methods"

-(IBAction) newIncidentManager:(id)sender
{ 
	[[[LCIncmgrController alloc] init] autorelease];
}

-(IBAction) openNewCase:(id)sender
{
	[[LCCaseController alloc] initForNewCase];
}

- (IBAction) activityViewer:(id)sender
{
	[LCActivityController activityController];
}

- (IBAction) newBrowser:(id)sender
{
	[[[LCBrowser2Controller alloc] init] autorelease];
}

- (IBAction) preferencesClicked:(id)sender
{
	[LCPreferencesController preferencesController];
}

- (IBAction) caseSearchClicked:(id)sender
{ 
	[[LCCaseSearchController alloc] init];
}

- (IBAction) newWatcherWindow:(id)sender
{
	[[LCWatcherWindowController alloc] init];
}

- (IBAction) newGraphWindow:(id)sender
{
	LCMetricGraphDocument *document;
	document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];	
}

- (IBAction) newMultiGraphWindow:(id)sender
{
	LCMultiGraphDocument *mgraphDocument;
	mgraphDocument = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMultiGraphDocument"];
	[[NSDocumentController sharedDocumentController] addDocument:mgraphDocument];
	[mgraphDocument makeWindowControllers];
	[mgraphDocument showWindows];	
}

- (IBAction) onlineDocsClicked:(id)sender
{
	NSURL *url = [NSURL URLWithString:@"http://support.lithium5.com"];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (IBAction) forumClicked:(id)sender
{
	NSURL *url = [NSURL URLWithString:@"http://support.lithium5.com"];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (IBAction) newVirtualRackClicked:(id)sender
{
}

- (IBAction) newStateSceneClicked:(id)sender
{
}

- (IBAction) showLicenseClicked:(id)sender
{
}

- (IBAction) errorLogClicked:(id)sender
{
	[LCErrorLogWindowController errorLogController];
}

- (IBAction) sendFeedbackClicked:(id)sender
{
	[[LCFeedbackController alloc] initForWindow:nil];
}

#pragma mark "Menu Methods"

- (NSMenu *) executeActionMenu
{ return executeActionMenu; }

- (void)menuWillOpen:(NSMenu *)menu
{

}

#pragma mark "Properties"

@synthesize persistentWindows;
@synthesize startupTimer;
@synthesize updater;
@synthesize executeActionMenu;
@synthesize processMonitorMenu;
@end
