//
//  LCServiceScriptManagerController.m
//  Lithium Console
//
//  Created by James Wilson on 7/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCServiceScriptManagerController.h"


@implementation LCServiceScriptManagerController

#pragma mark "Constructors"

- (void) awakeFromNib
{
	/* Create script list */
	[self setScriptList:[LCServiceScriptList scriptListForCustomer:[setupController customer] service:nil]];

	/* Refresh list */
	[scriptList highPriorityRefresh];
}

- (void) dealloc
{
	[scriptList release];
	[super dealloc];
}

#pragma mark "UI Services"

- (IBAction) uploadScriptClicked:(id)sender
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
	[openPanel setTitle:@"Select script to upload (2Mb max)"];
	[openPanel setAllowsMultipleSelection:NO];
	
	int result = [openPanel runModalForDirectory:nil file:nil];
	if (result == NSOKButton)
	{
		[self setUploadRequest:[[LCScriptUploadRequest uploadScript:[openPanel filename] type:@"service" forCustomer:[setupController customer]] retain]];
		[uploadRequest setDelegate:self];
		[uploadRequest performAsyncRequest];
		
		/* Open sheet */
		[NSApp beginSheet:uploadSheet
		   modalForWindow:[setupController window] 
			modalDelegate:self 
		   didEndSelector:nil 
			  contextInfo:nil];			
	}
}

- (void) scriptUploadFinished:(id)sender
{
	[scriptList highPriorityRefresh];
}

- (IBAction) closeUploadSheetClicked:(id)sender
{
	[NSApp endSheet:uploadSheet];
	[uploadSheet close];
}

- (IBAction) cancelUploadClicked:(id)sender
{
	[uploadRequest cancel];
	[NSApp endSheet:uploadSheet];
	[uploadSheet close];
}

- (IBAction) deleteScriptClicked:(id)sender
{
	/* Check focus */
	if ([[setupController window] firstResponder] != localScriptsTableView) return;
	
	/* Deletes script */
	LCServiceScript *selectedScript = [self selectedScript];
	if (!selectedScript) return;
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"script"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"type" stringValue:@"service"]];
	[rootnode addChild:[NSXMLNode elementWithName:@"name" stringValue:[selectedScript name]]];
	
	/* Create and perform request */
	deleteRequest = [[LCXMLRequest requestWithCriteria:[setupController customer]
											  resource:[[setupController customer] resourceAddress]
												entity:[[setupController customer] entityAddress]
											   xmlname:@"script_delete"
												refsec:0
												xmlout:xmldoc] retain];
	[deleteRequest setPriority:XMLREQ_PRIO_HIGH];
	[deleteRequest setDelegate:self];
	[deleteRequest performAsyncRequest];
	
	/* Remove from lists */
	selectedScript.installedVersion = nil;
	[selectedScript setInstalledIcon:[NSImage imageNamed:@"off_16.tif"]];
	if ([[scriptList localScripts] containsObject:selectedScript])
	{ [scriptList removeObjectFromLocalScriptsAtIndex:[[scriptList localScripts] indexOfObject:selectedScript]]; }
}

- (IBAction) repairScriptClicked:(id)sender
{
	/* Check focus */
	if ([[setupController window] firstResponder] != localScriptsTableView) return;

	/* Deletes script */
	LCServiceScript *selectedScript = [self selectedScript];
	if (!selectedScript) return;
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"script"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"type" stringValue:@"service"]];
	[rootnode addChild:[NSXMLNode elementWithName:@"name" stringValue:[selectedScript name]]];
	
	/* Create and perform request */
	[self setRepairInProgress:YES];
	[self setRepairScriptOutput:nil];
	[self setRepairStatusIcon:nil];
	[self setRepairStatusString:nil];
	repairRequest = [[LCXMLRequest requestWithCriteria:[setupController customer]
											  resource:[[setupController customer] resourceAddress]
												entity:[[setupController customer] entityAddress]
											   xmlname:@"script_repair"
												refsec:0
												xmlout:xmldoc] retain];
	[repairRequest setPriority:XMLREQ_PRIO_HIGH];
	[repairRequest setXMLDelegate:self];
	[repairRequest setDelegate:self];
	[repairRequest performAsyncRequest];
	
	/* Open sheet */
	[NSApp beginSheet:repairSheet
	   modalForWindow:[setupController window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];		
}

- (IBAction) repairSheetCloseClicked:(id)sender
{
	[NSApp endSheet:repairSheet];
	[repairSheet close];
}

