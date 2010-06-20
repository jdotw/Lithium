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
        // Initialization code
		self.backgroundView = [[LTTableViewCellBackground alloc] initWithFrame:CGRectZero];
		((LTTableViewCellBackground *)self.backgroundView).cell = self;
		self.selectedBackgroundView = [[LTTableViewCellSelectedBackground alloc] initWithFrame:CGRectZero];
    }
    return self;	
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {

    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)dealloc {
    [super dealloc];
}


@end
