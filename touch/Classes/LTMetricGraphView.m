//
//  UIMetricGraphView.m
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricGraphView.h"
#import "LTEntityDescriptor.h"
#import <QuartzCore/QuartzCore.h>


@implementation LTMetricGraphView

- (id)initWithFrame:(CGRect)frame 
{
    if (self = [super initWithFrame:frame]) 
	{
        // Initialization code
		self.graphRequest = [[LTMetricGraphRequest alloc] init];
		
		spinner = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
		spinner.hidesWhenStopped = YES; 
		[self addSubview:spinner];
		[spinner autorelease];
		
		notRecordedLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		notRecordedLabel.backgroundColor = [UIColor clearColor];;
		notRecordedLabel.opaque = YES;
		notRecordedLabel.textColor = [UIColor grayColor];
		notRecordedLabel.highlightedTextColor = [UIColor grayColor];
		notRecordedLabel.font = [UIFont boldSystemFontOfSize:14.0];
		notRecordedLabel.textAlignment = UITextAlignmentCenter;
		notRecordedLabel.hidden = YES;
		[self addSubview:notRecordedLabel];
		[notRecordedLabel autorelease];

		deviceLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		deviceLabel.backgroundColor = [UIColor clearColor];;
		deviceLabel.opaque = YES;
		deviceLabel.textColor = [UIColor darkGrayColor];
		deviceLabel.highlightedTextColor = [UIColor grayColor];
		deviceLabel.font = [UIFont boldSystemFontOfSize:12.0];
		deviceLabel.textAlignment = UITextAlignmentCenter;
		deviceLabel.hidden = NO;
		[self addSubview:deviceLabel];
		[deviceLabel autorelease];

		metricLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		metricLabel.backgroundColor = [UIColor clearColor];;
		metricLabel.opaque = YES;
		metricLabel.textColor = [UIColor darkGrayColor];
		metricLabel.highlightedTextColor = [UIColor grayColor];
		metricLabel.font = [UIFont boldSystemFontOfSize:12.0];
		metricLabel.textAlignment = UITextAlignmentCenter;
		metricLabel.hidden = NO;
		[self addSubview:metricLabel];
		[metricLabel autorelease];
		
		imageView = [[UIImageView alloc] initWithFrame:self.graphImageArea];
		[self addSubview:imageView];
		[imageView autorelease];
				
		reflectionView = [[UIImageView alloc] initWithFrame:CGRectZero];
		reflectionView.alpha = 0.3f;
		[self addSubview:reflectionView];
		[reflectionView autorelease];
		
		leftDateLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		leftDateLabel.backgroundColor = [UIColor clearColor];;
		leftDateLabel.opaque = YES;
		leftDateLabel.textColor = [UIColor lightGrayColor];
		leftDateLabel.highlightedTextColor = [UIColor lightGrayColor];
		leftDateLabel.font = [UIFont boldSystemFontOfSize:dateLabelFontSize];
		leftDateLabel.textAlignment = UITextAlignmentCenter;
		leftDateLabel.hidden = YES;
		[self addSubview:leftDateLabel];
		[leftDateLabel release];

		rightDateLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		rightDateLabel.backgroundColor = [UIColor clearColor];;
		rightDateLabel.opaque = YES;
		rightDateLabel.textColor = [UIColor lightGrayColor];
		rightDateLabel.highlightedTextColor = [UIColor lightGrayColor];
		rightDateLabel.font = [UIFont boldSystemFontOfSize:dateLabelFontSize];
		rightDateLabel.textAlignment = UITextAlignmentCenter;
		rightDateLabel.hidden = YES;
		[self addSubview:rightDateLabel];		
		[rightDateLabel autorelease];

		minLabel = [[UILabel alloc] initWithFrame:CGRectMake(CGRectGetMinX(self.graphArea)+2, CGRectGetMaxY(self.graphArea) - (yLabelFontSize + 2), 100.0, yLabelFontSize)];
		minLabel.backgroundColor = [UIColor clearColor];
		minLabel.opaque = YES;
		minLabel.textColor = [UIColor colorWithWhite:160.0f/256.0f alpha:1.0];
		minLabel.highlightedTextColor = [UIColor colorWithWhite:160.0f/256.0f alpha:1.0];
		minLabel.font = [UIFont systemFontOfSize:yLabelFontSize];
		minLabel.textAlignment = UITextAlignmentLeft;
		[self addSubview:minLabel];		
		[minLabel autorelease];		

		avgLabel = [[UILabel alloc] initWithFrame:CGRectMake(CGRectGetMinX(self.graphArea)+2, CGRectGetMidY(self.graphArea) - (yLabelFontSize * 0.5f), 100.0, yLabelFontSize)];
		avgLabel.backgroundColor = [UIColor clearColor];
		avgLabel.opaque = YES;
		avgLabel.textColor = [UIColor colorWithWhite:160.0f/256.0f alpha:1.0];
		avgLabel.highlightedTextColor = [UIColor colorWithWhite:160.0f/256.0f alpha:1.0];
		avgLabel.font = [UIFont systemFontOfSize:yLabelFontSize];
		avgLabel.textAlignment = UITextAlignmentLeft;
		[self addSubview:avgLabel];		
		[avgLabel autorelease];				

		maxLabel = [[UILabel alloc] initWithFrame:CGRectMake(CGRectGetMinX(self.graphArea)+2, CGRectGetMinY(self.graphArea)+2, 100.0, yLabelFontSize)];
		maxLabel.backgroundColor = [UIColor clearColor];
		maxLabel.opaque = YES;
		maxLabel.textColor = [UIColor colorWithWhite:160.0f/256.0f alpha:1.0];
		maxLabel.highlightedTextColor = [UIColor colorWithWhite:160.0f/256.0f alpha:1.0];
		maxLabel.font = [UIFont systemFontOfSize:yLabelFontSize];
		maxLabel.textAlignment = UITextAlignmentLeft;
		[self addSubview:maxLabel];		
		[maxLabel autorelease];				
    }
    return self;
}

