//
//  LTriggerSet.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTTriggerSet.h"
#import "LTTriggerSetAppRule.h"
#import "LTTriggerSetValRule.h"
#import "LTTrigger.h"

@implementation LTTriggerSet

@synthesize applied=_applied, defaultApplied, appRuleDict, appRules, ruleUpdateInProgress;

- (id) init
{
    self = [super init];
    if (!self) return nil;
    
    appRuleDict = [[NSMutableDictionary dictionary] retain];
    appRules = [[NSMutableArray array] retain];
    
    return self;
}

- (void) dealloc
{
    [appRuleDict release];
    [appRules release];
    [super dealloc];
}

- (void) beginAPIUpdate
{
    _apiUpdate = YES;
}

- (void) endAPIUpdate
{
    _apiUpdate = NO;
}

- (void) setApplied:(BOOL)applied
{
    _applied = applied;
    if (_apiUpdate) _apiApplied = applied;
}

#pragma - Rule Updating

- (LTTriggerSetAppRule *) _scopedAppRuleForObject:(NSString *)objName device:(NSString *)devName site:(NSString *)siteName
{
    /* Return an AppRule the first appRule that matches the scope */
    for (LTTriggerSetAppRule *appRule in self.appRules)
    {
        if ((objName && appRule.objName) && [appRule.objName isEqualToString:objName] 
            && (devName && appRule.devName) && [appRule.devName isEqualToString:devName] 
            && (siteName && appRule.siteName) && [appRule.siteName isEqualToString:siteName])
        {
            /* Match! */
            return appRule;
        }
    }
    return nil; // No match
}

- (BOOL) _setHasChanged
{
    if (_applied != _apiApplied) return YES;
    else return NO;
}

- (BOOL) setOrTriggersHaveChanged
{
    /* Returns YES if there are changes in either the triggerset
     * or the triggers in the site. This is used to control UI elements 
     */
    if ([self _setHasChanged]) return YES;
    for (LTTrigger *trg in self.children)
    {
        if (trg.triggerHasChanged) return YES;
    }
    return NO;
}

- (NSArray *) rulesToUpdateForChangesInObject:(NSString *)objName device:(NSString *)devName site:(NSString *)siteName
{
    /* Returns an array of the ValRule and AppRule objects that need
     * to be pushed to LithiumCore in order to affect any changes made
     * in the trigger set and/or triggers based on the scope of the 
     * changes being dictated by the supplied objName, devName and siteName
     */
    
    NSMutableArray *rulesToUpdate = [NSMutableArray array];
    
    /* Determine local appRule changes */
    if ([self _setHasChanged])
    {
        LTTriggerSetAppRule *appRule = [self _scopedAppRuleForObject:objName device:devName site:siteName];
        if (!appRule) 
        {
            appRule = [[LTTriggerSetAppRule new] autorelease];
            appRule.objName = objName;
            if (objName) appRule.objDesc = self.object.desc;
            appRule.devName = devName;
            if (devName) appRule.devDesc = self.device.desc;
            appRule.siteName = siteName;
            if (siteName) appRule.siteDesc = self.site.desc;
        }
        appRule.applyFlag = _applied;
        [rulesToUpdate addObject:appRule];
    }
    
    /* Loop through triggers */
    for (LTTrigger *trg in self.children)
    {
        LTTriggerSetValRule *valRule = [trg ruleToUpdateForChangesInObject:objName device:devName site:siteName];
        if (valRule) [rulesToUpdate addObject:valRule];
    }
    
    return rulesToUpdate;
}

