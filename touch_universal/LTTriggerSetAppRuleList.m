//
//  LTTriggerSetAppRuleList.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTTriggerSetAppRuleList.h"

#import "LTEntity.h"
#import "LTTriggerSet.h"
#import "LTCoreDeployment.h"
#import "LTTriggerSetAppRule.h"

@implementation LTTriggerSetAppRuleList

@synthesize tset=_tset, ruleDict, rules;

- (id) initWithTriggerSet:(LTTriggerSet *)tset
{
    self = [super init];
    if (!self) return nil;
    
    rules = [[NSMutableArray array] retain];
    ruleDict = [[NSMutableDictionary dictionary] retain];
    
    self.tset = tset;
    
    return self;
}

- (void) dealloc
{
    self.tset = nil;
    [rules release];
    [ruleDict release];
    [super dealloc];
}

- (void) refresh
{
    /* Check state */
	if (self.refreshInProgress || ![(LTCoreDeployment *)[self.tset.metric coreDeployment] enabled])
	{
		/* Customer disabled or refresh already in progress, do not proceed */
		return;
	}
    
    /* Create XML Request */
	NSMutableString *xmlString;
	xmlString = [NSMutableString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"];
	[xmlString appendString:@"<parameters>"];
	[xmlString appendFormat:@"<tset_name>%@</tset_name>", self.tset.name];
	[xmlString appendFormat:@"<met_name>%@</met_name>", self.tset.metric.name];
	[xmlString appendString:@"</parameters>"];

	/* Refresh the triggerset apprule list */
	NSMutableURLRequest *theRequest = [NSMutableURLRequest requestWithURL:[self.tset.metric.object urlForXml:@"triggerset_apprule_list" timestamp:0]
															  cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														  timeoutInterval:60.0];	
    NSLog (@"Req is %@", theRequest);
    /* Outbound XML doc to be sent */
	NSString *formBoundary = [[NSProcessInfo processInfo] globallyUniqueString];
	NSString *boundaryString = [NSString stringWithFormat: @"multipart/form-data; boundary=%@", formBoundary];
	[theRequest addValue:boundaryString forHTTPHeaderField:@"Content-Type"];
	[theRequest setHTTPMethod: @"POST"];
	NSMutableData *postData = [NSMutableData data];
	[postData appendData:[[NSString stringWithFormat:@"--%@\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
	[postData appendData:[@"Content-Disposition: form-data; name=\"xmlfile\"; filename=\"ltouch.xml\"\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
	[postData appendData:[@"Content-Type: text/xml\r\n\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
	[xmlString replaceOccurrencesOfString:@"\r\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
	[xmlString replaceOccurrencesOfString:@"\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
	[postData appendData:[xmlString dataUsingEncoding:NSUTF8StringEncoding]];
	[postData appendData:[[NSString stringWithFormat:@"\r\n--%@--\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
	[theRequest setHTTPBody:postData];
    
	
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
            if (strcmp(node->name, "apprule")==0)
            {
                /* Found an apprule */
                LTTriggerSetAppRule *rule = [ruleDict objectForKey:[TBXML textForElementNamed:@"id" parentElement:node]];
                if (!rule)
                {
                    rule = [[LTTriggerSetAppRule new] autorelease];
                    rule.identifier = [TBXML intFromTextForElementNamed:@"id" parentElement:node];
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
    }    
    
    /* Set state and clean up*/
    [super connectionDidFinishLoading:connection];  // Does not release connection
    [connection release];   // Safe to release, we created it
    
    /* Post notification */
    [[NSNotificationCenter defaultCenter] postNotificationName:kLTTriggerSetAppRuleListRefreshFinished object:self];
}    


@end
