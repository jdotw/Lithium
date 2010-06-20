//
//  LTGroup.h
//  Lithium
//
//  Created by James Wilson on 18/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTEntity.h"
#import "LTCoreDeployment.h"

@interface LTGroup : LTEntity 
{
	int groupID;	
	int parentID;
}

- (void) refresh;

@property (assign) int groupID;
@property (assign) int parentID;

@end
