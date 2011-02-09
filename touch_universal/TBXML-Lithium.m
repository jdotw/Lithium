//
//  TBXML-Lithium.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 9/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "TBXML-Lithium.h"


@implementation TBXML (TBXML_Lithium)

+ (NSString *) textForElementNamed:(NSString *)name parentElement:(TBXMLElement *)parentElement
{
    return [TBXML textForElement:[TBXML childElementNamed:name parentElement:parentElement]];
}

@end
