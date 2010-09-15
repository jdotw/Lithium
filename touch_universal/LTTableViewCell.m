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
		NSLog (@"Backview was %@", self.backgroundView);
		if (
#ifdef UI_USER_INTERFACE_IDIOM
			UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad
#else
			0
#endif
			)
		{
//			self.backgroundColor = [UIColor colorWithRed:1.0
//												   green:1.0
//													blue:1.0
//												   alpha:1.0];			
		}
		else 
		{
//			self.backgroundView = [[LTTableViewCellBackground alloc] initWithFrame:CGRectZero];
//			((LTTableViewCellBackground *)self.backgroundView).cell = self;
//			self.selectedBackgroundView = [[LTTableViewCellSelectedBackground alloc] initWithFrame:CGRectZero];
//			self.backgroundColor = [UIColor colorWithRed:0.0
//												   green:0.0
//													blue:0.0
//												   alpha:0.5];
		}

			
//		self.textColor = [UIColor redColor];
		
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
