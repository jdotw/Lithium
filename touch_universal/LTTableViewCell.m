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

@synthesize entityState, drawEntityStateBackgroundColor;

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if ((self = [super initWithStyle:style reuseIdentifier:reuseIdentifier])) 
	{
		self.backgroundView = [[LTTableViewCellBackground alloc] initWithFrame:CGRectZero];
		((LTTableViewCellBackground *)self.backgroundView).cell = self;
		self.selectedBackgroundView = [[LTTableViewCellSelectedBackground alloc] initWithFrame:CGRectZero];
		self.backgroundColor = [UIColor colorWithRed:0.0
											   green:0.0
												blue:0.0
											   alpha:0.5];
        self.opaque = NO;
		self.textLabel.backgroundColor = [UIColor clearColor];
		self.textLabel.textColor = [UIColor whiteColor];
		self.textLabel.shadowColor = [UIColor colorWithWhite:0.0 alpha:0.6];
		self.textLabel.shadowOffset = CGSizeMake(0.0, -1.);
		self.detailTextLabel.backgroundColor = [UIColor clearColor];
		self.detailTextLabel.textColor = [UIColor whiteColor];
		self.detailTextLabel.shadowColor = [UIColor colorWithWhite:0.0 alpha:0.6];
		self.detailTextLabel.shadowOffset = CGSizeMake(0.0, -1.);
		if (style == UITableViewCellStyleSubtitle) self.detailTextLabel.font = [UIFont systemFontOfSize:12.];
    }
    return self;	
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
    [super setSelected:selected animated:animated];
}

- (void) setEntityState:(int)value
{
	entityState = value;
	if ([self.backgroundView isMemberOfClass:[LTTableViewCellBackground class]])
	{ ((LTTableViewCellBackground *)self.backgroundView).entityState = self.entityState; }
	switch (entityState) {
		case -2:
		case -1:
			self.textLabel.alpha = 0.3;
			self.detailTextLabel.alpha = 0.3
			;
			break;
		default:
			self.textLabel.alpha = 1.0;
			self.detailTextLabel.alpha = 1.0;
			break;
	}
}

- (void) setDrawEntityStateBackgroundColor:(BOOL)value
{
	drawEntityStateBackgroundColor = value;
	if ([self.backgroundView isMemberOfClass:[LTTableViewCellBackground class]])
	{ ((LTTableViewCellBackground *)self.backgroundView).drawEntityStateBackgroundColor = self.drawEntityStateBackgroundColor; }
}

	

- (void)dealloc {
    [super dealloc];
}


@end