- (void)dealloc 
{
	[graphRequest release];
    [super dealloc];
}

- (void) awakeFromNib
{
	[super awakeFromNib];
	[self initWithFrame:self.frame];	
}

- (CGRect) graphArea
{
	return CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMinY(self.bounds)+4.0f, CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds)-22.0);
}

- (CGRect) graphImageArea
{
	return CGRectMake(CGRectGetMinX(self.graphArea), CGRectGetMinY(self.graphArea) + (yLabelFontSize * 0.5f), 
					  CGRectGetWidth(self.graphArea), CGRectGetHeight(self.graphArea)-yLabelFontSize);
	
}

- (void)layoutSubviews
{	
	spinner.frame = CGRectMake(CGRectGetMidX(self.bounds) - (spinner.bounds.size.width * 0.5f),
							   CGRectGetMidY(self.bounds) - (spinner.bounds.size.height * 0.5f),
							   spinner.bounds.size.width, spinner.bounds.size.height);
	if (!metric.recordEnabled)
	{
		/* Set not recorded flag */
		CGSize stringSize = [notRecordedLabel.text sizeWithFont:notRecordedLabel.font];
		notRecordedLabel.frame = CGRectMake(roundf(CGRectGetMidX(self.bounds) - (stringSize.width * 0.5f)),
											roundf(CGRectGetMidY(self.bounds) - (stringSize.height * 0.5f) + (CGRectGetHeight(self.bounds) * 0.25)),
											stringSize.width, stringSize.height);
	}

	deviceLabel.frame = CGRectMake(roundf(CGRectGetMinX(self.graphArea)),
										roundf(CGRectGetMidY(self.graphArea) - 12.0 - 3.0),
										CGRectGetWidth(self.graphArea), 14.0f);

	metricLabel.frame = CGRectMake(roundf(CGRectGetMinX(self.graphArea)),
										roundf(CGRectGetMidY(self.graphArea) + 3.0),
										CGRectGetWidth(self.graphArea), 14.0f);
	
	
	/* Arrange right date label */
	CGFloat padding = 10.0f;
	NSDateFormatter *dateFormatter = [[[NSDateFormatter alloc] init]  autorelease];
	[dateFormatter setDateStyle:NSDateFormatterShortStyle];
	rightDateLabel.text = [dateFormatter stringFromDate:graphRequest.referenceDate];
	CGSize stringSize = [rightDateLabel.text sizeWithFont:rightDateLabel.font];
	rightDateLabel.frame = CGRectMake(roundf(CGRectGetMaxX(self.bounds) - stringSize.width - padding),
									  roundf((CGRectGetMidY(self.bounds) + (CGRectGetHeight(self.bounds) * 0.25f)) - (stringSize.height * 0.5f) - 10.f),
									  stringSize.width, stringSize.height);

	/* Arrange left date label */
	NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
	NSDateComponents *deltaComp = [[NSDateComponents alloc] init];
	[deltaComp setDay:-1];
	leftDateLabel.text = [dateFormatter stringFromDate:[calendar dateByAddingComponents:deltaComp toDate:graphRequest.referenceDate options:0]];
	stringSize = [leftDateLabel.text sizeWithFont:leftDateLabel.font];
	leftDateLabel.frame = CGRectMake(roundf(CGRectGetMinX(self.bounds) + padding),
									  roundf((CGRectGetMidY(self.bounds) + (CGRectGetHeight(self.bounds) * 0.25f)) - (stringSize.height * 0.5f) - 10.f),
									  stringSize.width, stringSize.height);	

	/* Image View Layout */
	CGRect graphDataRect;
	if (useArtificialScale)
	{
		/* Unscaled */
		float height = CGRectGetMaxY(self.graphImageArea) - CGRectGetMinY(self.graphImageArea);
		float scaledRange = (scaledMax - scaledMin);
		float scaledPixelPerUnit = height / scaledRange;
		
		/* Calculate scaled max */
		float maxY = CGRectGetMinY(self.graphImageArea);
		float maxDelta = (scaledMax - graphRequest.maxValue);
		float scaledMaxY = maxY + (maxDelta * scaledPixelPerUnit);
		
		/* Calculate scaled min */
		float minY = CGRectGetMaxY(self.graphImageArea);
		float minDelta = (scaledMin - graphRequest.minValue);
		float scaledMinY = minY + (minDelta * scaledPixelPerUnit);
		
		graphDataRect = CGRectMake(CGRectGetMinX(self.graphImageArea), scaledMaxY, CGRectGetWidth(self.graphImageArea), scaledMinY - scaledMaxY);
	}
	else
	{
		graphDataRect = self.graphImageArea;
	}
	if (!CGRectEqualToRect(graphDataRect, imageView.frame))
	{ 
		if (animateScaling)
		{
			[UIView beginAnimations:nil context:nil];
			[UIView setAnimationDuration:1.0];
			[UIView setAnimationTransition:UIViewAnimationTransitionNone forView:imageView cache:YES];
			[UIView setAnimationTransition:UIViewAnimationTransitionNone forView:maxLabel cache:YES];
			[UIView setAnimationTransition:UIViewAnimationTransitionNone forView:avgLabel cache:YES];
			[UIView setAnimationTransition:UIViewAnimationTransitionNone forView:minLabel cache:YES];
			[imageView setFrame:graphDataRect];
			[maxLabel setFrame:CGRectMake(CGRectGetMinX(graphDataRect)+2, CGRectGetMinY(graphDataRect) - (yLabelFontSize * 0.5f), 100.0, yLabelFontSize)];
			[avgLabel setFrame:CGRectMake(CGRectGetMinX(graphDataRect)+2, roundf(CGRectGetMidY(graphDataRect) - (yLabelFontSize * 0.5f)), 100.0, yLabelFontSize)];
			[minLabel setFrame:CGRectMake(CGRectGetMinX(graphDataRect)+2, CGRectGetMaxY(graphDataRect) - (yLabelFontSize * 0.5f), 100.0, yLabelFontSize)];
			[UIView commitAnimations];
		}
		else
		{
			[imageView setFrame:graphDataRect];
			[maxLabel setFrame:CGRectMake(CGRectGetMinX(graphDataRect)+2, CGRectGetMinY(graphDataRect) - (yLabelFontSize * 0.5f), 100.0, yLabelFontSize)];
			[avgLabel setFrame:CGRectMake(CGRectGetMinX(graphDataRect)+2, roundf(CGRectGetMidY(graphDataRect) - (yLabelFontSize * 0.5f)), 100.0, yLabelFontSize)];
			[minLabel setFrame:CGRectMake(CGRectGetMinX(graphDataRect)+2, CGRectGetMaxY(graphDataRect) - (yLabelFontSize * 0.5f), 100.0, yLabelFontSize)];

		}
	}
	
	[reflectionView setFrame:CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMaxY(self.graphImageArea), CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds) - CGRectGetHeight(self.graphImageArea))];
	reflectionView.image = [self reflectedImage:imageView withHeight:(NSUInteger)reflectionView.bounds.size.height];

}

