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

static float tapeVectors[16][2] = {
    { 0.1, 0.06 },
    { 0.12, 0.05 },
    { -0.15, 0.08 },
    { 0.15, 0.02 },
    { 0.20, -0.01 },
    { -0.01, 0.08 },
    { 0.02, -0.09 },
    { 0.30, 0.03 },
    { -0.02, -0.01 },
    { -0.30, 0.00 },
    { 0.11, -0.06 },
    { -0.08, -0.02 },
    { 0.01, 0.03 },
    { 0.04, -0.07 },
    { -0.16, 0.03 },
    { 0.23, -0.00 },
};
#define TAPE_VEC_COUNT 16

@implementation LTIncidentTableViewCell

@synthesize incidentLabel, row, lastRow, firstRow;

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
        ticket.transform = CGAffineTransformMakeRotation(angle * M_PI / 180);
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
        flag = [[[UIImageView alloc] initWithFrame:CGRectZero] autorelease];
        flag.contentMode = UIViewContentModeCenter;
        [ticket addSubview:flag];
        
        /* Create top tape view */
        topTapeView = [[[UIView alloc] initWithFrame:CGRectZero] autorelease];
        topTapeView.backgroundColor = [UIColor colorWithWhite:0.4 alpha:0.2];
        topTapeView.opaque = NO;
        [self addSubview:topTapeView];

        /* Create top tape view */
        bottomTapeView = [[[UIView alloc] initWithFrame:CGRectZero] autorelease];
        bottomTapeView.backgroundColor = [UIColor colorWithWhite:0.4 alpha:0.2];
        bottomTapeView.opaque = NO;
        [self addSubview:bottomTapeView];
        
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
    [super layoutSubviews];
    
    self.textLabel.frame = CGRectZero;
    self.detailTextLabel.frame = CGRectZero;
    
    NSLog (@"Asked to layuoit %@", self);
    /* Layout the ticket image -- image is always 288x40 */
    CGFloat ticketImageHeight = 50.;
    CGFloat ticketWidth = 288.;
    if (self.editing) ticketWidth = 240.;  // Switch to 240. width when editing to accomodate Clear button
    CGRect ticketRect = {{CGRectGetMidX(self.bounds)-(288.*0.5), CGRectGetMidY(self.bounds)-(ticketImageHeight*0.5)}, {ticketWidth, ticketImageHeight}};
    ticket.frame = ticketRect;
    
    /* Layout Flag -- Square to the left inside the ticket */
    CGRect flagRect = {{4., 0.}, {CGRectGetHeight(ticket.bounds), CGRectGetHeight(ticket.bounds)}};
    flag.frame = flagRect;
    
    /* Layout Label -- To the right of the flag, plus offset */
    CGFloat labelXOffset = 4.;
    CGRect labelRect = {{CGRectGetMaxX(flagRect)+labelXOffset, 0.}, 
        {CGRectGetWidth(ticket.bounds)-CGRectGetMaxX(flagRect)-labelXOffset, CGRectGetHeight(ticket.bounds)}};
    incidentLabel.frame = labelRect;

    /* Tape constants */
    CGFloat tapeHeight = 20.;
    CGFloat tapeWidth = 10.;
    
    /* Layout Top Tape */
//    if (!self.firstRow)
    {
        CGFloat tapeXShiftMultiple = tapeVectors[self.row % TAPE_VEC_COUNT][0];
        CGFloat tapeRotation = tapeVectors[self.row % TAPE_VEC_COUNT][1];
        CGRect topTapeRect = CGRectMake(CGRectGetMidX(ticket.bounds)-(CGRectGetWidth(ticket.bounds)*tapeXShiftMultiple), 
                                        CGRectGetMinY(ticket.bounds)-(tapeHeight*0.75), 
                                        tapeWidth, tapeHeight);
        topTapeRect = CGRectIntersection(topTapeRect, ticket.bounds);   // Keep it within the ticket
        topTapeView.frame = topTapeRect;
        topTapeView.transform = CGAffineTransformMakeRotation(tapeRotation);
    }

    /* Layout Bottom Tape */
    if (!self.lastRow)
    {
        CGFloat tapeXShiftMultiple = tapeVectors[(self.row+1) % TAPE_VEC_COUNT][0];
        CGFloat tapeRotation = tapeVectors[(self.row+1) % TAPE_VEC_COUNT][1];
        CGRect bottomTapeRect = CGRectMake(CGRectGetMidX(ticket.bounds)-(CGRectGetWidth(ticket.bounds)*tapeXShiftMultiple), 
                                           CGRectGetMaxY(ticket.bounds)-(tapeHeight*0.75), 
                                        tapeWidth, tapeHeight);
        bottomTapeRect = CGRectIntersection(bottomTapeRect, ticket.bounds);   // Keep it within the ticket
        bottomTapeView.frame = bottomTapeRect;
        bottomTapeView.transform = CGAffineTransformMakeRotation(tapeRotation);
    }

}

//- (void)willTransitionToState:(UITableViewCellStateMask)state
//{
//    NSLog (@"%@ got willTransitionToState:", self);
//}

@end
