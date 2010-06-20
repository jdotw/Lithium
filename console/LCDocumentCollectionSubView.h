//
//  LCDocumentCollectionSubView.h
//  Lithium Console
//
//  Created by James Wilson on 19/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCDocumentCollectionSubView : NSView 
{
	BOOL m_isSelected;
	IBOutlet id m_theDelegate;
}

-(id)delegate;
-(void)setDelegate:(id)theDelegate;
-(void)setSelected:(BOOL)flag;
-(BOOL)selected;

@end

@interface NSObject (LCDocumentCollectionSubViewDelegate)
-(void)collectionViewDoubleClicked:(id)sender;
@end