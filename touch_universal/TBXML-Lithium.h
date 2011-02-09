//
//  TBXML-Lithium.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 9/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TBXML.h"

@interface TBXML (TBXML_Lithium)

+ (NSString *) textForElementNamed:(NSString *)name parentElement:(TBXMLElement *)parentElement;
+ (NSInteger) intFromTextForElementNamed:(NSString *)name parentElement:(TBXMLElement *)parentElement;

@end
