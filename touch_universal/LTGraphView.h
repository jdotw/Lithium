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

	NSDate *graphStartDate;						// The furtest-right point of the graph
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

- (void) refreshGraph;

@property (nonatomic,retain) NSArray *metrics;
@property (nonatomic,readonly) NSMutableArray *minLabels;
@property (nonatomic,readonly) NSMutableArray *avgLabels;
@property (nonatomic,readonly) NSMutableArray *maxLabels;

@end
