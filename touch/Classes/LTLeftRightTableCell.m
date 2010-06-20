//
//  LTLeftRightTableCell.m
//  Lithium
//
//  Created by James Wilson on 21/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTLeftRightTableCell.h"


@implementation LTLeftRightTableCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) 
	{
        UIView *myContentView = self.contentView;
        
        self.leftLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor whiteColor] fontSize:18.0 bold:YES]; 
		self.leftLabel.textAlignment = UITextAlignmentLeft;
		self.leftLabel.text = @"";
		[myContentView addSubview:self.leftLabel];
		[self.leftLabel release];
		
        self.rightLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor lightGrayColor] fontSize:16.0 bold:NO];
		self.rightLabel.textAlignment = UITextAlignmentRight;
		self.rightLabel.text = @"";
		[myContentView addSubview:self.rightLabel];
		[self.rightLabel release];
    }
    return self;
}

- (void)layoutSubviews 
{
    
#define LEFT_COLUMN_OFFSET 12
#define LEFT_COLUMN_WIDTH 190
	
#define RIGHT_COLUMN_OFFSET 8
	
#define UPPER_ROW_TOP 7
    
    [super layoutSubviews];
    CGRect contentRect = self.contentView.bounds;
	
	CGFloat boundsX = contentRect.origin.x;
	CGRect frame;
	
	// Place the name labels.
	frame = CGRectMake(boundsX + LEFT_COLUMN_OFFSET, UPPER_ROW_TOP, LEFT_COLUMN_WIDTH, 32);
	self.leftLabel.frame = frame;
	
	// Place the value label 
	CGFloat rightOffset = LEFT_COLUMN_OFFSET + LEFT_COLUMN_WIDTH + RIGHT_COLUMN_OFFSET;
	frame = CGRectMake(boundsX + rightOffset, UPPER_ROW_TOP, contentRect.size.width - rightOffset - 12, 32);
	self.rightLabel.frame = frame;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
	/*
	 Views are drawn most efficiently when they are opaque and do not have a clear background, so in newLabelForMainText: the labels are made opaque and given a white background.  To show selection properly, however, the views need to be transparent (so that the selection color shows through).  
     */
	[super setSelected:selected animated:animated];
	
	self.leftLabel.highlighted = selected;
	self.leftLabel.opaque = !selected;
	
	self.rightLabel.highlighted = selected;
	self.rightLabel.opaque = !selected;
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
	newLabel.font = font;
	newLabel.textColor = [UIColor whiteColor];
	newLabel.highlightedTextColor = [UIColor whiteColor];
	newLabel.shadowColor = [UIColor blackColor];
	newLabel.shadowOffset = CGSizeMake(0.0, -1.0);		
		
	return newLabel;
}

- (void)dealloc {
    [super dealloc];
}

@synthesize leftLabel;
@synthesize rightLabel;

- (void) setText:(NSString *)value
{ return; }

@end
