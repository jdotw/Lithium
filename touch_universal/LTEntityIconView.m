//
//  LTEntityIconView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 4/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTEntityIconView.h"
#import "LTEntity.h"

@implementation LTEntityIconView

@synthesize selected, entity=_entity;

- (id)initWithFrame:(CGRect)frame {
    
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code.
    }
    return self;
}

- (UIImage *) _selectionOverlayImage
{
    if (self.entity.type == ENT_CONTAINER) return [UIImage imageNamed:@"ContScrollerSelectionOverlay.png"];
    else if (self.entity.type == ENT_OBJECT) return [UIImage imageNamed:@"ObjScrollerSelectionOverlay.png"];
    return nil;
}

- (UIImage *) _statusIndicatorImage
{
    NSString *prefix = nil;
    switch (self.entity.type) {
        case ENT_CONTAINER:
            prefix = @"Cont";
            break;
        case ENT_OBJECT:
            prefix = @"Obj";
            break;
        default:
            break;
    }
    NSString *color = nil;
    switch (self.entity.opState)
    {
        case 3:
            color = @"Red";
            break;
        case 2:
            color = @"Orange";
            break;
        case 1:
            color = @"Yellow";
            break;
        case 0:
            color = @"Green";
            break;
        default:
            color = @"Gray";
            break;
    }
    return [UIImage imageNamed:[NSString stringWithFormat:@"%@ScrollerIndicator-%@.png", prefix, color]];
}

- (void)drawRect:(CGRect)rect 
{	
	/* Draw right-side edge */
	CGRect vertBorderRect = CGRectMake(CGRectGetMaxX(self.bounds)-2.0, CGRectGetMinY(self.bounds), 1.0, CGRectGetHeight(self.bounds));
	[[UIColor colorWithWhite:0.0 alpha:1.0] setFill];
	[[UIBezierPath bezierPathWithRect:vertBorderRect] fillWithBlendMode:kCGBlendModeDarken alpha:0.3];
	[[UIColor colorWithWhite:1.0 alpha:1.0] setFill];
	[[UIBezierPath bezierPathWithRect:CGRectOffset(vertBorderRect, 1.0, 0.0)] fillWithBlendMode:kCGBlendModeDarken alpha:0.25];
    
    /* Draw Status Indicator */
    UIImage *indicatorImage = [self _statusIndicatorImage];
    CGRect indicatorRect = {{1.0, CGRectGetMaxY(self.bounds)-indicatorImage.size.height}, {CGRectGetWidth(self.bounds)-2., indicatorImage.size.height}};
    [indicatorImage drawInRect:indicatorRect];    
    
    /* Draw selection overlay (Darken) */
    if (self.selected)
    { 
        [[self _selectionOverlayImage] drawInRect:self.bounds blendMode:kCGBlendModePlusDarker alpha:0.6]; 
    }
}

- (void)dealloc 
{
    [_entity release];
    [super dealloc];
}


@end
