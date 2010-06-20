//
//  LTActionInfoCell.m
//  Lithium
//
//  Created by James Wilson on 5/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTActionInfoCell.h"


@implementation LTActionInfoCell

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) 
	{
		iconImageView = [[UIImageView alloc] initWithFrame:CGRectZero];
		iconImageView.image = [UIImage imageNamed:@"tools_48.tif"];
		[self addSubview:iconImageView];
		[iconImageView autorelease];
		
		descLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		descLabel.backgroundColor = [UIColor clearColor];
		descLabel.opaque = YES;
		descLabel.textColor = [UIColor darkTextColor];
		descLabel.highlightedTextColor = [UIColor darkTextColor];
		descLabel.font = [UIFont boldSystemFontOfSize:18.0];
		descLabel.textAlignment = UITextAlignmentLeft;
		descLabel.shadowColor = [UIColor whiteColor];
		descLabel.shadowOffset = CGSizeMake(0.0, 1.0);
		descLabel.text = @"Action Description";
		[self addSubview:descLabel];
		[descLabel release];

		scriptLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		scriptLabel.backgroundColor = [UIColor clearColor];
		scriptLabel.opaque = YES;
		scriptLabel.textColor = [UIColor darkTextColor];
		scriptLabel.highlightedTextColor = [UIColor darkTextColor];
		scriptLabel.font = [UIFont systemFontOfSize:14.0];
		scriptLabel.textAlignment = UITextAlignmentLeft;
		scriptLabel.shadowColor = [UIColor whiteColor];
		scriptLabel.shadowOffset = CGSizeMake(0.0, 1.0);
		scriptLabel.text = @"script.name";
		[self addSubview:scriptLabel];
		[scriptLabel release];

		executionLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		executionLabel.backgroundColor = [UIColor clearColor];
		executionLabel.opaque = YES;
		executionLabel.textColor = [UIColor darkTextColor];
		executionLabel.highlightedTextColor = [UIColor darkTextColor];
		executionLabel.font = [UIFont systemFontOfSize:13.0];
		executionLabel.textAlignment = UITextAlignmentLeft;
		executionLabel.shadowColor = [UIColor whiteColor];
		executionLabel.shadowOffset = CGSizeMake(0.0, 1.0);
		executionLabel.text = @"Executes automatically after XXXX seconds";
		[self addSubview:executionLabel];
		[executionLabel release];

		runLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		runLabel.backgroundColor = [UIColor clearColor];
		runLabel.opaque = YES;
		runLabel.textColor = [UIColor darkTextColor];
		runLabel.highlightedTextColor = [UIColor darkTextColor];
		runLabel.font = [UIFont systemFontOfSize:13.0];
		runLabel.textAlignment = UITextAlignmentLeft;
		runLabel.shadowColor = [UIColor whiteColor];
		runLabel.shadowOffset = CGSizeMake(0.0, 1.0);
		runLabel.text = @"Will re-run every XXXX seconds";
		[self addSubview:runLabel];
		[runLabel release];

		runCountLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		runCountLabel.backgroundColor = [UIColor clearColor];
		runCountLabel.opaque = YES;
		runCountLabel.textColor = [UIColor darkTextColor];
		runCountLabel.highlightedTextColor = [UIColor darkTextColor];
		runCountLabel.font = [UIFont systemFontOfSize:13.0];
		runCountLabel.textAlignment = UITextAlignmentLeft;
		runCountLabel.shadowColor = [UIColor whiteColor];
		runCountLabel.shadowOffset = CGSizeMake(0.0, 1.0);
		runCountLabel.text = @"Has already run XXXX times.";
		[self addSubview:runCountLabel];
		[runCountLabel release];
    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
	/* Do nothing on select */
}

- (void)layoutSubviews 
{
//	CGRect contentRect = self.contentView.frame;
	/* Esure clear background is used */
	self.backgroundView.frame = CGRectZero;

	/* Layout */
	iconImageView.frame = CGRectMake(20.0, 8.0, 48.0, 48.0);
	descLabel.frame = CGRectMake(CGRectGetMaxX(iconImageView.frame)+10.0, 12.0, 
								 320.0 - (CGRectGetMaxX(iconImageView.frame) + 10.0) - 20.0, 18.0);
	scriptLabel.frame = CGRectMake(CGRectGetMaxX(iconImageView.frame)+10.0, 32.0, 
								   320.0 - (CGRectGetMaxX(iconImageView.frame) + 10.0) - 20.0, 18.0);
	executionLabel.frame = CGRectMake(CGRectGetMinX(iconImageView.frame), CGRectGetMaxY(iconImageView.frame)+40,
									  320 - (2 * CGRectGetMinX(iconImageView.frame)), 16.0);
	runLabel.frame = CGRectMake(CGRectGetMinX(iconImageView.frame), CGRectGetMaxY(executionLabel.frame)+6,
									  320.0 - (2 * CGRectGetMinX(iconImageView.frame)), 16.0);
	runCountLabel.frame = CGRectMake(CGRectGetMinX(iconImageView.frame), CGRectGetMaxY(runLabel.frame)+6,
								320.0 - (2 * CGRectGetMinX(iconImageView.frame)), 16.0);
}

- (void)dealloc {
    [super dealloc];
}

@synthesize action;
- (void) setAction:(LTAction *)value
{
	[action release];
	action = [value retain];
	descLabel.text = action.desc;
	scriptLabel.text = action.scriptFile;
	if (action.activationMode == 1)
	{
		/* Auto */
		if (action.delay == 0.0)
		{
			/* Immediate */
			executionLabel.text = @"Executes immediately when Incident is raised.";
		}
		else
		{
			/* Ater Delay */
			executionLabel.text = [NSString stringWithFormat:@"Executes automatically after %.0f seconds.", action.delay];
		}
	}
	else
	{
		/* Manual */
		executionLabel.text = @"Manual execution only.";
	}
	
	if (action.rerun)
	{
		/* Re-Runs */
		runLabel.text = [NSString stringWithFormat:@"Will re-run every %.0f minutes.", action.rerunDelay];
	}
	else
	{
		/* Does not re-run */
		runLabel.text = @"Runs once only.";
	}
	
	if (action.runCount > 0)
	{
		runCountLabel.text = [NSString stringWithFormat:@"Has already run %i times.", action.runCount];
	}
	else
	{ 
		runCountLabel.text = @"Has not run yet.";
	}
	
	if (action.runState == 0)
	{
		iconImageView.image = [UIImage imageNamed:@"tools_grey_48.tif"];
	}
	else
	{
		iconImageView.image = [UIImage imageNamed:@"tools_48.tif"];
	}
	
}

@end
