#! /usr/bin/perl 
use XML::Simple; 
use Net::SMTP; 
use Data::Dumper; 
use Time::CTime; 
use HTTP::Request::Common qw(POST);
use LWP::UserAgent;


###################################
#  Script Configuration Variables
###################################

%xmlstruct = (  'desc' => "iPhone Push Notification Script",
                'version' => '1.0.0',
                'config_variable'=> [
                 ]
            );


###################################
#  Command processing
###################################

if ($ARGV[0] eq "info") { print &generateConfig($xmlroot, \%xmlstruct); }
 
if ($ARGV[0] eq "report") { reportIncident(); }

if ($ARGV[0] eq "report_recurring") { reportRecurringIncident($ARGV[$#ARGV]); }

if ($ARGV[0] eq "transition") { transitionIncident($ARGV[$#ARGV]); }

if ($ARGV[0] eq "remind") { remindIncident($ARGV[$#ARGV]); }

if ($ARGV[0] eq "clear") { clearIncident($ARGV[$#ARGV]); }

if ($ARGV[0] eq "clear_recurring") { clearRecurringIncident($ARGV[$#ARGV]); }

###################################
# Handling of an incident report
#
# Sends an alert text for Impaired and
# Critical events. Anything else just gets
# a badge count update
###################################

sub reportIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
  
  # Design alert text
  my $alert;
  if (%action->{state} >= 2)
  { $alert = "New Incident: " . incidentAlertText(%action); }
  else
  { $alert = ""; }

  # Send Alert
  sendAlertNotification ($alert, %action);
}


##########################################
# Handling of a recurring incident report
#
# Sends an alert text for Impaired and
# Critical events. Anything else just gets
# a badge count update
##########################################

sub reportRecurringIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
  
  # Check occurrence count
  #
  # Only send a recurring incident account on 
  # the second occurrence (the first recurrence)
  #
  # All future reports are suppressed
  #
  if (%action->{occurrences} eq "2")
  {
    # Design email content
    my $alert;
    if (%action->{highest_state} >= 2)
    { $alert = "Recurring Incident: " . incidentAlertText(%action); }
    else
    { $alert = ""; }

    # Send Alert
    sendAlertNotification ($alert, %action);
  }
}

###################################
# Handling of an incident transition
#
# Only sends an escalation or downgrade
# notice if the current state is higher
# or lower than the highest/lowest opstate 
# seen in the life of the incident. 
#
# Alert text is only sent if the highest
# state was impaired or critical
###################################

sub transitionIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
  
  # Design email content
  my $alert;
  if (%action->{highest_state} >= 2)
  {
    if (%action->{state} gt %action->{highest_state}) 
    { $alert = "Incident Escalation: " . incidentAlertText(%action); }
    elsif (%action->{state} lt %action->{lowest_state}) 
    { $alert = "Incident Downgrade: " . incidentAlertText(%action); }
    else { $alert = ""; }
  }
  else
  { $alert = ""; }
    
  # Send Alert
  sendAlertNotification ($alert, %action);
}

############################################
# Handling of an incident reminder (re-run)
#
# Only sends for Impaired or Critical events
############################################

sub remindIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
   
  if (%action->{highest_state} >= 2)
  { 
    # Design alert content
    my $alert = "Reminder: " . incidentAlertText(%action);

    # Send Alert
    sendAlertNotification ($alert, %action);
  }
}

###########################################
# Handling of incident cleared
#
# Only sends txt alert for Impaired or
# Critical events
###########################################

sub clearIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
  
  # Design email content
  my $alert;
  if (%action->{highest_state} >= 2)
  { $alert = "Cleared: " . incidentAlertText(%action); }
  else
  { $alert = ""; }

  # Send Alert
  sendAlertNotification ($alert, %action);
}

###########################################
# Handling of cleared recurring incident
#
# This will be fired when a recurring incident
# has been inactive for the hold-down period
# (4 hours)
###########################################

sub clearRecurringIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
  
  # Design email content
  my $alert;
  if (%action->{highest_state} >= 2)
  { $alert = "Cleared Recurring: " . incidentAlertText(%action); }
  else
  { $alert = ""; }

  # Send Alert
  sendAlertNotification ($alert, %action);
}

###########################################
# Standard Functions
###########################################

sub incidentAlertText
{
  my (%action) = @_;

  my $subject = %action->{device};
  $subject .= " " . %action->{container};
  if (%action->{object} ne "Master") { $subject .= " " . %action->{object}; }
  $subject .= " " . %action->{metric};
  $subject .= " " . %action->{trigger};
  $subject .= " at " . %action->{site};

  return $subject;
}

sub sendAlertNotification
{
  my ($alert, %action) = @_;

  # Send push notification to LithiumCorp APNS dispatch
  my $userAgent = LWP::UserAgent->new();
  my $request = HTTP::Request->new(POST => 'http://secure.lithiumcorp.com.au/pud/dispatch.php?uuid=' . %action->{customer_uuid});
  $request->content("badge=" . %action->{incident_count} . "&url=" . %action->{incident_url} . "&alert=" . $alert);
  $request->content_type("application/x-www-form-urlencoded");
  my $response = $userAgent->request($request);
  if($response->code == 200) 
  {
    print $response->as_string;
  }
  else 
  {
    print $response->error_as_HTML;
  }
}

sub generateConfig 
{
  # Create the XML Config file. 
  # You don't have to make changes to this sub.
  # To make changes to this XML Structure:
  #   Modify %xmlstruct at the begining of this file

  # Parameter 1: Temporary XML File
  my ($xmlrool) = $_[0];
  my (%struct) = %{$_[1]};
  
  my $xml = new XML::Simple(NoAttr=>1, RootName=>"action_script",XMLDecl=>1);
  $data = $xml->XMLout(\%struct);
  return "$data";
}

sub getVariables 
{
  # Parameter: Filename for the XML
  my ($filename) = @_;

  # ------>Collect XML Info Here <----------
  my $xml = new XML::Simple; # create new XML object
  my $data = $xml->XMLin($filename);
  my %variables =%{$data->{variable}};
  return %variables;
}

sub getAction
{
  my %action = ( incident_id => $ARGV[1],
                 entity_address => $ARGV[2],
                 state => $ARGV[3],
                 customer => $ARGV[4],
                 site => $ARGV[5],
                 device => $ARGV[6],
                 container => $ARGV[7],
                 object => $ARGV[8],
                 metric => $ARGV[9],
                 trigger => $ARGV[10],
                 run_count => $ARGV[11],
                 start_time => $ARGV[12],
                 end_time => $ARGV[13],
                 highest_state => $ARGV[14],
                 lowest_state => $ARGV[15],
                 previous_state => $ARGV[16],
                 previous_trigger => $ARGV[17],
                 last_transition => $ARGV[18],
                 last_occurrence => $ARGV[19],
                 occurrences => $ARGV[20],
                 entity_url => $ARGV[21],
                 metric_url => $ARGV[22],
                 incident_count => $ARGV[23],
                 incident_url => $ARGV[24],
                 customer_uuid => $ARGV[25],
               );
  return %action;
}