- (IBAction) refreshListClicked:(id)sender
{
	[scriptList highPriorityRefresh];
}

- (IBAction) downloadScriptClicked:(id)sender
{
	/* Check focus */
	if ([[setupController window] firstResponder] != localScriptsTableView) return;

	if (![self selectedScript]) return;
	LCServiceScript *script = [self selectedScript];
	
	NSSavePanel *savePanel = [NSSavePanel savePanel];
	[savePanel setTitle:@"Save script to..."];
	int result = [savePanel runModalForDirectory:nil file:[script name]];
	if (result == NSOKButton)
	{
		[self setDownloadRequest:[[LCScriptDownloadRequest downloadScript:[script name] 
																	 type:@"service"
															  forCustomer:[setupController customer] 
																	   to:[savePanel filename]] retain]];
		[downloadRequest setDelegate:self];
		[downloadRequest performAsyncRequest];
		
		/* Open sheet */
		[NSApp beginSheet:downloadSheet
		   modalForWindow:[setupController window] 
			modalDelegate:self 
		   didEndSelector:nil 
			  contextInfo:nil];			
	}	
}

- (void) scriptDownloadFinished:(id)sender
{
	[NSApp endSheet:downloadSheet];
	[downloadSheet close];
}

#pragma mark "Community Script Downloads"

- (IBAction) installCommunityScriptClicked:(NSArray *)arg
{
	/* Check focus */
	if ([[setupController window] firstResponder] != communityScriptsTableView) return;

	/* Get selected */
	LCServiceScript *selectedScript = [self selectedCommunityScript];
	if (!selectedScript) return;
	scriptBeingInstalled = selectedScript;
	
	/* Installs a community script */
	NSString *urlString = [NSString stringWithFormat:[selectedScript communityURL]];
	NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															  cachePolicy:NSURLRequestReloadIgnoringCacheData
														  timeoutInterval:[[NSUserDefaults standardUserDefaults] floatForKey:@"xmlHTTPTimeoutSec"]];
	
	urlConn = [[NSURLConnection connectionWithRequest:urlRequest delegate:self] retain];
	if (urlConn)
	{
		/* Connection proceeding */
		receivedData = [[NSMutableData data] retain];
	}
	else
	{ 
		/* Connection failed */
	}	

	/* Set state */
	[self setScriptInstallInProgress:YES];
	[self setScriptInstallProgressString:@"Downloading script from forum.lithiumcorp.com"];
	
	/* Open sheet */
	[NSApp beginSheet:communityDownloadSheet
	   modalForWindow:[setupController window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];				
	
	return;
}

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

-(void) connection:(NSURLConnection *)connection
		didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *) challenge
{
}		

-(void) connectionDidFinishLoading:(NSURLConnection *) connection
{
	/* Set state */
	[self setScriptInstallInProgress:NO];
	[self setScriptInstallProgressString:@"Writing to temporary file..."];

	/* Download finished, Save to a temporary file */
	NSString *filename = [NSString stringWithFormat:@"%@/%@", 
		[@"~/Library/Caches/com.lithiumcorp.Console" stringByExpandingTildeInPath], [scriptBeingInstalled name]];
	[receivedData writeToFile:filename atomically:NO];
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];
	
	/* End the download sheet */
	[NSApp endSheet:communityDownloadSheet];
	[communityDownloadSheet close];
	
	/* Now upload the file to Core */
	[self setUploadRequest:[[LCScriptUploadRequest uploadScript:filename type:@"service" forCustomer:[setupController customer]] retain]];
	[uploadRequest setDelegate:self];
	[uploadRequest performAsyncRequest];
	
	/* Open sheet */
	[NSApp beginSheet:uploadSheet
	   modalForWindow:[setupController window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];			
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	/* Set state */
	[self setScriptInstallInProgress:NO];
	[self setScriptInstallProgressString:@"Failed to download script from forum.lithiumcorp.com"];

	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];
} 

- (IBAction) cancelInstallCommunityScriptClicked:(id)sender
{
	/* Cancel the download/install */
	[urlConn cancel];
	[urlConn release];
	
	[NSApp endSheet:communityDownloadSheet];
	[communityDownloadSheet close];	
}