- (void)drawRect:(CGRect)rect 
{
	[super drawRect:rect];
	int i;

	/* Draw background */
	[[UIColor blackColor] setFill];
	UIRectFill(self.bounds);
	UIImage *backImage = nil;
	if (graphViewStyle == 1)
	{ 
		backImage = [UIImage imageNamed:@"mediumgraphback.png"]; ;
	}
	else if (graphViewStyle == 2)
	{ 
		backImage = [UIImage imageNamed:@"mediumgraphback.png"]; ;
	}
	else if (graphViewStyle == 3)
	{
		backImage = [UIImage imageNamed:@"largegraphback.png"]; 
	}
	[backImage drawInRect:self.bounds
				blendMode:kCGBlendModeSourceAtop 
					alpha:1.0f];
	
	if (self.graphViewStyle == 1)
	{
		// Ridge lines
		CGRect topLineRect;
		
		// Bottom (Dark)
		[[UIColor colorWithRed:20.0/255.0 green:20.0/255.0 blue:20.0/255.0 alpha:1.0] setFill];
		topLineRect = CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMaxY(self.bounds)-1.0, CGRectGetWidth(self.bounds), 1.0);
		UIRectFill(topLineRect);
		
		// Top (Light)
		[[UIColor colorWithRed:120.0/255.0 green:120.0/255.0 blue:120.0/255.0 alpha:1.0] setFill];
		topLineRect = CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMinY(self.bounds)+0.0, CGRectGetWidth(self.bounds), 1.0);
		UIRectFill(topLineRect);
	}
	
	/* Check we're recorded */
	if (!metric.recordEnabled) 
	{
		return;
	}

	/* Calculate Image Rect */
	CGRect imageRect = CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMinY(self.bounds), CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds)-26.0f);
	
	/* Draw Date Line */
	CGRect dateLineRect = CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMaxY(self.bounds)-26.0f, CGRectGetWidth(self.bounds), 26.0f);
	float pixelPerMinute = CGRectGetWidth(imageRect) / (48.0f * 60.0f);
	NSDate *now = [NSDate date];
	NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
	NSDateComponents *nowMinuteComp = [calendar components:NSMinuteCalendarUnit fromDate:now];
	float minuteAhead = (float) [nowMinuteComp minute];
	[nowMinuteComp setMinute:([nowMinuteComp minute] * -1)];
	NSDate *nearestHour = [calendar dateByAddingComponents:nowMinuteComp toDate:now options:0];
	CGSize stringSize = CGSizeMake(100.0, 11.0f);
	CGRect nearestHourRect = CGRectMake(CGRectGetMaxX(self.bounds) - (minuteAhead * pixelPerMinute) - (stringSize.width * 0.5f),
										CGRectGetMaxY(dateLineRect) - yLabelFontSize - 4.0, stringSize.width, stringSize.height);
	int hourIncrement = 8;
	int verticalSections = (48 / hourIncrement);
	for (i=0; i < verticalSections; i++)
	{
		NSDateComponents *delta = [[[NSDateComponents alloc] init] autorelease];
		[delta setHour:(((hourIncrement) * -1) * i)];
		NSDate *hourDate = [calendar dateByAddingComponents:delta toDate:nearestHour options:0];
		NSDateComponents *hourDateComp = [calendar components:NSHourCalendarUnit|NSMinuteCalendarUnit fromDate:hourDate];
		NSString *hourString = [NSString stringWithFormat:@"%.2d:00 ", [hourDateComp hour]];
		CGSize hourStringSize = CGSizeMake(100.0, 11.0f);

		CGRect hourRect = CGRectMake(CGRectGetMinX(nearestHourRect) - (((float) i) * (pixelPerMinute * hourIncrement * 60.0f)),
									 CGRectGetMinY(nearestHourRect), hourStringSize.width, hourStringSize.height);
		
		/* Check to make sure this hour wont draw too close to the Y axis */
		if (CGRectGetMinX(hourRect) > 10.0)
		{ 
			[[UIColor colorWithWhite:0.9 alpha:1.0] setStroke];
			[[UIColor colorWithWhite:0.9 alpha:1.0] setFill];
			
			[hourString drawInRect:hourRect
						  withFont:[UIFont systemFontOfSize:dateLabelFontSize]
					 lineBreakMode:UILineBreakModeClip
						 alignment:UITextAlignmentLeft];
		}
	}
}

