//
//  CALayer-PDF.h
//  Lithium Console
//
//  Created by James Wilson on 4/02/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

@interface CALayer (PDF) 

-(NSData *)dataForPDFRepresentationOfLayer;

@end
