//
//  LCGraphFlowLayout.m
//  Lithium Console
//
//  Created by James Wilson on 4/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGraphFlowLayout.h"

CGFloat DegreesToRadians(CGFloat degrees);
CGFloat RadiansToDegrees(CGFloat radians);

#import "LCGraphFlowController.h"
#import "LCGraphFlowCard.h"

@implementation LCGraphFlowLayout

- (void)layoutSublayersOfLayer:(CALayer *)rootLayer
{
	/* This function is called to layout the root flow layer.
	 * Loop through each individual sub-layer (cards) to determine
	 * their position, rotation, etc
	 */

	/* Geometry */
	CGFloat sideMargin = roundf([rootLayer bounds].size.width * 0.15);			// Left/Right Margin 
	CGFloat bottomMargin = [rootLayer bounds].size.height * 0.3;		// Space between bottom of the card and the layer bounds
	CGFloat topMargin = 8.0;					// Space between top of the card and the layer bounds
	CGFloat flatSpacing = 4.0;						// Spacing between the flat cards
	CGFloat stackSpacing = roundf(sideMargin / 10.0);		// Spacing between the stacked cards
	CGFloat stackScaleFactor = 1.0;
	CGFloat stackOverlapFactor = 0.0;	// How far the stack should creep in behind the flat 
	int flatCount = 4;					// How many flat cards to show
	
	/* Calculat rects, etc */
	CGRect flatRect = CGRectMake(CGRectGetMinX([rootLayer bounds]) + sideMargin, 
								 CGRectGetMinY([rootLayer bounds]) + bottomMargin,
								 CGRectGetWidth([rootLayer bounds]) - (2 * sideMargin), 
								 CGRectGetHeight([rootLayer bounds]) - (bottomMargin + topMargin));
	CGRect leftStackRect = CGRectMake(CGRectGetMinX([rootLayer bounds]), CGRectGetMinY([rootLayer bounds]) + bottomMargin, 
									  sideMargin, CGRectGetHeight([rootLayer bounds]) - (bottomMargin + topMargin));
	CGRect rightStackRect = CGRectMake(CGRectGetMaxX([rootLayer bounds]) - sideMargin, CGRectGetMinY([rootLayer bounds]) + bottomMargin, 
									   sideMargin, CGRectGetHeight([rootLayer bounds]) - (bottomMargin + topMargin));
	CGSize cardSize = CGSizeMake(lroundf((CGRectGetWidth(flatRect) - (((float) flatCount - 1) * flatSpacing)) / (float) flatCount), 
								 CGRectGetHeight(flatRect));
	
	/* Set transforms */
	float zDistance = cardSize.width * 2.0;
	CATransform3D sublayerTransform = CATransform3DIdentity; 
	sublayerTransform.m34 = 1. / -zDistance;
	float angle = 1.15;
	CATransform3D leftTransform = CATransform3DMakeRotation(-angle, 0, -1, 0);
	CATransform3D rightTransform = CATransform3DMakeRotation(angle, 0, -1, 0);

	/* Loop through cards */
	int focusIndex = [controller.cards indexOfObject:controller.focusMetric];
	LCGraphFlowCard *previousCard = nil;
	for (LCGraphFlowCard *card in controller.cards)
//	for (CALayer *cardLayer in [rootLayer sublayers])
	{
		/* Get card */
//		LCMetric *metric = [cardLayer valueForKey:@"metric"];
//		LCGraphFlowCard *card = [controller.cardDictionary objectForKey:metric.uniqueIdentifier];
		int cardIndex = [controller.cards indexOfObject:card];
		
		/* Card Perspective */
		card.cardLayer.sublayerTransform = sublayerTransform;

		/* Position-dependent geometry */
		CGRect cardRect = CGRectZero;
		if (cardIndex >= focusIndex && cardIndex < (focusIndex + flatCount))
		{
			/* 
			 * Card is flat 
			 */
			
			int offset = cardIndex - focusIndex;
			cardRect = CGRectMake(CGRectGetMinX(flatRect) + (offset * (cardSize.width + flatSpacing)),
								  CGRectGetMinY(flatRect), cardSize.width, cardSize.height);
			card.imageLayer.transform = CATransform3DIdentity;
			card.cardLayer.zPosition = 0.0;
			if (card.orientation != FLAT)
			{
				card.orientation = FLAT;
				[card.imageLayer setNeedsDisplay];				
			}
		}
		else if (cardIndex < focusIndex)
		{
			/* 
			 * Card is stacked to the left 
			 */
			
			int firstStackedIndex = focusIndex - 1;
			int offset = firstStackedIndex - cardIndex;
			cardRect = CGRectMake(roundf(CGRectGetMaxX(leftStackRect) - (stackSpacing * (offset)) - (cardSize.width * (0.6 - stackOverlapFactor))),
								  CGRectGetMinY(leftStackRect), cardSize.width, cardSize.height);
			cardRect.size.width = cardRect.size.width * stackScaleFactor;
			card.imageLayer.transform = leftTransform;
			if (card.orientation != STACKED_LEFT)
			{
				card.orientation = STACKED_LEFT;
				[card.imageLayer setNeedsDisplay];
			}			
		}
		else if (cardIndex >= (focusIndex + flatCount))
		{
			/* 
			 * Card is stacked to the right 
			 */
			
			int firstStackedIndex = focusIndex + flatCount;
			int offset = cardIndex - firstStackedIndex;
			cardRect = CGRectMake(roundf((CGRectGetMinX(rightStackRect) + (stackSpacing * (offset))) - (cardSize.width * 0.4)),
								  CGRectGetMinY(rightStackRect), cardSize.width, cardSize.height);
			cardRect.size.width = cardRect.size.width * stackScaleFactor;
			card.imageLayer.transform = rightTransform;
			card.cardLayer.zPosition = -1 * cardIndex;
			if (card.orientation != STACKED_RIGHT)
			{
				card.orientation = STACKED_RIGHT;
				[card.imageLayer setNeedsDisplay];
			}			
		}
		
		/*
		 * Check to see if the card is visible 
		 */
		
		if (CGRectIntersectsRect(cardRect, [rootLayer bounds]))
		{
			if (card.orientation == FLAT)
			{
				[card.graphController refreshGraph:XMLREQ_PRIO_HIGH];
			}
		}
		
		/* 
		 * Apply Geometry 
		 */

		/* Set card layer rect */
		card.cardLayer.frame = cardRect;
		
		/* Set frame for Image Layer */
		cardRect.origin = CGPointZero;
		card.imageLayer.frame = cardRect;

		/* Set frame for object label layer */
		CGRect labelRect = CGRectMake(2.0, card.imageLayer.bounds.size.height * 0.5 + (card.objLabelLayer.fontSize * 0.20), 
									  CGRectGetWidth(card.imageLayer.bounds) - 4.0, 10.0);
		card.objLabelLayer.frame = labelRect;
		
		/* Set frame for metric label layer */
		labelRect = CGRectMake(2.0, card.imageLayer.bounds.size.height * 0.5 - (card.metLabelLayer.fontSize * 1.2), 
							   CGRectGetWidth(card.imageLayer.bounds) - 4.0, 10.0);
		card.metLabelLayer.frame = labelRect;
		
		/* Set frame for reflection layer */
		cardRect.origin = CGPointMake(0.0, -1.0 * cardRect.size.height);
		card.reflectionLayer.frame = cardRect;
		
		/* Set frame for gradient layer */
		cardRect.origin.y += cardRect.size.height;
		cardRect.origin.x -= 0.5;
		cardRect.size.height += 1.0;
		cardRect.size.width += 1.0;
		card.gradientLayer.frame = cardRect;
		
		/* Reset bounds on inverted layers */
		cardRect.origin = CGPointMake(0.0, 0.0);
		card.reflectionLayer.bounds = cardRect;
		card.gradientLayer.bounds = cardRect;
			 
		/* Set previous */
		previousCard = card;		
	}
	
}

@synthesize controller;

@end
