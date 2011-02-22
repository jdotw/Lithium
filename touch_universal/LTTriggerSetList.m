//
//  LTTriggerSetList.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTTriggerSetList.h"

#import "LTEntity.h"
#import "LTCoreDeployment.h"
#import "LTTrigger.h"
#import "LTTriggerSet.h"
#import "LTTriggerSetAppRule.h"
#import "LTTriggerSetValRule.h"

@implementation LTTriggerSetList

@synthesize metric=_metric, children, childDict;

- (id) initWithMetric:(LTEntity *)metric
{
    self = [super init];
    if (!self) return nil;
    
    children = [[NSMutableArray array] retain];
    childDict = [[NSMutableDictionary dictionary] retain];
    
    self.metric = metric;
    
    return self;
}

- (void) dealloc
{
    self.metric = nil;
    [children release];
    [childDict release];
    [super dealloc];
}

- (void) refresh
{
    /* Check state */
	if (self.refreshInProgress || ![(LTCoreDeployment *)[self.metric coreDeployment] enabled])
	{
		/* Customer disabled or refresh already in progress, do not proceed */
		return;
	}

	/* Refresh the triggerset list */
	NSMutableURLRequest *theRequest = [NSMutableURLRequest requestWithURL:[self.metric.object urlForXml:@"triggerset_list" timestamp:0]
															  cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														  timeoutInterval:60.0];	
    NSLog (@"Req is %@", theRequest);
	
	/* Establish Connection */
	NSURLConnection *theConnection = [[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		self.refreshInProgress = YES;
        self.receivedData = [NSMutableData data];
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to download triggerset list");
	}
}

#pragma mark Parsing

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{    
    /* Parse XML Using TBXML */
    TBXML *xml = [TBXML tbxmlWithXMLData:self.receivedData];
    if (xml && xml.rootXMLElement && xml.rootXMLElement->firstChild)
    {
        TBXMLElement *node;
        for (node=xml.rootXMLElement->firstChild; node; node=node->nextSibling)
        {
            if (strcmp(node->name, "triggerset")==0)
            {
                /* Triggerset Found */
                LTTriggerSet *tset = [childDict objectForKey:[TBXML textForElementNamed:@"name" parentElement:node]];
                if (!tset)
                {
                    tset = [[LTTriggerSet new] autorelease];
                    [tset beginAPIUpdate];
                    tset.name = [TBXML textForElementNamed:@"name" parentElement:node];
                    tset.desc = [TBXML textForElementNamed:@"desc" parentElement:node];
                    tset.parent = self.metric;
                    [childDict setObject:tset forKey:tset.name];
                    [children addObject:tset];
                }
                [tset beginAPIUpdate];
                tset.applied = [TBXML intFromTextForElementNamed:@"applied_flag" parentElement:node];
                
                /* Loop through triggerset children */
                TBXMLElement *tsetChild;
                for (tsetChild=node->firstChild; tsetChild; tsetChild=tsetChild->nextSibling)
                {
                    /* Look for triggers */
                    if (strcmp(tsetChild->name, "trigger")==0)
                    {
                        /* Trigger found */
                        LTTrigger *trg = [tset.childDict objectForKey:[TBXML textForElementNamed:@"name" parentElement:tsetChild]];
                        if (!trg)
                        {
                            trg = [[LTTrigger new] autorelease];
                            [trg beginAPIUpdate];
                            trg.name = [TBXML textForElementNamed:@"name" parentElement:tsetChild];
                            trg.desc = [TBXML textForElementNamed:@"desc" parentElement:tsetChild];
                            [tset.childDict setObject:trg forKey:trg.name];
                            [tset.children addObject:trg];
                        }
                        [trg beginAPIUpdate];
                        trg.valueType = [TBXML intFromTextForElementNamed:@"valtype_num" parentElement:tsetChild];
                        trg.effect = [TBXML intFromTextForElementNamed:@"effect_num" parentElement:tsetChild];
                        trg.triggerType = [TBXML intFromTextForElementNamed:@"trgtype_num" parentElement:tsetChild];
                        trg.units = [TBXML textForElementNamed:@"units" parentElement:tsetChild];
                        trg.xValue = [TBXML textForElementNamed:@"xval" parentElement:tsetChild];
                        trg.yValue = [TBXML textForElementNamed:@"yval" parentElement:tsetChild];
                        trg.duration = [TBXML intFromTextForElementNamed:@"duration" parentElement:tsetChild];
                        trg.adminState = [TBXML intFromTextForElementNamed:@"adminstate_num" parentElement:tsetChild];
                        
                        /* Loop through trigger children */
                        TBXMLElement *trgChild;
                        for (trgChild=tsetChild->firstChild; trgChild; trgChild=trgChild->nextSibling)
                        {
                            /* Look for val rules under the trigger */
                            if (strcmp(trgChild->name, "valrule")==0)
                            {
                                /* Found an valrule */
                                LTTriggerSetValRule *rule = [trg.valRuleDict objectForKey:[TBXML textForElementNamed:@"id" parentElement:node]];
                                if (!rule)
                                {
                                    rule = [[LTTriggerSetValRule new] autorelease];
                                    rule.identifier = [TBXML intFromTextForElementNamed:@"id" parentElement:node];
                                    [trg.valRuleDict setObject:rule forKey:[TBXML textForElementNamed:@"id" parentElement:node]];
                                    [trg.valRules addObject:rule];
                                }
                                rule.siteName = [TBXML textForElementNamed:@"site_name" parentElement:node];
                                rule.siteDesc = [TBXML textForElementNamed:@"site_desc" parentElement:node];
                                rule.devName = [TBXML textForElementNamed:@"dev_name" parentElement:node];
                                rule.devDesc = [TBXML textForElementNamed:@"dev_desc" parentElement:node];
                                rule.objName = [TBXML textForElementNamed:@"obj_name" parentElement:node];
                                rule.objDesc = [TBXML textForElementNamed:@"obj_desc" parentElement:node];
                                rule.trgName = [TBXML textForElementNamed:@"trg_name" parentElement:node];
                                rule.trgDesc = [TBXML textForElementNamed:@"trg_desc" parentElement:node];
                                rule.xValue = [TBXML textForElementNamed:@"xval" parentElement:node];
                                rule.yValue = [TBXML textForElementNamed:@"yval" parentElement:node];
                                rule.duration = [TBXML intFromTextForElementNamed:@"duration" parentElement:node];
                                rule.triggerType = [TBXML intFromTextForElementNamed:@"trg_type_num" parentElement:node];
                                rule.adminState = [TBXML intFromTextForElementNamed:@"adminstate_num" parentElement:node];
                                
                            }
                        }
                        
                        [trg endAPIUpdate];
                    }

                    /* Look for AppRules under the triggerset */
                    if (strcmp(tsetChild->name, "apprule")==0)
                    {
                        /* Found an apprule */
                        LTTriggerSetAppRule *rule = [tset.appRuleDict objectForKey:[TBXML textForElementNamed:@"id" parentElement:node]];
                        if (!rule)
                        {
                            rule = [[LTTriggerSetAppRule new] autorelease];
                            rule.identifier = [TBXML intFromTextForElementNamed:@"id" parentElement:node];
                            [tset.appRuleDict setObject:rule forKey:[TBXML textForElementNamed:@"id" parentElement:node]];
                            [tset.appRules addObject:rule];
                        }
                        rule.siteName = [TBXML textForElementNamed:@"site_name" parentElement:node];
                        rule.siteDesc = [TBXML textForElementNamed:@"site_desc" parentElement:node];
                        rule.devName = [TBXML textForElementNamed:@"dev_name" parentElement:node];
                        rule.devDesc = [TBXML textForElementNamed:@"dev_desc" parentElement:node];
                        rule.objName = [TBXML textForElementNamed:@"obj_name" parentElement:node];
                        rule.objDesc = [TBXML textForElementNamed:@"obj_desc" parentElement:node];
                        rule.applyFlag = [TBXML intFromTextForElementNamed:@"applyflag" parentElement:node];
                        
                    }
                }
                
                [tset endAPIUpdate];
            }
        }
    }    
    
    /* Set state and clean up*/
    [super connectionDidFinishLoading:connection];  // Does not release connection
    [connection release];   // Safe to release, we created it
    
    /* Post notification */
    [[NSNotificationCenter defaultCenter] postNotificationName:kLTTriggerSetListRefreshFinished object:self];
}    

@end