#pragma mark "XML Delegate"

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Retain element */
	xmlElement = [element retain];
	
	/* Release previous string */
	if (xmlString)
	{
		[xmlString release];
		xmlString = nil;
	}
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Create new string or append string to existing */
	if (xmlElement)
	{
		if (xmlString) { [xmlString appendString:string]; }
		else { xmlString = [[NSMutableString stringWithString:string] retain]; }
	}
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Update properties */
	if (xmlElement && xmlString)
	{ 
		/*
		 * Update the result of execution
		 */
		
		if ([xmlElement isEqualToString:@"message"])
		{ 
			[self setRepairStatusString:xmlString]; 
			[self setRepairStatusIcon:[NSImage imageNamed:@"ok_48.tif"]];
		}
		else if ([xmlElement isEqualToString:@"error"])
		{ 
			[self setRepairStatusString:xmlString]; 
			[self setRepairStatusIcon:[NSImage imageNamed:@"stop_48.tif"]];
		}
		else if ([xmlElement isEqualToString:@"script_output"])
		{
			[self setRepairScriptOutput:xmlString];
		}
	}
	
	/* Release current element */
	[xmlElement release];
	xmlElement = nil;
	[xmlString release];
	xmlString = nil;
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Refresh list */
	[scriptList highPriorityRefresh];
	
	/* Release request */
	if (sender == deleteRequest)
	{
		deleteRequest = nil;
	}
	else if (sender == repairRequest)
	{
		repairRequest = nil;
		[self setRepairInProgress:NO];
		if (!repairStatusString)
		{
			[self setRepairStatusString:@"Failed to request repair operation."]; 
			[self setRepairStatusIcon:[NSImage imageNamed:@"stop_48.tif"]];
		}
	}
	[sender release];
}


#pragma mark "Script Selection"

- (LCServiceScript *) selectedScript
{
	if ([[scriptArrayController selectedObjects] count] < 1) return nil;
	return [[scriptArrayController selectedObjects] objectAtIndex:0];
}

- (LCServiceScript *) selectedCommunityScript
{
	if ([[communityScriptArrayController selectedObjects] count] < 1) return nil;
	return [[communityScriptArrayController selectedObjects] objectAtIndex:0];
}

#pragma mark "Accessors"

- (LCServiceScriptList *) scriptList
{ return scriptList; }

- (void) setScriptList:(LCServiceScriptList *)list
{
	[scriptList release];
	scriptList = [list retain];
}

- (LCScriptUploadRequest *) uploadRequest
{ return uploadRequest; }

- (void) setUploadRequest:(LCScriptUploadRequest *)request
{
	[uploadRequest release];
	uploadRequest = [request retain];
}

- (LCScriptDownloadRequest *) downloadRequest
{ return downloadRequest; }

- (void) setDownloadRequest:(LCScriptDownloadRequest *)request
{
	[downloadRequest release];
	downloadRequest = [request retain];
}

- (BOOL) repairInProgress
{ return repairInProgress; }
- (void) setRepairInProgress:(BOOL)flag
{ repairInProgress = flag; }

- (NSString *) repairStatusString
{ return repairStatusString; }
- (void) setRepairStatusString:(NSString *)string
{
	[repairStatusString release];
	repairStatusString = [string retain];
}

- (NSString *) repairScriptOutput
{ return repairScriptOutput; }
- (void) setRepairScriptOutput:(NSString *)string
{ 
	[repairScriptOutput release];
	repairScriptOutput = [string retain];
}

- (NSImage *) repairStatusIcon
{ return repairStatusIcon; }
- (void) setRepairStatusIcon:(NSImage *)icon
{
	[repairStatusIcon release];
	repairStatusIcon = [icon retain];
}

- (LCServiceScript *) scriptBeingRepaired
{ return scriptBeingRepaired; }
- (void) setScriptBeingRepaired:(LCServiceScript *)script
{ scriptBeingRepaired = script; }

- (BOOL) scriptInstallInProgress
{ return scriptInstallInProgress; }
- (void) setScriptInstallInProgress:(BOOL)flag
{ scriptInstallInProgress = flag; }

- (NSString *) scriptInstallProgressString
{ return scriptInstallProgressString; }
- (void) setScriptInstallProgressString:(NSString *)string
{
	[scriptInstallProgressString release];
	scriptInstallProgressString = [string retain];
}


@synthesize setupController;
@synthesize uploadSheet;
@synthesize downloadSheet;
@synthesize scriptArrayController;
@synthesize communityScriptArrayController;
@synthesize repairSheet;
@synthesize communityDownloadSheet;
@synthesize localScriptsTableView;
@synthesize communityScriptsTableView;
@synthesize urlConn;
@synthesize receivedData;
@synthesize scriptBeingInstalled;
@synthesize localScriptsFilter;
@synthesize deleteRequest;
@synthesize repairRequest;
@synthesize xmlString;
@synthesize xmlElement;
@end