- (NSString *) scaledValueString:(float)units
{
	float k = 1000;
	float m = 1000000;
	float g = 1000000000;
	
	if (units < k)
	{ return [NSString stringWithFormat:@"%.2f", units]; }
	else if (units < m)
	{ return [NSString stringWithFormat:@"%.2fK", units / k]; }
	else if (units < g)
	{ return [NSString stringWithFormat:@"%.2fM", units / m]; }
	else if ((units / 1024) < g)
	{ return [NSString stringWithFormat:@"%.2fG", units / g]; }
	else
	{ return [NSString stringWithFormat:@"%.2fT", (units / g) * (1 / 1000)]; }
}

#pragma mark - Image Reflection


- (UIImage *)reflectedImage:(UIImageView *)fromImage withHeight:(NSUInteger)height
{
	/* Check Height and image */
	if (height == 0 || !fromImage) return nil;
	
	// create the bitmap context
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef mainViewContentContext = CGBitmapContextCreate (nil, (size_t) fromImage.bounds.size.width, (size_t) height, 8, 0, colorSpace,
													   (kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst));
	CGColorSpaceRelease(colorSpace);
		   
	// offset the context -
	// This is necessary because, by default, the layer created by a view for caching its content is flipped.
	// But when you actually access the layer content and have it rendered it is inverted.  Since we're only creating
	// a context the size of our reflection view (a fraction of the size of the main view) we have to translate the
	// context the delta in size, and render it.
	//
	CGFloat translateVertical= fromImage.bounds.size.height - height;
	CGContextTranslateCTM(mainViewContentContext, 0, -translateVertical);
	
	// render the layer into the bitmap context
	CALayer *layer = fromImage.layer;
	[layer renderInContext:mainViewContentContext];
	
	// create CGImageRef of the main view bitmap content, and then release that bitmap context
	CGImageRef mainViewContentBitmapContext = CGBitmapContextCreateImage(mainViewContentContext);
	CGContextRelease(mainViewContentContext);
	

	/*
	 * Create gradient 
	 */
	
	// gradient is always black-white and the mask must be in the gray colorspace
    colorSpace = CGColorSpaceCreateDeviceGray();
	
	// create the bitmap context
	CGContextRef gradientBitmapContext = CGBitmapContextCreate(nil, 1, height,
															   8, 0, colorSpace, kCGImageAlphaNone);
	
	// define the start and end grayscale values (with the alpha, even though
	// our bitmap context doesn't support alpha the gradient requires it)
	CGFloat colors[] = {0.0, 1.0, 1.0, 1.0};
	
	// create the CGGradient and then release the gray color space
	CGGradientRef grayScaleGradient = CGGradientCreateWithColorComponents(colorSpace, colors, NULL, 2);
	CGColorSpaceRelease(colorSpace);
	
	// create the start and end points for the gradient vector (straight down)
	CGPoint gradientStartPoint = CGPointZero;
	CGPoint gradientEndPoint = CGPointMake(0, height);
	
	// draw the gradient into the gray bitmap context
	CGContextDrawLinearGradient(gradientBitmapContext, grayScaleGradient, gradientStartPoint,
								gradientEndPoint, kCGGradientDrawsAfterEndLocation);
	CGGradientRelease(grayScaleGradient);
	
	// convert the context into a CGImageRef and release the context
	CGImageRef gradientMaskImage = CGBitmapContextCreateImage(gradientBitmapContext);
	CGContextRelease(gradientBitmapContext);
	
	/* 
	 * Mask
	 */
	
	// create an image by masking the bitmap of the mainView content with the gradient view
	// then release the  pre-masked content bitmap and the gradient bitmap
	CGImageRef reflectionImage = CGImageCreateWithMask(mainViewContentBitmapContext, gradientMaskImage);
	CGImageRelease(mainViewContentBitmapContext);
	CGImageRelease(gradientMaskImage);
	
	// convert the finished reflection image to a UIImage 
	if (reflectionImage) 
	{
		/* A COPY of the reflectionImage must be used
		 * or else a malloc error claiming that the 
		 * image was freed but not alloced occurs when the 
		 * resulting UIImage is freed
		 */
		UIImage *theImage = [UIImage imageWithCGImage:CGImageCreateCopy(reflectionImage)];
		CGImageRelease(reflectionImage);
		return theImage;
	}
	else return nil;	
}

