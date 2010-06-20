//
//  LTEntityTableViewCell.m
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTEntityTableViewCell.h"


@implementation LTEntityTableViewCell

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) 
	{
        // Initialization code
		self.textLabel.textColor = [UIColor whiteColor];
		self.textLabel.highlightedTextColor = [UIColor whiteColor];
		self.textLabel.shadowColor = [UIColor blackColor];
		self.textLabel.shadowOffset = CGSizeMake(0.0, -1.0);		
    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated {

    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}


- (void)dealloc {
	[entity release];
    [super dealloc];
}


@synthesize entity;
- (void) setEntity:(LTEntity *)value
{
	[entity release];
	entity = [value retain];
}

@end
