//
//  LCShadowTextFieldCell.h
//  Lithium Console
//
//  Created by James Wilson on 15/10/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCShadowTextFieldCell : NSTextFieldCell 
{
	NSShadow *theShadow;
	BOOL altRow;
	int colIndex;
}

@end