- (void) graphRefreshFinished:(NSNotification *)notification
{
	[spinner stopAnimating]; 
	imageView.image = [UIImage imageWithData:graphRequest.imageData];
	
	minLabel.text = [self scaledValueString:graphRequest.minValue];
	avgLabel.text = [self scaledValueString:(graphRequest.minValue + ((graphRequest.maxValue - graphRequest.minValue) * 0.5f))];
	maxLabel.text = [self scaledValueString:graphRequest.maxValue];	
	[self setNeedsLayout];
	
	reflectionView.image = [self reflectedImage:imageView withHeight:(NSUInteger)reflectionView.bounds.size.height];
}

- (void) metricRefreshFinished:(NSNotification *)notification
{
	if (metric.recordEnabled)
	{
		if (graphRequestNeedsRefresh) [graphRequest refresh];
		if (graphRequest.refreshInProgress)
		{ [spinner startAnimating]; }
	}
	else
	{
		notRecordedLabel.text = @"Metric is Not Recorded";
		notRecordedLabel.hidden = NO;
		[spinner stopAnimating];
	}
	[self setNeedsLayout];
}

@synthesize graphRequest;
- (void) setGraphRequest:(LTMetricGraphRequest *)value
{
	[graphRequest release];
	graphRequest = [value retain];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(graphRefreshFinished:)
												 name:@"GraphRefreshFinished" 
											   object:graphRequest];		

	if (!graphRequest.refreshInProgress)
	{ [self graphRefreshFinished:nil]; }
}

