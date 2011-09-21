//
//  LCUpdateController.m
//  LCAdminTools
//
//  Created by James Wilson on 11/05/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LCUpdateController.h"
#import "LCHelperToolUpdate.h"

@implementation LCUpdateController

#pragma mark "Constructor"

- (void) awakeFromNib
{
	self.userCanCancel = YES;
	[contentTabView selectTabViewItemWithIdentifier:@"noupdate"];
	[noUpdateMessage setStringValue:@""];
}

#pragma mark "Check for Update Methods"

- (IBAction) checkForUpdateClicked:(id)sender
{
	/* Show content tab */
	[contentTabView selectTabViewItemWithIdentifier:@"check"];
	
	/* Download version information */
	NSMutableURLRequest *urlReq = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:@"https://secure.lithiumcorp.com.au/appcast/core_osx.xml"]
														   cachePolicy:NSURLRequestReloadIgnoringCacheData
													   timeoutInterval:10.0];
	checkUrlConn = [[NSURLConnection connectionWithRequest:urlReq delegate:self] retain];
	if (checkUrlConn)
	{
		/* Connection proceeding */
		checkData = [[NSMutableData data] retain];
		[checkProgressIndicator startAnimation:self];
	}
	else
	{ 
		/* Connection failed */
		[self updateCheckFailed];
		return;
	}	
	
}

- (IBAction) moreInfoClicked:(id)sender
{
	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://lithiumcorp.com/lithium5/download-lithium/"]];
}

- (void) connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
	/* Received non-data response */
	[checkData setLength:0];
}

-(void)	connection:(NSURLConnection *) connection didReceiveData:(NSData *) data
{
	/* Received the actual data */
	[checkData appendData:data];
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
	/* Download finished, parse the XML */
	NSXMLParser *parser;
	
	/* Validate */
	NSString *xml = [[[NSString alloc] initWithData:checkData encoding:NSUTF8StringEncoding] autorelease];
	if ([xml hasPrefix:@"<?xml"])
	{
		/* Parse XML doc */
		parser = [[[NSXMLParser alloc] initWithData:checkData] autorelease];
		[parser setDelegate:self];
		[parser setShouldResolveExternalEntities:YES];
		[parser parse];
		
		/* Validate */
		[self validateVersion];		
	}
	else
	{
		[self updateCheckFailed];
	}
	
	/* Release data */
	[checkData release];
	checkData = nil;
	
	/* Release connection */
	checkUrlConn= nil;
	[connection release];
}

#pragma mark "XML Parser Delegate Methods"

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
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
	if (xmlString) { [xmlString appendString:string]; }
	else { xmlString = [[NSMutableString stringWithString:string] retain]; }
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Update properties */
	if ([element isEqualToString:@"version"]) self.updateVersion = xmlString;
	else if ([element isEqualToString:@"build"]) self.updateBuild = [xmlString intValue];
	else if ([element isEqualToString:@"date"]) self.updateDate = xmlString;
	else if ([element isEqualToString:@"size"]) self.updateSize = [xmlString intValue];
	else if ([element isEqualToString:@"url"]) self.updateUrl = xmlString;
	else if ([element isEqualToString:@"relnotes"]) self.updateNotesUrl = xmlString;
	
	/* Cleanup */
	if (xmlString)
	{
		[xmlString release];
		xmlString = nil;
	}
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	/* Release data */
	[checkData release];
	checkData = nil;
	
	/* Release connection */
	checkUrlConn = nil;
	[connection release];

	/* Show fail */
	[self updateCheckFailed];
}

- (void) updateCheckFailed
{
	[checkProgressIndicator stopAnimation:self];

	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert addButtonWithTitle:@"Cancel"];
	[alert setMessageText:@"Failed to check for update"];
	[alert setInformativeText:@"Lithium Core Admin was unable to reach lithium5.com to check for update availability."];
	[alert setAlertStyle:NSWarningAlertStyle];
	[alert beginSheetModalForWindow:window
					  modalDelegate:nil
					 didEndSelector:nil
						contextInfo:nil];
	[noUpdateMessage setStringValue:@"Unable to reach Lithium Software Update Server"];
	[contentTabView selectTabViewItemWithIdentifier:@"noupdate"];	
}

