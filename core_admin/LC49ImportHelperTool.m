//
//  LC49ImportHelperTool.m
//  LCAdminTools
//
//  Created by James Wilson on 2/06/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LC49ImportHelperTool.h"


@implementation LC49ImportHelperTool

#pragma mark "Constructors"

- (id) initWithDelegate:(id)initDelegate
{
	[super init];
	
	self.delegate = initDelegate;
	
	return self;
}

#define mark "Commands"

- (void) allStop 
{
	operation = OP_ALL_STOP;
	[self startHelperToolCommand:@"all" arguments:[NSArray arrayWithObject:@"stop"]];
}

- (void) allStart
{
	operation = OP_ALL_START;
	[self startHelperToolCommand:@"all" arguments:[NSArray arrayWithObject:@"start"]];
}

- (void) reinstateL49Root
{
	operation = OP_L49ROOT_REINSTATE;
	[self startHelperToolCommand:@"l49_reinstate_root" arguments:[NSArray array]];
}

- (void) startL49Database
{
	operation = OP_L49DB_START;
	[self startHelperToolCommand:@"l49_database_start" arguments:[NSArray array]];	
}

- (void) exportL49Database
{
	operation = OP_L49DB_EXPORT;
	[self startHelperToolCommand:@"l49_database_export" arguments:[NSArray array]];		
}

- (void) stopL49Database
{
	operation = OP_L49DB_STOP;
	[self startHelperToolCommand:@"l49_database_stop" arguments:[NSArray array]];		
}

- (void) startL50Database
{
	operation = OP_L50DB_START;
	[self startHelperToolCommand:@"l50_database_start" arguments:[NSArray array]];
}

- (void) importL50Database
{
	operation = OP_L50DB_IMPORT;
	[self startHelperToolCommand:@"l50_database_import" arguments:[NSArray array]];
}

- (void) stopL50Database
{
	operation = OP_L50DB_STOP;
	[self startHelperToolCommand:@"l50_database_stop" arguments:[NSArray array]];
}

- (void) copyL49RRDFiles
{
	operation = OP_L49RRD_COPY;
	[self startHelperToolCommand:@"l49_rrd_copy" arguments:[NSArray array]];
}

- (void) copyL49ClientFiles
{
	operation = OP_L49CUST_CREATE;
	[self startHelperToolCommand:@"l49_clientd_copy" arguments:[NSArray array]];
}

- (void) renameL49Root
{
	operation = OP_L49ROOT_RENAME;
	[self startHelperToolCommand:@"l49_rename_root" arguments:[NSArray array]];
}

- (void) finalizeImport
{
	operation = OP_FINALIZE;
	[self startHelperToolCommand:@"l50_finalize" arguments:[NSArray array]];	
}

#pragma mark "Helper Process Communication"