@synthesize metric;
- (void) setMetric:(LTEntity *)value
{
	[metric release];
	metric = [value retain];

	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(metricRefreshFinished:)
												 name:@"RefreshFinished"
											   object:metric];	
	
	LTEntityDescriptor *entityDescriptor = [metric entityDescriptor];
	deviceLabel.text = [NSString stringWithFormat:@"%@ @ %@", entityDescriptor.devDesc, entityDescriptor.siteDesc];
	metricLabel.text = [NSString stringWithFormat:@"%@ %@ %@", entityDescriptor.cntDesc, entityDescriptor.objDesc, entityDescriptor.desc];
	
	if (graphRequest.metric != metric)
	{ 
		graphRequest.metric = metric; 
		graphRequestNeedsRefresh = YES;
	}
		
	if (metric.hasBeenRefreshed)
	{
		if (!metric.refreshInProgress)
		{ [self metricRefreshFinished:nil]; }
	}
	else
	{ 
		[metric refresh];
		if (metric.refreshInProgress)
		{ [spinner startAnimating]; }
	}
}

- (void) setFrame:(CGRect)value
{
	CGRect oldFrame = self.frame;
	[super setFrame:value];
	
	/* Check to see if we're a big graph */
	if (self.frame.size.width > 400)
	{
		dateLabelFontSize = 12.0;
		yLabelFontSize = 11.0;
	}
	else
	{
		dateLabelFontSize = 10.0;
		yLabelFontSize = 9.0;
	}
	
	/* Check to see if this is *really* a change in size */
	if (metric && metric.recordEnabled && (CGRectGetWidth(oldFrame) != CGRectGetWidth(value) || CGRectGetHeight(oldFrame) != CGRectGetHeight(value)))
	{
		self.graphRequest.size = self.frame.size;
		if (!graphRequest.imageData) 
		{ 
			/* Only refresh the graph if there's no existing data */
			[graphRequest refresh];
			[spinner startAnimating];
		}
	}

	UIImage *image = [self reflectedImage:imageView withHeight:(NSUInteger)reflectionView.bounds.size.height];
	if (image) reflectionView.image = image;
}

