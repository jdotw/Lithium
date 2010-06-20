//
//  LTEntityDescriptor.m
//  Lithium
//
//  Created by James Wilson on 28/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTEntityDescriptor.h"


@implementation LTEntityDescriptor

#pragma mark "Constructors"

- (LTEntityDescriptor *) initWithEntity:(LTEntity *)entity
{
	[self init];
	
	if (entity.parent.entityDescriptor)
	{
		/* Clone from parent */
		LTEntityDescriptor *parentEntDesc = (LTEntityDescriptor *)entity.parent.entityDescriptor;
		self.type = parentEntDesc.type + 1;
		self.name = entity.name;
		self.desc = entity.desc;
		self.custName = parentEntDesc.custName;
		self.custDesc = parentEntDesc.custDesc;
		self.siteName = parentEntDesc.siteName;
		self.siteDesc = parentEntDesc.siteDesc;
		self.siteSuburb = parentEntDesc.siteSuburb;
		self.devName = parentEntDesc.devName;
		self.devDesc = parentEntDesc.devDesc;
		self.cntName = parentEntDesc.cntName;
		self.cntDesc = parentEntDesc.cntDesc;
		self.objName = parentEntDesc.objName;
		self.objDesc = parentEntDesc.objDesc;
		self.metName = parentEntDesc.metName;
		self.metDesc = parentEntDesc.trgDesc;
		if (entity.type == 7)
		{
			self.trgName = entity.name;
			self.trgDesc = entity.desc;
		}
		else if (entity.type == 6)
		{
			self.metName = entity.name;
			self.metDesc = entity.desc;
		}
		else if (entity.type == 5)
		{
			self.objName = entity.name;
			self.objDesc = entity.desc;
		}
		else if (entity.type == 4)
		{
			self.cntName = entity.name;
			self.cntDesc = entity.desc;
		}
		else if (entity.type == 3)
		{
			self.devName = entity.name;
			self.devDesc = entity.desc;
		}
		else if (entity.type == 2)
		{
			self.siteName = entity.name;
			self.siteDesc = entity.desc;
		}
		else if (entity.type == 1)
		{
			self.custName = entity.name;
			self.custDesc = entity.desc;
		}		
	}
	else 
	{
		/* Build from hierarchy */
		self.type = entity.type;
		self.name = entity.name;
		self.desc = entity.desc;
		
		while (entity)
		{
			if (entity.type == 7)
			{
				self.trgName = entity.name;
				self.trgDesc = entity.desc;
			}
			else if (entity.type == 6)
			{
				self.metName = entity.name;
				self.metDesc = entity.desc;
			}
			else if (entity.type == 5)
			{
				self.objName = entity.name;
				self.objDesc = entity.desc;
			}
			else if (entity.type == 4)
			{
				self.cntName = entity.name;
				self.cntDesc = entity.desc;
			}
			else if (entity.type == 3)
			{
				self.devName = entity.name;
				self.devDesc = entity.desc;
			}
			else if (entity.type == 2)
			{
				self.siteName = entity.name;
				self.siteDesc = entity.desc;
			}
			else if (entity.type == 1)
			{
				self.custName = entity.name;
				self.custDesc = entity.desc;
			}
			else
			{ break; }
			
			entity = entity.parent;
		}		
	}
	
	return self;
}

- (LTEntityDescriptor *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	
	self.type = [decoder decodeIntForKey:@"type"];
	self.name = [decoder decodeObjectForKey:@"name"];
	self.desc = [decoder decodeObjectForKey:@"desc"];

	self.custName = [decoder decodeObjectForKey:@"custName"];
	self.custDesc = [decoder decodeObjectForKey:@"custDesc"];

	self.siteName = [decoder decodeObjectForKey:@"siteName"];
	self.siteDesc = [decoder decodeObjectForKey:@"siteDesc"];
	self.siteSuburb = [decoder decodeObjectForKey:@"siteSuburb"];

	self.devName = [decoder decodeObjectForKey:@"devName"];
	self.devDesc = [decoder decodeObjectForKey:@"devDesc"];

	self.cntName = [decoder decodeObjectForKey:@"cntName"];
	self.cntDesc = [decoder decodeObjectForKey:@"cntDesc"];
	
	self.objName = [decoder decodeObjectForKey:@"objName"];
	self.objDesc = [decoder decodeObjectForKey:@"objDesc"];

	self.metName = [decoder decodeObjectForKey:@"metName"];
	self.metDesc = [decoder decodeObjectForKey:@"metDesc"];

	self.trgName = [decoder decodeObjectForKey:@"trgName"];
	self.trgDesc = [decoder decodeObjectForKey:@"trgDesc"];
	
	return self;
}

