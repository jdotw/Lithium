//
//  LCSSceneImportDocument.m
//  Lithium Console
//
//  Created by James Wilson on 26/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCSSceneImportDocument.h"
#import "LCSSceneDocument.h"

@implementation LCSSceneImportDocument

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{    
	NSDictionary *properties = [NSKeyedUnarchiver unarchiveObjectWithData:data];
	
	LCSSceneDocument *scene = [[LCSSceneDocument new] autorelease];
	scene.desc = [[[self fileName] lastPathComponent] stringByDeletingPathExtension];
	scene.backgroundImage = [properties objectForKey:@"image"];
	
	for (LCSSceneOverlay *overlay in [properties objectForKey:@"overlays"])
	{
		[scene insertObject:overlay inOverlaysAtIndex:scene.overlays.count];
	}
	
	self.document = (LCDocument *)scene;
	
	return YES;
}

@end
