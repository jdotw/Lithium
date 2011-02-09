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
    TBXMLElement *element = [TBXML childElementNamed:name parentElement:parentElement];
    return element ? [TBXML textForElement:element] : nil;
}

+ (NSInteger) intFromTextForElementNamed:(NSString *)name parentElement:(TBXMLElement *)parentElement
{
    return [[TBXML textForElementNamed:name parentElement:parentElement] intValue];
}

@end
