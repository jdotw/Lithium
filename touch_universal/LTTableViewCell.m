//
//  LTTableViewCell.m
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTTableViewCell.h"

#import "LTTableViewCellBackground.h"
#import "LTTableViewCellSelectedBackground.h"

@implementation LTTableViewCell

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:UITableViewCellStyleDefault reuseIdentifier:reuseIdentifier]) 
	{
		self.backgroundView = [[LTTableViewCellBackground alloc] initWithFrame:CGRectZero];
		((LTTableViewCellBackground *)self.backgroundView).cell = self;
		self.selectedBackgroundView = [[LTTableViewCellSelectedBackground alloc] initWithFrame:CGRectZero];
		self.backgroundColor = [UIColor colorWithRed:0.0
											   green:0.0
												blue:0.0
											   alpha:0.5];
		self.textLabel.textColor = [UIColor whiteColor];
		self.detailTextLabel.textColor = [UIColor whiteColor];
    }
    return self;	
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
    [super setSelected:selected animated:animated];
}

- (void)dealloc {
    [super dealloc];
}


@end
