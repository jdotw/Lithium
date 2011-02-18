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
	NSMutableURLRequest *theRequest = [NSMutableURLRequest requestWithURL:[self.metric urlForXml:@"triggerset_list" timestamp:0]
															  cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														  timeoutInterval:60.0];	
	
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
                    tset.name = [TBXML textForElementNamed:@"name" parentElement:node];
                    tset.desc = [TBXML textForElementNamed:@"name" parentElement:node];
                    [childDict setObject:tset forKey:tset.name];
                    [children addObject:tset];
                }
                tset.applied = [TBXML intFromTextForElementNamed:@"applied_flag" parentElement:node];
                
                /* Look for triggers */
                TBXMLElement *triggerNode;
                for (triggerNode=node->firstChild; triggerNode; triggerNode=triggerNode->nextSibling)
                {
                    if (strcmp(triggerNode->name, "trigger")==0)
                    {
                        /* Trigger found */
                        LTTrigger *trg = [tset.childDict objectForKey:[TBXML textForElementNamed:@"name" parentElement:triggerNode]];
                        if (!trg)
                        {
                            trg = [[LTTrigger new] autorelease];
                            trg.name = [TBXML textForElementNamed:@"name" parentElement:triggerNode];
                            trg.desc = [TBXML textForElementNamed:@"desc" parentElement:triggerNode];
                            [tset.childDict setObject:trg forKey:trg.name];
                            [tset.children addObject:tset];
                        }
                        trg.valueType = [TBXML intFromTextForElementNamed:@"valtype_num" parentElement:triggerNode];
                        trg.effect = [TBXML intFromTextForElementNamed:@"effect_num" parentElement:triggerNode];
                        trg.triggerType = [TBXML intFromTextForElementNamed:@"trgtype_num" parentElement:triggerNode];
                        trg.units = [TBXML textForElementNamed:@"units" parentElement:triggerNode];
                        trg.xValue = [TBXML textForElementNamed:@"xval" parentElement:triggerNode];
                        trg.yValue = [TBXML textForElementNamed:@"yval" parentElement:triggerNode];
                        trg.duration = [TBXML intFromTextForElementNamed:@"duration" parentElement:triggerNode];
                        trg.adminState = [TBXML intFromTextForElementNamed:@"adminstate_num" parentElement:triggerNode];
                    }
                }
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
