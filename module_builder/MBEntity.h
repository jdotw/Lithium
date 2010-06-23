//
//  MBEntity.h
//  ModuleBuilder
//
//  Created by James Wilson on 12/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MBEntity : NSObject 
{
	id document;
	NSMutableDictionary *properties;
	NSMutableArray *children;
	MBEntity *parent;
	
	IBOutlet NSView *view;
}

#pragma mark "Constructors"
- (id) init;
- (id) initWithCoder:(NSCoder *)decoder;
- (void) encodeWithCoder:(NSCoder *)encoder;
- (void) dealloc;
- (MBEntity *) copy;

#pragma mark "Properties"
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)dict;

#pragma mark "Children" 
- (NSMutableArray *) children;
- (void) setChildren:(NSMutableArray *)array;
- (void) insertObject:(MBEntity *)entity inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
- (void) moveChild:(MBEntity *)child toIndex:(unsigned int)index;
- (unsigned int) countOfChildren;
- (MBEntity *) childNamed:(NSString *)desc;
- (MBEntity *) container;
- (MBEntity *) metric;
- (MBEntity *) trigger;

#pragma mark "XML Output"
- (NSString *) xmlNodeName;
- (NSString *) xmlChildrenNodeName;
- (NSXMLNode *) xmlNode;

#pragma mark "View Controller"
- (NSViewController *) viewController;
@property (readonly) float rowHeight;

#pragma mark "Dependent Metric Binding"
- (void) bindDependents;

#pragma mark "General Accessors"
- (id) document;
- (void) setDocument:(id)newDocument;
- (NSString *) desc;
- (void) setDesc:(NSString *)string;
- (NSNumber *) type;
- (void) setType:(NSNumber *)type;
- (NSString *) typeString;
- (MBEntity *) parent;
- (void) setParent:(MBEntity *)value;
- (BOOL) infoViewVisible;
- (void) setInfoViewVisible:(BOOL)flag;


@end
