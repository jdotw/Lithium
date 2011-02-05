//
//  LTIncidentListGroup.h
//  Lithium
//
//  Created by James Wilson on 16/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface LTIncidentListGroup : NSObject 
{
	NSMutableArray *children;
	NSString *title;
}

@property (readonly) NSMutableArray *children;
@property (copy) NSString *title;
@property (readonly) int highestEntityState;


@end
