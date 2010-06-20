//
//  LTTwoLineTableCell.m
//  Lithium
//
//  Created by James Wilson on 1/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTTwoLineTableCell.h"


@implementation LTTwoLineTableCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) 
	{
        UIView *myContentView = self.contentView;
        
        // A label that displays the location of the earthquake.
        self.topLineLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor whiteColor] fontSize:18.0 bold:YES]; 
		self.topLineLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.topLineLabel];
		[self.topLineLabel release];
        
        // A label that displays the date of the earthquake.
        self.bottomLineLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor lightGrayColor] fontSize:11.0 bold:NO];
		self.bottomLineLabel.textAlignment = UITextAlignmentLeft; // default
		[myContentView addSubview:self.bottomLineLabel];
		[self.bottomLineLabel release];
    }
    return self;
}

- (void)layoutSubviews 
{
#define LEFT_COLUMN_OFFSET 12
#define LEFT_COLUMN_WIDTH 190
	
#define RIGHT_COLUMN_OFFSET 6
	
#define UPPER_ROW_TOP 2
#define LOWER_ROW_TOP 27
    
    [super layoutSubviews];
    CGRect contentRect = self.contentView.bounds;
	
	CGFloat leftIndent;
	if (dotImage) leftIndent = 36;
	else leftIndent = 12;
	
	CGFloat leftWidth;
	if (hasRightLabel) leftWidth = 190.0f;
	else leftWidth = (self.contentView.bounds.size.width - leftIndent) - 2.0f;
	
	CGFloat rightIndent = leftIndent + leftWidth + 8.0f;
	
	CGFloat rightWidth;
	rightWidth = contentRect.size.width - rightIndent;
	
	CGRect frame;
	
	// Place the top label.
	frame = CGRectMake(leftIndent, UPPER_ROW_TOP, leftWidth, 28.0f);
	self.topLineLabel.frame = frame;
	
	// Place the date label.
	frame = CGRectMake(leftIndent, LOWER_ROW_TOP, leftWidth, 12.0f);
	self.bottomLineLabel.frame = frame;
	
	// Place the right label 
	if (hasRightLabel)
	{
		frame = CGRectMake(rightIndent, 7.0f, rightWidth, 32.0f);
		self.rightLabel.frame = frame;		
	}
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
	/*
	 Views are drawn most efficiently when they are opaque and do not have a clear background, so in newLabelForMainText: the labels are made opaque and given a white background.  To show selection properly, however, the views need to be transparent (so that the selection color shows through).  
     */
	[super setSelected:selected animated:animated];
	
	self.topLineLabel.highlighted = selected;
	self.topLineLabel.opaque = !selected;
	
	self.bottomLineLabel.highlighted = selected;
	self.bottomLineLabel.opaque = !selected;
}

- (UILabel *)newLabelWithPrimaryColor:(UIColor *)primaryColor selectedColor:(UIColor *)selectedColor fontSize:(CGFloat)fontSize bold:(BOOL)bold
{
	/*
	 Create and configure a label.
	 */
	
    UIFont *font;
    if (bold) 
	{
        font = [UIFont boldSystemFontOfSize:fontSize];
    } else 
	{
        font = [UIFont systemFontOfSize:fontSize];
    }
    
    /*
	 Views are drawn most efficiently when they are opaque and do not have a clear background, so set these defaults.  To show selection properly, however, the views need to be transparent (so that the selection color shows through).  This is handled in setSelected:animated:.
	 */
	UILabel *newLabel = [[UILabel alloc] initWithFrame:CGRectZero];
	newLabel.backgroundColor = [UIColor clearColor];
	newLabel.opaque = YES;
	newLabel.textColor = primaryColor;
	newLabel.font = font;
	newLabel.textColor = [UIColor whiteColor];
	newLabel.highlightedTextColor = [UIColor whiteColor];
	newLabel.shadowColor = [UIColor blackColor];
	newLabel.shadowOffset = CGSizeMake(0.0, -1.0);		
	
	return newLabel;
}

- (void)dealloc 
{
    [super dealloc];
}

@synthesize topLineLabel;
@synthesize bottomLineLabel;
@synthesize dotImage;
- (void) setDotImage:(UIImage *)value
{
	[dotImage release];
	dotImage = [value retain];
	self.imageView.image = value;
}
@synthesize hasRightLabel;
- (void) setHasRightLabel:(BOOL)flag
{
	hasRightLabel = flag;
	if (hasRightLabel)
	{
        self.rightLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor whiteColor] fontSize:18.0 bold:YES];
		self.rightLabel.textAlignment = UITextAlignmentRight;
		[self.contentView addSubview:self.rightLabel];
		[self.rightLabel release];
	}	
	else
	{
		[self.rightLabel removeFromSuperview];
		self.rightLabel = nil;
	}
}
@synthesize rightLabel;

@end
