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
@property (assign) id task;
@property (copy) NSString *name;
@property (copy) NSString *desc;
@property (copy) NSString *value;
@property (assign) BOOL required;
@property (copy) NSImage *requiredIcon;

@end
