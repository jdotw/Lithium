//
//  LTEntityRefreshProgressViewCell.m
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTEntityRefreshProgressViewCell.h"


@implementation LTEntityRefreshProgressViewCell

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) 
	{
        // Initialization code
		
		/*
		 Views are drawn most efficiently when they are opaque and do not have a clear background, so set these defaults.  To show selection properly, however, the views need to be transparent (so that the selection color shows through).  This is handled in setSelected:animated:.
		 */
		progressLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		progressLabel.backgroundColor = [UIColor clearColor];
		progressLabel.opaque = YES;
		progressLabel.textColor = [UIColor whiteColor];
		progressLabel.highlightedTextColor = [UIColor blackColor];
		progressLabel.font = [UIFont boldSystemFontOfSize:16.0];
		[self addSubview:progressLabel];
		
		progressView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
		progressView.hidesWhenStopped = YES;
		[self addSubview:progressView];
		
    }
    return self;
}

- (void)layoutSubviews 
{
	progressView.frame = CGRectMake(CGRectGetMidX(self.bounds) - (CGRectGetWidth(progressView.frame) * 0.5),
									 CGRectGetMidY(self.bounds) - (CGRectGetHeight(progressView.frame) * 0.5), 
									 CGRectGetWidth(progressView.frame), CGRectGetHeight(progressView.frame));
	
	CGSize progressLabelSize = [progressLabel.text sizeWithFont:[UIFont boldSystemFontOfSize:16.0]];
	
	if ([progressView isAnimating])
	{
		progressLabel.frame = CGRectMake(roundf(CGRectGetMidX(self.bounds) - (progressLabelSize.width * 0.5)),
										 roundf((CGRectGetMidY(self.bounds) - (progressLabelSize.height * 0.5)) + progressView.frame.size.height + 10.0), 
										 CGRectGetWidth(self.bounds), progressLabelSize.height);
	}
	else
	{
		progressLabel.frame = CGRectMake(roundf(CGRectGetMidX(self.bounds) - (progressLabelSize.width * 0.5)),
										 roundf((CGRectGetMidY(self.bounds) - (progressLabelSize.height * 0.5))), 
										 CGRectGetWidth(self.bounds), progressLabelSize.height);
	}
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {

    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}


- (void)dealloc 
{
	
    [super dealloc];
}

@synthesize progressLabel;
@synthesize progressView;


@end
