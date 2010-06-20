//
//  LCXMLParseOperation.h
//  Lithium Console
//
//  Created by James Wilson on 4/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCXMLNode.h"

@interface LCXMLParseOperation : NSOperation 
{
	NSData *xmlData;
	id delegate;
	
	LCXMLNode *rootNode;
	
	NSMutableArray *nodeStack;
}

@property (copy) NSData *xmlData;
@property (retain) id delegate;
@property (retain) LCXMLNode *rootNode;

@end
