//
//  MBOIDCell.h
//  ModuleBuilder
//
//  Created by James Wilson on 25/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCShadowTextFieldCell.h"
#import "MBOid.h"

@interface MBOIDCell : LCShadowTextFieldCell
{
	MBOid *drawnOid;
}

@property (retain) MBOid *drawnOid;

@end
