//
//  MBSnmpWalk.m
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "ModuleDocument.h"
#import "MBSnmpWalk.h"
#import "MBOid.h"
#import "MBEnum.h"
#import "MBDocumentWindowController.h"

#pragma mark "Core Foundation Callbacks"

void walk_socket_callback (CFSocketRef s, CFSocketCallBackType callbackType, CFDataRef address, const void *data, void *info)
{
	MBSnmpWalk *walk = info;
	[walk socketCallback];
}

int walk_pdu_callback (int operation, struct snmp_session *ss, int reqid, struct snmp_pdu *pdu, void *magic)
{
	MBSnmpWalk *walk = magic;
	
	[walk processPDU:pdu reqid:reqid operation:operation];
	
	return 1; /* Keep libsnmp happy */
}

@implementation MBSnmpWalk

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	self.snmpVersion = SNMP_VERSION_2c;
	self.bulkwalk = YES;
	self.community = @"public";
	self.retries = 1;
	self.timeoutSeconds = 30;
	self.replaceExistingScan = YES;
	
	return self;
}

- (void) dealloc
{
	[community release];
	[ip release];
	[startOid release];
	
	[super dealloc];
}

#pragma mark Socket API

- (void) resetSocketTimeout
{
	/* Remove old */
	[timeoutTimer invalidate];
	[timeoutTimer release];
	timeoutTimer = nil;

	/* Add timeout timer */
	int fds = 0, block = 1;
	struct timeval timeout = { 0, 0 };
	fd_set read_fds;
	FD_ZERO (&read_fds);
	snmp_select_info (&fds, &read_fds, &timeout, &block);
	if (block == 0)
	{
		timeoutTimer = [[NSTimer scheduledTimerWithTimeInterval:((float)timeout.tv_sec + ((float)timeout.tv_usec / 1000000))
													   target:self
													   selector:@selector(timeoutCallback:)
													 userInfo:nil
													  repeats:NO] retain]; 
	}
}

- (void) socketCallback
{
	/* Called when there is data to be read */
	[timeoutTimer invalidate];
	[timeoutTimer release];
	timeoutTimer = nil;
	
	/* Read SNMP data */
	int fds = 0;
	fd_set read_fds;
	FD_ZERO (&read_fds);
	struct timeval timeout = { 0, 0 };
	int block = 1;
	snmp_select_info (&fds, &read_fds, &timeout, &block);
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	select (fds, &read_fds, NULL, NULL, &timeout);
	snmp_read (&read_fds);

	if (shouldTearDown)
	{ 
		[self teardown]; 
	}
}

#pragma mark Timeout Handling

- (void) timeoutCallback:(NSTimer *)timer
{
	[timeoutTimer release];
	timeoutTimer = nil;
	if (sessionOpen) 
	{
		snmp_timeout ();
		[self resetSocketTimeout];
		if (shouldTearDown)
		{
			[self teardown]; 
		}		
	}
}

#pragma mark SNMP API