- (BOOL) processDataFromHelper:(NSString *)data
{
	NSLog(@"%@ Helper Received: '%@' for operation=%i", self, data, operation);
	NSArray *lines = [data componentsSeparatedByString:[NSString stringWithFormat:@"\n"]];
	for (NSString *line in lines)
	{
		self.status = line;
		
		/* Reinstate Root */
		if (operation == OP_L49ROOT_REINSTATE)
		{
			if ([line hasPrefix:@"OK"])
			{
				[delegate performSelector:@selector(reinstateOldRootDidFinish:) withObject:self];
				return NO;
			}
		}		
		
		/* Old Database Start Operation */
		if (operation == OP_L49DB_START)
		{
			if ([line hasPrefix:@"LOG:  database system is ready"])
			{
				[delegate performSelector:@selector(oldDatabaseReady:) withObject:self];
				return NO;
			}
			else if ([line hasPrefix:@"FATAL"])
			{
				[delegate performSelector:@selector(oldDatabaseFailed:) withObject:self];
				return NO;
			}
		}
		
		/* Old Database Export Operation */
		if (operation == OP_L49DB_EXPORT)
		{
			if ([line hasPrefix:@"OK"])
			{
				[delegate performSelector:@selector(oldDatabaseExportDidFinish:) withObject:self];
				return NO;
			}
			else if ([line hasPrefix:@"ERROR"])
			{
				[delegate performSelector:@selector(oldDatabaseExportDidFail:) withObject:self];
				return NO;
			}
		}

		/* Old Database Stop Operation */
		if (operation == OP_L49DB_STOP)
		{
			if ([line hasPrefix:@"OK"])
			{
				[delegate performSelector:@selector(oldDatabaseStopDidFinish:) withObject:self];
				return NO;
			}
			else if ([line hasPrefix:@"ERROR"])
			{
				[delegate performSelector:@selector(oldDatabaseStopDidFail:) withObject:self];
				return NO;
			}
		}

		/* New Database Start Operation */
		if (operation == OP_L50DB_START)
		{
			if ([line hasPrefix:@"LOG:  database system is ready to accept connections"])
			{
				[delegate performSelector:@selector(newDatabaseStartDidFinish:) withObject:self];
				return NO;
			}
			else if ([line hasPrefix:@"FATAL"])
			{
				[delegate performSelector:@selector(newDatabaseStartDidFail:) withObject:self];
				return NO;
			}
		}

		
		/* New Database Import Operation */
		if (operation == OP_L50DB_IMPORT)
		{
			if ([line hasPrefix:@"OK"])
			{
				[delegate performSelector:@selector(newDatabaseImportDidFinish:) withObject:self];
				return NO;
			}
			else if ([line hasPrefix:@"ERROR"])
			{
				[delegate performSelector:@selector(newDatabaseImportDidFail:) withObject:self];
				return NO;
			}
		}		

		/* New Database Stop Operation */
		if (operation == OP_L50DB_STOP)
		{
			if ([line hasPrefix:@"OK"])
			{
				NSLog (@"%@ declaring OP_L50DB_STOP OK because '%@' has prefix 'OK'", self, line);
				[delegate performSelector:@selector(newDatabaseStopDidFinish:) withObject:self];
				return NO;
			}
			else if ([line hasPrefix:@"ERROR"])
			{
				NSLog (@"%@ declaring OP_L50DB_STOP FAILED because '%@' has prefix 'ERROR'", self, line);
				[delegate performSelector:@selector(newDatabaseStopDidFail:) withObject:self];
				return NO;
			}
		}		

		/* RRD Files Copy Operation */
		if (operation == OP_L49RRD_COPY)
		{
			if ([line hasSuffix:@"OK"])
			{
				[delegate performSelector:@selector(rrdFileCopyDidFinish:) withObject:self];
				return NO;
			}
			else if ([line hasPrefix:@"COUNT:"])
			{
				NSArray *components = [line componentsSeparatedByString:@":"];
				if (components.count > 1)
				{ [delegate performSelector:@selector(setRrdFileCopyTotal:) withObject:[NSNumber numberWithInt:[[components objectAtIndex:1] intValue]]]; }
				return YES;
			}
			else if ([line hasPrefix:@"customer-"])
			{
				[delegate performSelector:@selector(incrementRrdFileCopiedCount:) withObject:self];
			}
			else if ([line hasSuffix:@"ERROR"])
			{
				[delegate performSelector:@selector(rrdFileCopyDidFail:) withObject:self];
				return NO;
			}
		}	
		
		/* Client/Customer File Creation */
		if (operation == OP_L49CUST_CREATE)
		{
			if ([line hasSuffix:@"OK"])
			{
				[delegate performSelector:@selector(customerFileCreateDidFinish:) withObject:self];
				return NO;
			}
			else if ([line hasSuffix:@"ERROR"])
			{
				[delegate performSelector:@selector(customerFileCreateDidFail:) withObject:self];
				return NO;
			}
		}		
		
		/* All Stop */
		if (operation == OP_ALL_STOP)
		{
			if ([line hasPrefix:@"OK"])
			{
				[delegate performSelector:@selector(allStopDidFinish:) withObject:self];
				return NO;
			}
		}

		/* All Start */
		if (operation == OP_ALL_START)
		{
			if ([line hasPrefix:@"OK"])
			{
				[delegate performSelector:@selector(allStartDidFinish:) withObject:self];
				return NO;
			}
		}
		
		/* Rename Root */
		if (operation == OP_L49ROOT_RENAME)
		{
			if ([line hasPrefix:@"OK"])
			{
				[delegate performSelector:@selector(renameOldRootDidFinish:) withObject:self];
				return NO;
			}
		}
		
		/* Finalize */
		if (operation == OP_FINALIZE)
		{
			if ([line hasPrefix:@"OK"])
			{
				[delegate performSelector:@selector(finalizeImportDidFinish:) withObject:self];
				return NO;
			}			
		}
		
	}
	
	return YES;	
}

@end

