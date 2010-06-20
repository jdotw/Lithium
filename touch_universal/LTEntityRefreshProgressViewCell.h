//
//  LTEntityRefreshProgressViewCell.h
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface LTEntityRefreshProgressViewCell : UITableViewCell 
{
	UIActivityIndicatorView *progressView;
	UILabel *progressLabel;
}

@property (readonly) UILabel *progressLabel;
@property (readonly) UIActivityIndicatorView *progressView;

@end
