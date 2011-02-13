//
//  LTIncidentTableViewCell.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 5/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTIncidentTableViewCell.h"

/*
 * The paper-ticket style Incident Table View Cell
 */

@implementation LTIncidentTableViewCell

@synthesize incidentLabel;

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) 
    {
        /* Configure cell based off LTTableViewCell */
        self.drawEntityStateBackgroundColor = NO;
        self.backgroundView = nil;
        self.backgroundColor = [UIColor clearColor];
        
        /* The 'ticket' background image view.
         * This view is rotated with a transform so all 
         * other views must exist within it
         */
        ticket = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
        double angle = 0;
        switch (arc4random()%2)
        {
            case 0:
                angle = -0.2;
                break;
            case 1:
                angle = 0.2;
                break;
        }
        ticket.transform = incidentLabel.transform = CGAffineTransformMakeRotation(angle * M_PI / 180);
        ticket.image = [UIImage imageNamed:@"IncidentPaperTicket1.png"];
        ticket.clipsToBounds = NO;
        [self addSubview:ticket];
        
        /* Create Label
         * Here we create a new one and add it to the ticket,
         * to make sure it's a subview of ticket and rotated
         */
        incidentLabel = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
        incidentLabel.textColor = [[UIColor blackColor] colorWithAlphaComponent:0.8];
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
        { incidentLabel.font = [UIFont fontWithName:@"Chalkduster" size:12.0]; }
        else
        { incidentLabel.font = [UIFont fontWithName:@"MarkerFelt-Wide" size:12.0]; }
        incidentLabel.backgroundColor = [UIColor clearColor];        
        [ticket addSubview:incidentLabel];
        
        /* Flag */
        flag = [[UIImageView alloc] initWithFrame:CGRectZero];
        flag.contentMode = UIViewContentModeCenter;
        [ticket addSubview:flag];
        
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)dealloc
{
    [super dealloc];
}

- (void)setEntityState:(int)entityState
{
    [super setEntityState:entityState];
    switch (entityState) 
    {
        case 3:
            flag.image = [UIImage imageNamed:@"IncidentFlag-Red.png"];
            break;
        case 2:
            flag.image = [UIImage imageNamed:@"IncidentFlag-Orange.png"];
            break;
        case 1:
            flag.image = [UIImage imageNamed:@"IncidentFlag-Yellow.png"];
            break;
        case 0:
            flag.image = [UIImage imageNamed:@"IncidentFlag-Green.png"];
            break;
        default:
            flag.image = [UIImage imageNamed:@"IncidentFlag-Gray.png"];
            break;
    }
}

- (void)layoutSubviews
{
    /* Layout the ticket image -- image is always 288x40 */
    CGRect ticketRect = {{CGRectGetMidX(self.bounds)-(288.*0.5), CGRectGetMidY(self.bounds)-(40.*0.5)}, {288., 40.}};
    ticket.frame = ticketRect;
    
    /* Layout Flag -- Square to the left inside the ticket */
    CGRect flagRect = {{4., 0.}, {CGRectGetHeight(ticket.bounds), CGRectGetHeight(ticket.bounds)}};
    flag.frame = flagRect;
    
    /* Layout Label -- To the right of the flag, plus offset */
    CGFloat labelXOffset = 4.;
    CGRect labelRect = {{CGRectGetMaxX(flagRect)+labelXOffset, 0.}, 
        {CGRectGetWidth(ticket.bounds)-CGRectGetMaxX(flagRect)-labelXOffset, CGRectGetHeight(ticket.bounds)}};
    incidentLabel.frame = labelRect;
}

@end
