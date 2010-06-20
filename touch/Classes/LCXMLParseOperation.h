//
//  LCXMLParseOperation.h
//  Lithium Console
//
//  Created by James Wilson on 4/06/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LCXMLNode.h"

@interface LCXMLParseOperation : NSOperation <NSXMLParserDelegate>
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
