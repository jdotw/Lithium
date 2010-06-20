//
//  LCVRackImportDocument.m
//  Lithium Console
//
//  Created by James Wilson on 26/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCVRackImportDocument.h"


@implementation LCVRackImportDocument

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{    
	NSDictionary *properties = [NSKeyedUnarchiver unarchiveObjectWithData:data];

	LCVRackDocument *vrack = [[LCVRackDocument new] autorelease];
	vrack.desc = [[[self fileName] lastPathComponent] stringByDeletingPathExtension];
	
	for (LCVRackDevice *rDev in [properties objectForKey:@"devices"])
	{
		[vrack insertObject:rDev inDevicesAtIndex:vrack.devices.count];
	}
	for (LCVRackCableGroup *cGroup in [properties objectForKey:@"cableGroups"])
	{
		[vrack insertObject:cGroup inCableGroupsAtIndex:vrack.cableGroups.count];
	}
	for (LCVRackCable *cable in [properties objectForKey:@"cables"])
	{
		[vrack insertObject:cable inCablesAtIndex:vrack.cables.count];
	}
	[vrack bindCablestoCableGroups];
	
	self.document = (LCDocument *)vrack;
	
	return YES;
}

@end
