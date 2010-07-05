//
//  LTGraphView.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 11/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface LTGraphView : UIView
{
	NSArray *metrics;
	
	CGPDFDocumentRef myDocumentRef;
    CGPDFPageRef myPageRef;
	
	BOOL minMaxSet;
	BOOL invalidated;
	float maxValue;
	float minValue;
	NSMutableDictionary *graphRequestCache;
	
	NSMutableArray *minLabels;
	NSMutableArray *avgLabels;
	NSMutableArray *maxLabels;	
	
	CATiledLayer *graphTiledLayer;
}

@property (retain) NSArray *metrics;
@property (readonly) NSMutableArray *minLabels;
@property (readonly) NSMutableArray *avgLabels;
@property (readonly) NSMutableArray *maxLabels;

@end
