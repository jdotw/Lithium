//
//  LCDocumentBreadcrumView.h
//  Lithium Console
//
//  Created by James Wilson on 18/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCDocumentBreadcrumView : NSView
{
	NSMutableArray *buttons;
	
	NSMutableArray *items;
	BOOL editing;
	
	id delegate;
}

@property (nonatomic, assign) id delegate;
@property (readonly) NSMutableArray *items;
- (void) insertObject:(id)obj inItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromItemsAtIndex:(unsigned int)index;
@property (nonatomic, assign) BOOL editing;

@end