- (void) validateVersion
{
	/* Check to see if the update is valid */
	if ([[[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"] intValue] < self.updateBuild)
	{
		/* Update is newer and valid */
		[contentTabView selectTabViewItemWithIdentifier:@"update"];
		[notesWebView setMainFrameURL:self.updateNotesUrl];
	}
	else
	{
		/* Current version is up to date */
		[contentTabView selectTabViewItemWithIdentifier:@"noupdate"];
		[noUpdateMessage setStringValue:@"Lithium Core is up to date."];
	}	
	
	[checkProgressIndicator stopAnimation:self];
}

@synthesize updateVersion;
@synthesize updateBuild;
@synthesize updateDate;
@synthesize updateSize;
@synthesize updateUrl;
@synthesize updateNotesUrl;

- (NSString *) localVersion
{ return [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"]; }

- (int) localBuild
{ return [[[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"] intValue]; }

#pragma mark "Install Update Methods"

- (IBAction) installUpdateClicked:(id)sender
{
	/* Show sheet to confirm */	
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert addButtonWithTitle:@"OK"];
	[alert addButtonWithTitle:@"Cancel"];
	[alert setMessageText:@"Requires restart of Lithium Core"];
	[alert setInformativeText:@"The Lithium Core update process will require a restart of Lithium Core. Monitoring and status information will be unavailable whilst Lithium Core is restarted."];
	[alert setAlertStyle:NSWarningAlertStyle];
	[alert beginSheetModalForWindow:window
					  modalDelegate:self
					 didEndSelector:@selector(updateConfirmDidEnd:returnCode:contextInfo:)
						contextInfo:nil];
	
}

- (void) updateConfirmDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	/* Check return code */
	if (returnCode == 1000)
	{
		/* Begin the download */
		NSURLRequest *urlRequest = [NSURLRequest requestWithURL:[NSURL URLWithString:self.updateUrl]
													cachePolicy:NSURLRequestReloadIgnoringCacheData
												timeoutInterval:30.0];
		updateDownload = [[NSURLDownload alloc] initWithRequest:urlRequest delegate:self];
		if (updateDownload)
		{
			/* Set path */
			[[NSFileManager defaultManager] createDirectoryAtPath:[@"~/Library/Caches/com.lithiumcorp.CoreAdmin" stringByExpandingTildeInPath]
													   attributes:nil];
			NSString *path = [NSString stringWithFormat:@"~/Library/Caches/com.lithiumcorp.CoreAdmin/%@", [self.updateUrl lastPathComponent]];
			[updateDownload setDestination:[path stringByExpandingTildeInPath] allowOverwrite:YES];

			/* Show sheet */
			self.updateProgress = 0.0;
			self.updateStatus = @"Starting Download...";
			self.updateOperation = @"Downloading Lithium Core Update...";
			[updateProgressIndicator setIndeterminate:NO];
			[[alert window] orderOut:self];
			[NSApp beginSheet:progressSheet
			   modalForWindow:window
				modalDelegate:self
			   didEndSelector:nil
				  contextInfo:nil];			
		}
		else
		{
			[self downloadUpdateFailed];
		}

	}
}

- (void)download:(NSURLDownload *)download didReceiveResponse:(NSURLResponse *)response
{
    // reset the progress, this might be called multiple times
    updatebytesReceived = 0;
	
    // retain the response to use later
	updateDownloadResponse = [response retain];
}

- (void)download:(NSURLDownload *)download didReceiveDataOfLength:(NSUInteger)length
{
	/* Update percentage */
    long expectedLength=[updateDownloadResponse expectedContentLength];	
    updatebytesReceived = updatebytesReceived + length;
    if (expectedLength != NSURLResponseUnknownLength) 
	{
        // if the expected content length is
        // available, display percent complete
		self.updateProgress = (updatebytesReceived/(float)expectedLength)*100.0;
    } 
	else
	{ 
		self.updateProgress = 0.0; 
		[updateProgressIndicator setIndeterminate:YES];
	}
	
	/* Update Mb Total */
	self.updateMbReceived = updatebytesReceived / (1024.0 * 1024.0);
	self.updateStatus = [NSString stringWithFormat:@"Downloaded %.2fMb", self.updateMbReceived];
}

- (void)download:(NSURLDownload *)download didFailWithError:(NSError *)error
{
	/* Clean up */
	[updateDownload release];
	updateDownload = nil;
	[updateDownloadResponse release];
	updateDownloadResponse = nil;
	[self downloadUpdateFailed];
}

- (void)downloadDidFinish:(NSURLDownload *)download
{
	/* Install the update */
	NSString *cachePath = [@"~/Library/Caches/com.lithiumcorp.CoreAdmin" stringByExpandingTildeInPath];
	NSString *archivePath = [NSString stringWithFormat:@"%@/%@", cachePath, [self.updateUrl lastPathComponent]];
	NSString *packagePath = [archivePath stringByReplacingOccurrencesOfString:@".zip" withString:@".pkg"];
	[[LCHelperToolUpdate alloc] initWithArchive:archivePath extractTo:cachePath containingPackage:packagePath delegate:self];
	self.updateOperation = @"Installing Lithium Core Update...";
	self.userCanCancel = NO;
	
	/* Clean up */
	[updateDownloadResponse release];
	updateDownloadResponse = nil;
	[updateDownload release];
	updateDownload = nil;
}

- (void) helperTool:(LCHelperTool *)helper progressUpdate:(float)progress
{
	self.updateProgress = progress;
}

- (void) helperTool:(LCHelperTool *)helper statusUpdate:(NSString *)status
{
	self.updateStatus = status;
}

- (void) helperToolDidFinish:(LCHelperTool *)helper
{
	self.userCanCancel = YES;
	[NSApp endSheet:progressSheet];
	[progressSheet close];
	[helper autorelease];
	[contentTabView selectTabViewItemWithIdentifier:@"updated"];
	self.recentlyUpdated = YES;
}

- (void) helperToolDidFail:(LCHelperTool *)helper
{
	self.userCanCancel = YES;
	[self installUpdateFailed];
	[helper autorelease];
}

- (IBAction) installUpdateCancelClicked:(id)sender
{
	/* Cancel the in-progress update */
	if (updateDownload)
	{ 
		[updateDownload cancel];
		[updateDownload release];
		updateDownload = nil;
	}
	
	/* Close sheet */
	[NSApp endSheet:progressSheet];
	[progressSheet close];	
}

- (void) downloadUpdateFailed
{
	[NSApp endSheet:progressSheet];
	[progressSheet close];
	
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert addButtonWithTitle:@"Cancel"];
	[alert setMessageText:@"Failed to download update"];
	[alert setInformativeText:@"Lithium Core Admin was unable to download the update from lithium5.com"];
	[alert setAlertStyle:NSWarningAlertStyle];
	[alert beginSheetModalForWindow:window
					  modalDelegate:nil
					 didEndSelector:nil
						contextInfo:nil];
	
	[noUpdateMessage setStringValue:@"Failed to download update."];
	[contentTabView selectTabViewItemWithIdentifier:@"noupdate"];	
}

- (void) installUpdateFailed
{
	[NSApp endSheet:progressSheet];
	[progressSheet close];
	
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert addButtonWithTitle:@"Cancel"];
	[alert setMessageText:@"Failed to install update"];
	[alert setInformativeText:@"Lithium Core Admin failed to install the Lithium Core update."];
	[alert setAlertStyle:NSWarningAlertStyle];
	[alert beginSheetModalForWindow:window
					  modalDelegate:nil
					 didEndSelector:nil
						contextInfo:nil];
	
	[noUpdateMessage setStringValue:@"Failed to install update."];
	[contentTabView selectTabViewItemWithIdentifier:@"noupdate"];	
}

@synthesize updateProgress;
@synthesize updateMbReceived;
@synthesize updateStatus;
@synthesize userCanCancel;
@synthesize updateOperation;
@synthesize recentlyUpdated;

@end