- (void) encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeInt:type forKey:@"type"];
	[encoder encodeObject:name forKey:@"name"];
	[encoder encodeObject:desc forKey:@"desc"];

	[encoder encodeObject:custName forKey:@"custName"];
	[encoder encodeObject:custDesc forKey:@"custDesc"];

	[encoder encodeObject:siteName forKey:@"siteName"];
	[encoder encodeObject:siteDesc forKey:@"siteDesc"];
	[encoder encodeObject:siteSuburb forKey:@"siteSuburb"];

	[encoder encodeObject:devName forKey:@"devName"];
	[encoder encodeObject:devDesc forKey:@"devDesc"];

	[encoder encodeObject:cntName forKey:@"cntName"];
	[encoder encodeObject:cntDesc forKey:@"cntDesc"];

	[encoder encodeObject:objName forKey:@"objName"];
	[encoder encodeObject:objDesc forKey:@"objDesc"];

	[encoder encodeObject:metName forKey:@"metName"];
	[encoder encodeObject:metDesc forKey:@"metDesc"];

	[encoder encodeObject:trgName forKey:@"trgName"];
	[encoder encodeObject:trgDesc forKey:@"trgDesc"];
}

- (LTEntityDescriptor *) copy
{
	NSData *data = [NSKeyedArchiver archivedDataWithRootObject:self];
	return [NSKeyedUnarchiver unarchiveObjectWithData:data];
}

#pragma mark "XML Methods"

- (NSString *) xmlNodeString
{
	NSMutableString *xmlString = [NSMutableString stringWithString:@"<entity_descriptor>"];
	[xmlString appendFormat:@"<type_num>%i</type_num>", self.type];
	[xmlString appendFormat:@"<name>%@</name>", self.name];
	[xmlString appendFormat:@"<desc>%@</desc>", self.desc];
	if (self.custName) [xmlString appendFormat:@"<cust_name>%@</cust_name>", self.custName];
	if (self.custDesc) [xmlString appendFormat:@"<cust_desc>%@</cust_desc>", self.custDesc];
	if (self.siteName) [xmlString appendFormat:@"<site_name>%@</site_name>", self.siteName];
	if (self.siteDesc) [xmlString appendFormat:@"<site_desc>%@</site_desc>", self.siteDesc];
	if (self.devName) [xmlString appendFormat:@"<dev_name>%@</dev_name>", self.devName];
	if (self.devDesc) [xmlString appendFormat:@"<dev_desc>%@</dev_desc>", self.devDesc];
	if (self.cntName) [xmlString appendFormat:@"<cnt_name>%@</cnt_name>", self.cntName];
	if (self.cntDesc) [xmlString appendFormat:@"<cnt_desc>%@</cnt_desc>", self.cntDesc];
	if (self.objName) [xmlString appendFormat:@"<obj_name>%@</obj_name>", self.objName];
	if (self.objDesc) [xmlString appendFormat:@"<obj_desc>%@</obj_desc>", self.objDesc];
	if (self.metName) [xmlString appendFormat:@"<met_name>%@</met_name>", self.metName];
	if (self.metDesc) [xmlString appendFormat:@"<met_desc>%@</met_desc>", self.metDesc];
	if (self.trgName) [xmlString appendFormat:@"<trg_name>%@</trg_name>", self.trgName];
	if (self.trgDesc) [xmlString appendFormat:@"<trg_desc>%@</trg_desc>", self.trgDesc];
	[xmlString appendString:@"</entity_descriptor>"];
	
	return xmlString;
}