@synthesize dateLabelsHidden;
- (void) setDateLabelsHidden:(BOOL)flag
{
	dateLabelsHidden = flag;
	if (dateLabelsHidden)
	{
		rightDateLabel.hidden = YES;
		leftDateLabel.hidden = YES;
		CATransition *animation = [CATransition animation];
		[animation setDelegate:self];
		[animation setType:kCATransitionFade];
		[animation setDuration:2.0];
		[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
		[[rightDateLabel layer] addAnimation:animation forKey:@"transitionViewAnimation"];
		[[leftDateLabel layer] addAnimation:animation forKey:@"transitionViewAnimation"];
	}
	else
	{
		rightDateLabel.hidden = NO;
		leftDateLabel.hidden = NO;		
		CATransition *animation = [CATransition animation];
		[animation setDelegate:self];
		[animation setType:kCATransitionFade];
		[animation setDuration:0.3];
		[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
		[[rightDateLabel layer] addAnimation:animation forKey:@"transitionViewAnimation"];
		[[leftDateLabel layer] addAnimation:animation forKey:@"transitionViewAnimation"];
		
	}	
}

@synthesize scaledMax;
- (void) setScaledMax:(float)value
{
	scaledMax = value;
	scaledMaxLabel.text = [self scaledValueString:scaledMax];
	scaledAvgLabel.text = [self scaledValueString:(scaledMin + ((scaledMax - scaledMin) * 0.5f))];
}

@synthesize scaledMin;
- (void) setScaledMin:(float)value
{
	scaledMin = value;
	scaledMinLabel.text = [self scaledValueString:scaledMin];
	scaledAvgLabel.text = [self scaledValueString:(scaledMin + ((scaledMax - scaledMin) * 0.5f))];
}

@synthesize useArtificialScale;
- (void) setUseArtificialScale:(BOOL)flag
{
	if (flag == useArtificialScale) return;
	
	useArtificialScale = flag;
	
	[self setNeedsLayout];
}
@synthesize animateScaling;
@synthesize graphViewStyle;
- (void) setGraphViewStyle:(int)value
{
	graphViewStyle = value;
	
	if (graphViewStyle == 1 || graphViewStyle == 2)
	{
		if (graphViewStyle == 2)
		{
			deviceLabel.hidden = YES;
			metricLabel.hidden = YES;
		}
		else
		{
			deviceLabel.hidden = NO;
			metricLabel.hidden = NO;
		}
	}
	else if (graphViewStyle == 3)
	{
		deviceLabel.hidden = NO;
		metricLabel.hidden = NO;
	}

	UIColor *textColor = [UIColor colorWithWhite:0.0 alpha:0.6];
	UIColor *shadowColor = [UIColor colorWithWhite:1.0 alpha:0.08];
	CGSize shadowOffset = CGSizeMake(1.0, 1.0);
	minLabel.textColor = [UIColor colorWithWhite:0.9 alpha:1.0];
	avgLabel.textColor = [UIColor colorWithWhite:0.9 alpha:1.0];
	maxLabel.textColor = [UIColor colorWithWhite:0.9 alpha:1.0];
	deviceLabel.textColor = textColor;
	deviceLabel.highlightedTextColor = textColor;
	metricLabel.textColor = textColor;
	metricLabel.highlightedTextColor = textColor;
	metricLabel.shadowColor = shadowColor;
	metricLabel.shadowOffset = shadowOffset;
	deviceLabel.shadowColor = shadowColor;
	deviceLabel.shadowOffset = shadowOffset;	
	
	[self setNeedsDisplay];
}

@end
