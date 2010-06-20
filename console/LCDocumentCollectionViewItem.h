//
//  LCDocumentCollectionViewItem.h
//  Lithium Console
//
//  Created by James Wilson on 19/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface LCDocumentCollectionViewItem : NSCollectionViewItem 
{
	IBOutlet id delegate;
}

@property (assign) id delegate;

@end
