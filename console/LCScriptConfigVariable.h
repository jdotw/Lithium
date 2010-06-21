//
//  LCScriptConfigVariable.h
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCXMLObject.h"

@interface LCScriptConfigVariable : LCXMLObject 
{
	/* Properties */
	NSString *name;
	NSString *desc;
	NSString *value;
	BOOL required;
	NSImage *requiredIcon;
	
	/* Related Objects */
	id task;
	
	/* XML Ops */
	LCXMLRequest *xmlReq;	
}

#pragma mark "Properties"
@property (nonatomic,assign) id task;
@property (nonatomic,copy) NSString *name;
@property (nonatomic,copy) NSString *desc;
@property (nonatomic,copy) NSString *value;
@property (nonatomic,assign) BOOL required;
@property (nonatomic,copy) NSImage *requiredIcon;

@end
