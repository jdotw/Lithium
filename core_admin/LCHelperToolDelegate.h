//
//  LCHelperToolDelegate.h
//  LCAdminTools
//
//  Created by James Wilson on 12/05/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCHelperTool.h"

@interface LCHelperToolDelegate : NSObject {

}

- (void) helperTool:(LCHelperTool *)helper progressUpdate:(float)progress;
- (void) helperTool:(LCHelperTool *)helper statusUpdate:(NSString *)status;
- (void) helperToolDidFinish:(LCHelperTool *)helper;
- (void) helperToolDidFail:(LCHelperTool *)helper;

@end
