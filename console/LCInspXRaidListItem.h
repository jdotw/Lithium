//
//  LCInspXRaidListItem.h
//  Lithium Console
//
//  Created by James Wilson on 17/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorItem.h"

@interface LCInspXRaidListItem : LCInspectorItem {

}

- (void) createViewControllersForObjects:(NSArray *)objArray metaDataObjects:(NSArray *)metadataObjArray;

@end
