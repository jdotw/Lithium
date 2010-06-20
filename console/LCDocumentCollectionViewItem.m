//
//  LCDocumentCollectionViewItem.m
//  Lithium Console
//
//  Created by James Wilson on 19/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDocumentCollectionViewItem.h"

#import "LCDocumentCollectionSubView.h"
#import "LCSSceneDocument.h"
#import "LCVRackDocument.h"

@implementation LCDocumentCollectionViewItem

- (void) setRepresentedObject:(id)object
{
	[super setRepresentedObject:object];
}

-(void)setSelected:(BOOL)flag 
{
	[super setSelected:flag];
	
	// tell the view that it has been selected
	LCDocumentCollectionSubView* theView = (LCDocumentCollectionSubView *) [self view];
	if([theView isKindOfClass:[LCDocumentCollectionSubView class]]) 
	{
		[theView setSelected:flag];
		[theView setNeedsDisplay:YES];
	}
}

@synthesize delegate;

@end