- (void) processPDU:(struct snmp_pdu *)pdu reqid:(int)reqid operation:(int)operation
{
	if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
	{
		/* Process a PDU */
		if (pdu && pdu->errstat == SNMP_ERR_NOERROR)
		{
			/* Process PDU */
			BOOL endOfMib = NO;
			struct variable_list *vars;
			int oidCount = 0;
			for (vars = pdu->variables; vars; vars = vars->next_variable)
			{
				if ((vars->type != SNMP_ENDOFMIBVIEW) && (vars->type != SNMP_NOSUCHOBJECT) && (vars->type != SNMP_NOSUCHINSTANCE))
				{
					/* Add the OID */
					u_char *name_buf;
					size_t buf_len = 1024;
					name_buf = malloc (buf_len);
					size_t out_len = 0;
					int buf_overflow = 0;
					netsnmp_sprint_realloc_objid_tree (&name_buf, &buf_len, &out_len, 0, &buf_overflow, vars->name, vars->name_length);
					out_len = 0;
					buf_overflow = 0;
					buf_len = 1024;
					u_char *value_buf = malloc (buf_len);
					struct tree *subtree = get_tree_head();
					subtree = get_tree(vars->name, vars->name_length, subtree);					
					sprint_realloc_by_type(&value_buf, &buf_len, &out_len, 1, vars, 
										   subtree ? subtree->enums : 0, 
										   subtree ? subtree->hint : 0, 
										   subtree ? subtree->units : 0);
					MBOid *oid = [MBOid oidNamed:[NSString stringWithCString:(char *)name_buf
																	encoding:NSUTF8StringEncoding]
										   value:[NSString stringWithCString:(char *)value_buf
																	encoding:NSUTF8StringEncoding]
										 pduType:(int)vars->type
										   objid:vars->name objid_len:vars->name_length];
					if (subtree)
					{
						/* Store enums, set units, etc */
						struct enum_list *enums = subtree->enums;
						for (; enums; enums = enums->next)
						{
							MBEnum *snmpEnum = [MBEnum enumWithLabel:[NSString stringWithCString:(char *)enums->label encoding:NSUTF8StringEncoding]
															 forValue:[NSString stringWithFormat:@"%i", enums->value]];
							[oid insertObject:snmpEnum inEnumeratorsAtIndex:oid.enumerators.count];
						}
					}
					
					/* Check OID is part of root */
					if ((vars->name_length < root_length) || (memcmp(root, vars->name, root_length * sizeof(oid)) != 0))
					{
						/* Received OID is not part of subtree end of walk */
						endOfMib = YES;
					}
					else
					{
						/* Part of root tree, Add to OID list */
						[document insertObject:oid inOidsAtIndex:[document countOfOids]];
						oidCount++;
						
						/* Check OID is increasing */
						if (snmp_oid_compare(name, name_length, vars->name, vars->name_length) >= 0)
						{
							NSLog (@"OID Not increasing, ending walk.");
							endOfMib = YES;
						}
						else 
						{
							/* Move to next */
							memmove (name, (char *)vars->name, vars->name_length * sizeof(oid));
							name_length = vars->name_length;							
						}						
					}
					
				}
				else
				{
					endOfMib = YES;
				}
			}
			self.oidsReceived = self.oidsReceived + oidCount;
			if (endOfMib)
			{
				/* End of Walk */
				shouldTearDown = YES;
				[(MBDocumentWindowController *)delegate scanDidFinished:YES error:nil];
			}
			else
			{
				[self getNext];					
			}
		}
		else
		{
			/* Error or no PDU */
			shouldTearDown = YES;
			if (pdu)
			{
				/* PDU Received */
				if (pdu->errstat == SNMP_ERR_NOSUCHNAME)
				{
					/* End of MIB */
					[(MBDocumentWindowController *)delegate scanDidFinished:YES error:nil];					
				}
				else
				{
					/* Error */
					[(MBDocumentWindowController *)delegate scanDidFinished:NO error:[NSString stringWithCString:snmp_errstring(pdu->errstat)
																										encoding:NSUTF8StringEncoding]];
				}
			}
			else 
			{
				/* No PDU! */
				[(MBDocumentWindowController *)delegate scanDidFinished:NO error:@"No response from host."];		
			}

		}
	}
	else if (NETSNMP_CALLBACK_OP_TIMED_OUT)
	{
		/* Timeout occurred */
		shouldTearDown = YES;
		[(MBDocumentWindowController *)delegate scanDidFinished:NO error:@"Timeout occurred"];
	}
	else if (NETSNMP_CALLBACK_OP_SEND_FAILED)
	{
		/* Send failed */
		shouldTearDown = YES;
		[(MBDocumentWindowController *)delegate scanDidFinished:NO error:@"Failed to send request to host"];		
	}
	else
	{
		/* Other error */
		shouldTearDown = YES;
		[(MBDocumentWindowController *)delegate scanDidFinished:NO error:@"Other non-timeout error occurred"];		
	}
}