- (void) sendRuleUpdatesForScopeObject:(NSString *)objName device:(NSString *)devName site:(NSString *)siteName
{
    /* Get rules */
    NSArray *rules = [self rulesToUpdateForChangesInObject:objName
                                                    device:devName
                                                      site:siteName];
    if (!rules || rules.count < 1) 
    {
        self.ruleUpdateInProgress = NO;
        NSLog (@"%@ no rules to update!", self);
        return;
    }
    
    /* Create XML Request */
	NSMutableString *xmlString;
	xmlString = [NSMutableString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"];
	[xmlString appendString:@"<rules>"];    // Root element 
    [xmlString appendFormat:@"<tset_name>%@</tset_name>", self.name];
    [xmlString appendFormat:@"<met_name>%@</met_name>", self.metric.name];
	[xmlString appendString:@"<update>"];   // Rules to be updated (add/edit)
    for (id rule in rules)
    {
        if ([rule isMemberOfClass:[LTTriggerSetAppRule class]])
        {
            LTTriggerSetAppRule *appRule = (LTTriggerSetAppRule *)rule;
            [xmlString appendFormat:@"<apprule>"];
            if (appRule.identifier != 0) [xmlString appendFormat:@"<id>%i</id>", appRule.identifier];
            if (appRule.siteName) [xmlString appendFormat:@"<site_name>%@</site_name>", appRule.siteName];
            if (appRule.siteDesc) [xmlString appendFormat:@"<site_desc>%@</site_desc>", appRule.siteDesc];
            if (appRule.devName) [xmlString appendFormat:@"<dev_name>%@</dev_name>", appRule.devName];
            if (appRule.devDesc) [xmlString appendFormat:@"<dev_desc>%@</dev_desc>", appRule.devDesc];
            if (appRule.objName) [xmlString appendFormat:@"<obj_name>%@</obj_name>", appRule.objName];
            if (appRule.objDesc) [xmlString appendFormat:@"<obj_desc>%@</obj_desc>", appRule.objDesc];
            [xmlString appendFormat:@"<applyflag>%i</applyflag>", appRule.applyFlag];
            [xmlString appendFormat:@"</apprule>"];
        }
        else if ([rule isMemberOfClass:[LTTriggerSetValRule class]])
        {
            LTTriggerSetValRule *valRule = (LTTriggerSetValRule *)rule;
            [xmlString appendFormat:@"<valrule>"];
            if (valRule.identifier != 0) [xmlString appendFormat:@"<id>%i</id>", valRule.identifier];
            if (valRule.siteName) [xmlString appendFormat:@"<site_name>%@</site_name>", valRule.siteName];
            if (valRule.siteDesc) [xmlString appendFormat:@"<site_desc>%@</site_desc>", valRule.siteDesc];
            if (valRule.devName) [xmlString appendFormat:@"<dev_name>%@</dev_name>", valRule.devName];
            if (valRule.devDesc) [xmlString appendFormat:@"<dev_desc>%@</dev_desc>", valRule.devDesc];
            if (valRule.objName) [xmlString appendFormat:@"<obj_name>%@</obj_name>", valRule.objName];
            if (valRule.objDesc) [xmlString appendFormat:@"<obj_desc>%@</obj_desc>", valRule.objDesc];
            if (valRule.trgName) [xmlString appendFormat:@"<trg_name>%@</trg_name>", valRule.trgName];
            if (valRule.trgDesc) [xmlString appendFormat:@"<trg_desc>%@</trg_desc>", valRule.trgDesc];
            if (valRule.xValue) [xmlString appendFormat:@"<xval>%@</xval>", valRule.xValue];
            if (valRule.yValue) [xmlString appendFormat:@"<yval>%@</yval>", valRule.yValue];
            [xmlString appendFormat:@"<duration>%i</duration>", valRule.duration];
            [xmlString appendFormat:@"<trg_type_num>%i</trg_type_num>", valRule.triggerType];
            [xmlString appendFormat:@"<adminstate_num>%i</adminstate_num>", valRule.adminState];            
            [xmlString appendFormat:@"</valrule>"];
        }
    }
	[xmlString appendString:@"</update>"];
	[xmlString appendString:@"</rules>"];

    NSLog(@"XML for Rule Update is '%@'", xmlString);
	
	/* Create request */
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[self.object urlForXml:@"triggerset_bulkupdate" timestamp:0]
															 cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														 timeoutInterval:60.0];
    
    NSLog(@"Update req is %@", theRequest);
    
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
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		self.ruleUpdateInProgress = YES;
		self.receivedData=[NSMutableData data];
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to download console.php");
        self.ruleUpdateInProgress = NO;
	}	
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    /* Call super to clean up and record error */
	[super connection:connection didFailWithError:error];
    
    self.ruleUpdateInProgress = NO;
    
    /* Call delegate */
    if (self.delegate) 
    {
        [self.delegate performSelector:@selector(triggerSetUpdateDidFail:) withObject:self];
    }
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	/* Set state */
    self.ruleUpdateInProgress = NO;
    
    /* Clean up -- Dont use super, we dont want XML parsed */
    [connection release];
    self.receivedData = nil;

    /* Call delegate */
    if (self.delegate) 
    {
        [self.delegate performSelector:@selector(triggerSetUpdateDidFinish:) withObject:self];
    }
}


@end
