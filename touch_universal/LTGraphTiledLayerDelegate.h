//
//  LTGraphTiledLayerDelegate.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

@class LTDeviceViewController;

@interface LTGraphTiledLayerDelegate : NSObject 
{
	NSArray *metrics;

	UIScrollView *graphScrollView;
	CATiledLayer *graphLayer;
	
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
}

@property (retain) UIScrollView *graphScrollView;
@property (retain) NSArray *metrics;
@property (retain) CATiledLayer *graphLayer;
@property (readonly) NSMutableArray *minLabels;
@property (readonly) NSMutableArray *avgLabels;
@property (readonly) NSMutableArray *maxLabels;


@end