- (void) startWalk
{
	/* Resolve starting OID */
	oid objid_mib[] = { 1, 3, 6, 1 };
	if ([self.startOid length] > 0)
	{
		/* User-specified start OID 
		 * For reasons unknown, the parse MUST be done TWICE to succeed
		 */
		
		root_length = MAX_OID_LEN;
		if (snmp_parse_oid([self.startOid cStringUsingEncoding:NSUTF8StringEncoding], root, &root_length) == NULL)
		{
			snmp_perror([self.startOid cStringUsingEncoding:NSUTF8StringEncoding]);

			/* Failed to parse, try again */
			if (snmp_parse_oid([self.startOid cStringUsingEncoding:NSUTF8StringEncoding], root, &root_length) == NULL)
			{
				/* Second failure */
				snmp_perror([self.startOid cStringUsingEncoding:NSUTF8StringEncoding]);
				return;
			}
		}
	}
	else 
	{
		/* Default to std + enterprises */
		memmove(root, objid_mib, sizeof(objid_mib));
		root_length = sizeof(objid_mib) / sizeof(oid);
	}
	
	/* Open SNMP Session */
	shouldTearDown = NO;
	struct snmp_session ss;
	snmp_sess_init (&ss);
	ss.peername = (char *) [self.ip UTF8String];
	ss.community = (u_char *) [self.community UTF8String];
	ss.community_len = strlen([self.community UTF8String]);
	ss.version = self.snmpVersion;
	ss.callback = walk_pdu_callback;
	ss.callback_magic = self;
	ss.timeout = (timeoutSeconds * 1000000L) / (retries + 1);
	ss.retries = retries;
	session = snmp_open (&ss);
	if (session) 
	{
		self.sessionOpen = YES;
	}
	else 
	{
		self.sessionOpen = NO;
		return;
	}

	/* Start Walk */
	memmove (name, root, root_length * sizeof(oid));
	name_length = root_length;
	[self getNext];
	if (shouldTearDown == NO)
	{
		/* Walk started */
		self.walkInProgress = YES;
	
		/* Create CFSocket ref */
		CFSocketContext context = { 0, self, NULL, NULL, NULL };
		int fds = 0, block = 1;
		struct timeval timeout = { 0, 0 };
		fd_set read_fds;
		FD_ZERO (&read_fds);
		snmp_select_info (&fds, &read_fds, &timeout, &block);	
		sockref = CFSocketCreateWithNative(NULL, fds-1, kCFSocketReadCallBack, walk_socket_callback, &context);
		CFRunLoopSourceRef rlSource = CFSocketCreateRunLoopSource(NULL, sockref, 0);
		CFRunLoopAddSource (CFRunLoopGetCurrent(), rlSource, kCFRunLoopCommonModes);	
		CFRelease(rlSource);
	}
	else 
	{
		/* Walk did not start */
		snmp_close (session);
		session = NULL;
		self.sessionOpen = NO;
		self.walkInProgress = NO;
		[timeoutTimer invalidate];
		timeoutTimer = nil;
	}
}

- (void) getNext
{
	struct snmp_pdu *request = NULL;
	if (session->version == SNMP_VERSION_2c && self.bulkwalk)
	{
		request = snmp_pdu_create (SNMP_MSG_GETBULK); 
		request->non_repeaters = 0;
		request->max_repetitions = 10;
	}
	else
	{
		request = snmp_pdu_create (SNMP_MSG_GETNEXT);
	}
	snmp_add_null_var (request, name, name_length);
	int reqid = snmp_send (session, request);
	if (reqid == 0)
	{
		snmp_perror ("SNMP :");
		/* Error */
		shouldTearDown = YES;
		[delegate scanDidFinished:NO error:@"Error sending GET_NEXT packet"];
		return;
	}
	[self resetSocketTimeout];
}

- (void) cancel
{
	/* Cancel the walk */
	[self teardown];
	[(MBDocumentWindowController *)delegate scanDidFinished:YES error:@""];
}

- (void) teardown
{
	[timeoutTimer invalidate];
	[timeoutTimer release];
	timeoutTimer = nil;
	self.sessionOpen = NO;
	self.walkInProgress = NO;
	
	if (sockref)
	{
		CFSocketInvalidate (sockref);
		CFRelease(sockref);
		sockref = nil;
	}
	
	snmp_close (session);
	session = NULL;
	
	[document updateChangeCount:NSChangeDone];
}

#pragma mark Accessors

@synthesize delegate, document, walkInProgress, oidsReceived, replaceExistingScan;
@synthesize ip, community, snmpVersion, bulkwalk, startOid;
@synthesize timeoutSeconds, retries, sessionOpen, canUseBulkWalk;
- (void) setSnmpVersion:(long)value
{
	snmpVersion = value;
	if (value == SNMP_VERSION_2c) self.canUseBulkWalk = YES;
	else self.canUseBulkWalk = NO;
}
- (void) setStartOid:(NSString *)value
{
	[startOid release];
	startOid = [value copy];
	if ([startOid length]) self.replaceExistingScan = NO;
	else self.replaceExistingScan = YES;
}

@end
