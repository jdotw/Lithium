//
//  LTDeviceEditTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 21/12/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTEntity;

@interface LTDeviceEditTableViewController : UITableViewController <UITextFieldDelegate>
{
	/* Device Properties */
	UITextField *devDesc;
	UITextField *devIPAddress;
	UITextField *devLOMIPAddress;
	int devSNMPVersion;
	UITextField *devSNMPCommunity;
	int devSNMPAuthMethod;
	UITextField *devSNMPAuthPassword;
	int devSNMPPrivacyMethod;
	UITextField *devSNMPPrivacyPassword;
	UITextField *devUsername;
	UITextField *devPassword;
	UITextField *devLOMUsername;
	UITextField *devLOMPassword;
	NSString *devVendorModule;
	int devRefreshInterval;
	int devProtocol;
	UISwitch *devUseICMP;
	UISwitch *devUseProcessList;
	UISwitch *devUseLOM;
	
	/* State */
	UITextField *editingTextField;
}

- (id)initWithDeviceToEdit:(LTEntity *)initDevice;
- (id)initForNewDeviceAtSite:(LTEntity *)initSite;


@property (nonatomic,retain) LTEntity *device;
@property (nonatomic,retain) LTEntity *site;

/* Device Properties that are externally accessible */
@property (nonatomic,assign) int devSNMPVersion;
@property (nonatomic,assign) int devSNMPAuthMethod;
@property (nonatomic,assign) int devSNMPPrivacyMethod;
@property (nonatomic,retain) NSString *devVendorModule;
@property (nonatomic,assign) int devRefreshInterval;
@property (nonatomic,assign) int devProtocol;

@end
