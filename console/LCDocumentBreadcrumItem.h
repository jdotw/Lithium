//
//  LCDocumentBreadcrumItem.h
//  Lithium Console
//
//  Created by James Wilson on 18/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDocument.h"

@interface LCDocumentBreadcrumItem : NSObject 
{
	NSString *title;
	LCDocument *document;
}

@property (copy) NSString *title;
@property (retain) LCDocument *document;

@end
