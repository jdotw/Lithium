//
//  LCBrowserIncidentsContentController.h
//  Lithium Console
//
//  Created by James Wilson on 8/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2ContentViewController.h"
#import "LCCustomerList.h"
#import "LCBrowser2Controller.h"
#import "LCObjectTree.h"
#import "LCObjectTreeOutlineView.h"
#import "LCInspectorController.h"
#import "LCBrowserTableView.h"

@interface LCBrowserIncidentsContentController : LCBrowser2ContentViewController 
{
	/* Related Objects */
	LCBrowser2Controller *browser;
	LCObjectTree *objectTree;
	
	/* Selection */
	LCIncident *selectedIncident;
	NSArray *selectedIncidents;

	/* UI Elements */
	IBOutlet LCBrowserTableView *incidentTableView;
	IBOutlet NSArrayController *incidentArrayController;	
	IBOutlet LCObjectTreeOutlineView *objectOutlineView;
	IBOutlet NSTreeController *objectTreeController;
	IBOutlet LCInspectorController *inspectorController;
	IBOutlet NSMenu *actionMenu;
}

#pragma mark "Constructor"
- (id) initInBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection"
@property (retain) LCIncident *selectedIncident;
@property (retain) NSArray *selectedIncidents;

#pragma mark "UI Actions"
- (IBAction) graphSelectedClicked:(NSMenuItem *)sender;
- (IBAction) refreshClicked:(id)sender;
- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender;
- (IBAction) reviewActiveTriggersClicked:(id)sender;
- (IBAction) restartMonitoringProcessClicked:(id)sender;
- (IBAction) connectUsingSSH:(id)sender;
- (IBAction) connectUsingTelnet:(id)sender;
- (IBAction) connectUsingWeb:(id)sender;
- (IBAction) connectUsingARD:(id)sender;
- (IBAction) copyWebUrlToClipBoardClicked:(id)sender;
- (IBAction) openWebUrlInBrowserClicked:(id)sender;
- (IBAction) refreshEntityClicked:(id)sender;
- (IBAction) faultHistoryClicked:(id)sender;
- (IBAction) analyseSelectedClicked:(id)sender;
- (IBAction) triggerTuningClicked:(id)sender;
- (IBAction) resetTriggerRulesClicked:(id)sender;
- (IBAction) viewIncident:(id)sender;
- (IBAction) clearIncident:(id)sender;
- (IBAction) refreshIncidentsForSelectedIncident:(id)sender;
- (IBAction) browseToSelectedIncident:(id)sender;
- (IBAction) graphSelectedIncident:(id)sender;
- (IBAction) metricAnalyseSelectedIncident:(id)sender;
- (IBAction) faultHistoryForSelectedIncident:(id)sender;
- (IBAction) metricHistoryForSelectedIncident:(id)sender;
- (IBAction) triggerTuningForSelectedIncident:(id)sender;
- (IBAction) editActionsForSelectedIncident:(id)sender;
- (IBAction) cancelPendingActionsClicked:(id)sender;
- (IBAction) openCaseForSelectedClicked:(id)sender;

#pragma mark "Action Menu Methods"
- (void) buildActionMenu:(NSMenu *)menu forIncident:(LCIncident *)incident;
- (void) clearActionMenu:(NSMenu *)menu;

#pragma mark "Properties"
@property (readonly) LCCustomerList *customerList;
@property (assign) LCBrowser2Controller *browser;
@property (retain) LCObjectTree *objectTree;

@end
