//
//  MBOidSection.h
//  ModuleBuilder
//
//  Created by James Wilson on 25/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MBOidSection : NSObject 
{
	NSMutableDictionary *properties;
}

+ (MBOidSection *) sectionFromProperties:(NSMutableDictionary *)initProperties;
- (MBOidSection *) initWithProperties:(NSMutableDictionary *)initProperties;

#pragma mark "Properties"
@property (copy) NSMutableDictionary *properties;
@property (copy) NSString *name;
@property (copy) NSMutableArray *children;
- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
@property (readonly) NSString *displayName;
@property (readonly) NSString *value;

@end