+ (LTEntityDescriptor *) entityDescriptorFromXml:(LCXMLNode *)xmlNode
{
	LTEntityDescriptor *entDesc = [[LTEntityDescriptor alloc] init];
	
	entDesc.type = [[xmlNode.properties objectForKey:@"type_num"] intValue];
	
	entDesc.name = [xmlNode.properties objectForKey:@"name"];
	entDesc.desc = [xmlNode.properties objectForKey:@"desc"];
	entDesc.opState = [[xmlNode.properties objectForKey:@"opstate_num"] intValue];
	entDesc.adminState = [[xmlNode.properties objectForKey:@"adminstate_num"] intValue];

	entDesc.custName = [xmlNode.properties objectForKey:@"cust_name"];
	entDesc.custDesc = [xmlNode.properties objectForKey:@"cust_desc"];
	entDesc.custOpState = [[xmlNode.properties objectForKey:@"cust_opstate_num"] intValue];
	entDesc.custAdminState = [[xmlNode.properties objectForKey:@"cust_adminstate_num"] intValue];

	entDesc.siteName = [xmlNode.properties objectForKey:@"site_name"];
	entDesc.siteDesc = [xmlNode.properties objectForKey:@"site_desc"];
	entDesc.siteSuburb = [xmlNode.properties objectForKey:@"site_suburb"];
	entDesc.siteOpState = [[xmlNode.properties objectForKey:@"site_opstate_num"] intValue];
	entDesc.siteAdminState = [[xmlNode.properties objectForKey:@"site_adminstate_num"] intValue];
	
	entDesc.devName = [xmlNode.properties objectForKey:@"dev_name"];
	entDesc.devDesc = [xmlNode.properties objectForKey:@"dev_desc"];
	entDesc.devOpState = [[xmlNode.properties objectForKey:@"dev_opstate_num"] intValue];
	entDesc.devAdminState = [[xmlNode.properties objectForKey:@"dev_adminstate_num"] intValue];

	entDesc.cntName = [xmlNode.properties objectForKey:@"cnt_name"];
	entDesc.cntDesc = [xmlNode.properties objectForKey:@"cnt_desc"];
	entDesc.cntOpState = [[xmlNode.properties objectForKey:@"cnt_opstate_num"] intValue];
	entDesc.cntAdminState = [[xmlNode.properties objectForKey:@"cnt_adminstate_num"] intValue];

	entDesc.objName = [xmlNode.properties objectForKey:@"obj_name"];
	entDesc.objDesc = [xmlNode.properties objectForKey:@"obj_desc"];
	entDesc.objOpState = [[xmlNode.properties objectForKey:@"obj_opstate_num"] intValue];
	entDesc.objAdminState = [[xmlNode.properties objectForKey:@"obj_adminstate_num"] intValue];
	
	entDesc.metName = [xmlNode.properties objectForKey:@"met_name"];
	entDesc.metDesc = [xmlNode.properties objectForKey:@"met_desc"];
	entDesc.metOpState = [[xmlNode.properties objectForKey:@"met_opstate_num"] intValue];
	entDesc.metAdminState = [[xmlNode.properties objectForKey:@"met_adminstate_num"] intValue];
	
	entDesc.trgName = [xmlNode.properties objectForKey:@"trg_name"];
	entDesc.trgDesc = [xmlNode.properties objectForKey:@"trg_desc"];
	entDesc.trgOpState = [[xmlNode.properties objectForKey:@"trg_opstate_num"] intValue];
	entDesc.trgAdminState = [[xmlNode.properties objectForKey:@"trg_adminstate_num"] intValue];
	
	entDesc.resourceAddress = [xmlNode.properties objectForKey:@"resaddr"];
		
	return [entDesc autorelease];
}

#pragma mark "Properties"

@synthesize type;
@synthesize name;
@synthesize desc;
@synthesize opState;
@synthesize adminState;

@synthesize custName;
@synthesize custDesc;
@synthesize custOpState;
@synthesize custAdminState;
@synthesize siteName;
@synthesize siteDesc;
@synthesize siteSuburb;
@synthesize siteOpState;
@synthesize siteAdminState;
@synthesize devName;
@synthesize devDesc;
@synthesize devOpState;
@synthesize devAdminState;
@synthesize cntName;
@synthesize cntDesc;
@synthesize cntOpState;
@synthesize cntAdminState;
@synthesize objName;
@synthesize objDesc;
@synthesize objOpState;
@synthesize objAdminState;
@synthesize metName;
@synthesize metDesc;
@synthesize metOpState;
@synthesize metAdminState;
@synthesize trgName;
@synthesize trgDesc;
@synthesize trgOpState;
@synthesize trgAdminState;

- (NSString *) entityAddress
{
	NSMutableString *address = [NSMutableString stringWithFormat:@"%i", self.type];
	if (self.type >= 1) [address appendFormat:@":%@", self.custName];
	if (self.type >= 2) [address appendFormat:@":%@", self.siteName];
	if (self.type >= 3) [address appendFormat:@":%@", self.devName];
	if (self.type >= 4) [address appendFormat:@":%@", self.cntName];
	if (self.type >= 5) [address appendFormat:@":%@", self.objName];
	if (self.type >= 6) [address appendFormat:@":%@", self.metName];
	if (self.type >= 7) [address appendFormat:@":%@", self.trgName];
	return address;
}

@synthesize resourceAddress;

@end

