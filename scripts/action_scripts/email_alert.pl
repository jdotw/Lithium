#! /usr/bin/perl 
use XML::Simple; 
use Net::SMTP; 
use Data::Dumper; 
use Time::CTime; 
use Net::IRC;

###################################
#  Script Configuration Variables
###################################

%xmlstruct = (  'desc' => "Generic SMTP Email Notification Script",
                'version' => '1.0.2',
                'config_variable'=> [
                              {
                                'name' => 'sender',
                                'desc'=>'Senders Email Address'
                              },
                              {
                                'name' => 'mailhost',
                                'required' => '1',
                                'desc' => 'SMTP Server'
                              },
                              {
                                 'name' => 'replyto',
                                 'desc' => 'ReplyTo: Address'
                              },
                              {
                                 'name' => 'recipients',
                                 'required' => '1',
                                 'desc' => 'Recipients (comma-separated)'
                              }
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
###################################

sub reportIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
  
  # Extract Variables from config XML and store in %variables Hash
  %variables = &getVariables($ARGV[$#ARGV]);
   
  # Design email subject
  my $subject = "New Incident: " . incidentSubjectText(%action);

  # Design email text content
  my $textContent = incidentContentHeaderText(%action);
  $textContent .= "\n";
  $textContent .= incidentStatementText(%action);
  $textContent .= "\n";
  $textContent .= "Incident Start Timestamp: " . ctime(%action->{start_time});
  $textContent .= "\n";
  $textContent .= incidentDetailText(%action);
  $textContent .= "\n";
  $textContent .= incidentContentFooterText(%action);

  # Send email
  sendEmail ($subject, $textContent);
}


##########################################
# Handling of a recurring incident report
##########################################

sub reportRecurringIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
  
  # Extract Variables from XML and store in %variables Hash
  %variables = &getVariables($ARGV[$#ARGV]);
   
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
    my $subject = "Recurring Incident: " . incidentSubjectText(%action);
    my $textContent = incidentContentHeaderText(%action);
    $textContent .= "\n";
    $textContent .= incidentStatementText(%action);
    $textContent .= "\n";
    $textContent .= "Original Incident Timestamp: " . ctime(%action->{start_time});
    $textContent .= "Current Incident Timestamp: " . ctime(%action->{last_occurrence});
    $textContent .= "\n";
    $textContent .= "NOTE: No further reports for this incident will be sent unless the incident transitions to a new highest or lowest severity. The Recurring Incident Cleared notification will be sent when the Incident had remained inactive for 4-hours.\n";
    $textContent .= "\n";
    $textContent .= incidentDetailText(%action);
    $textContent .= "\n";
    $textContent .= incidentContentFooterText(%action);

    # Send email
    sendEmail ($subject, $textContent);
  }
}

###################################
# Handling of an incident transition
#
# Only sends an escalation or downgrade
# notice if the current state is higher
# or lower than the highest/lowest opstate 
# seen in the life of the incident. 
###################################

sub transitionIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
  
  # Extract Variables from XML and store in %variables Hash
  %variables = &getVariables($ARGV[$#ARGV]);

  # Design email content
  my $subject;
  if (%action->{state} gt %action->{highest_state}) 
  { $subject = "Incident Escalation: " . incidentSubjectText(%action); }
  elsif (%action->{state} lt %action->{lowest_state}) 
  { $subject = "Incident Downgrade: " . incidentSubjectText(%action); }
  else { return; }
  my $textContent = incidentContentHeaderText(%action);
  $textContent .= "\n";
  $textContent .= incidentStatementText(%action);
  $textContent .= "\n";
  $textContent .= "Original Incident Timestamp: " . ctime(%action->{start_time});
  $textContent .= "State Transition Timestamp: " . ctime(%action->{last_transition});
  $textContent .= "\n";
  $textContent .= "NOTE: No further reports for this incident will be sent unless the incident transitions to a new highest or lowest severity.\n";
  $textContent .= "\n";
  $textContent .= incidentDetailText(%action);
  $textContent .= "\n";
  $textContent .= incidentContentFooterText(%action);

  # Send email
  sendEmail ($subject, $textContent);
}

############################################
# Handling of an incident reminder (re-run)
############################################

sub remindIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
  
  # Extract Variables from XML and store in %variables Hash
  %variables = &getVariables($ARGV[$#ARGV]);
   
  # Design email content
  my $subject = "Reminder: " . incidentSubjectText(%action);

  # Design email text content
  my $textContent = incidentContentHeaderText(%action);
  $textContent .= "\n";
  $textContent .= incidentStatementText(%action);
  $textContent .= "\n";
  $textContent .= "Original Incident Timestamp: " . ctime(%action->{start_time});
  $textContent .= "This is an automated reminder of an existing Incident.\n\n";
  $textContent .= "\n";
  $textContent .= incidentDetailText(%action);
  $textContent .= "\n";
  $textContent .= incidentContentFooterText(%action);

  # Send email
  sendEmail ($subject, $textContent);
}

###########################################
# Handling of incident cleared
###########################################

sub clearIncident
{
  # Retrieve action from ARGV 
  %action = &getAction();
  
  # Extract Variables from XML and store in %variables Hash
  %variables = &getVariables($ARGV[$#ARGV]);
   
  # Design email content
  my $subject = "Cleared: " . incidentSubjectText(%action);

  # Design email text content
  my $textContent = incidentContentHeaderText(%action);
  $textContent .= "\n";
  $textContent .= incidentStatementText(%action);
  $textContent .= "\n";
  $textContent .= "Incident Start Timestamp: " . ctime(%action->{start_time});
  $textContent .= "Incident End Timestamp: " . ctime(%action->{end_time});
  $textContent .= "\n";
  $textContent .= incidentDetailText(%action);
  $textContent .= "\n";
  $textContent .= incidentContentFooterText(%action);

  # Send email
  sendEmail ($subject, $textContent);
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
  
  # Extract Variables from XML and store in %variables Hash
  %variables = &getVariables($ARGV[$#ARGV]);
   
  # Design email content
  my $subject = "Cleared Recurring: " . incidentSubjectText(%action);

  # Design email text content
  my $textContent = incidentContentHeaderText(%action);
  $textContent .= "\n";
  $textContent .= incidentStatementText(%action);
  $textContent .= "\n";
  $textContent .= "Incident Start Timestamp: " . ctime(%action->{start_time});
  $textContent .= "Incident End Timestamp: " . ctime(%action->{end_time});
  $textContent .= "Last Occurrence Timestamp: " . ctime(%action->{last_occurrence});
  $textContent .= "\n";
  $textContent .= "This Recurring Incident has remained inactive for 4-hours and hence is now cleared.\n";
  $textContent .= "\n";
  $textContent .= incidentDetailText(%action);
  $textContent .= "\n";
  $textContent .= incidentContentFooterText(%action);

  # Send email
  sendEmail ($subject, $textContent);
}

###########################################
# Standard Functions
###########################################

sub incidentStatementText
{
  my (%action) = @_;

  my $content;

  $content .= "The value of the " . %action->{metric};
  $content .= " metric for " . %action->{container} . " ";
  $content .= %action->{object} . " on device " . %action->{device} . " at " . %action->{site};

  if (%action->{state} eq "0") 
  { $content .= " is no longer within the " . %action->{trigger} . " trigger condition.\n"; }
  else 
  { $content .= " has reached or exceeded the " . %action->{trigger} . " trigger condition.\n"; }

  $content .= "\n";

  $content .= "URL: " . %action->{metric_url} . "\n";

  return $content;
}

sub incidentDetailText
{
  my (%action) = @_;

  my $content;

  if (%action->{state} eq "0") { $content .= "Current State: Normal\n" };
  if (%action->{state} eq "1") { $content .= "Current State: Warning\n" };
  if (%action->{state} eq "2") { $content .= "Current State: Impaired\n" };
  if (%action->{state} eq "3") { $content .= "Current State: Critical\n" };
  
  $content .= "\n";

  $content .= "Incident ID:   " . %action->{incident_id} . "\n";
  $content .= "Site:          " . %action->{site} . "\n";
  $content .= "Device:        " . %action->{device} . "\n";
  $content .= "Container:     " . %action->{container} . "\n";
  $content .= "Object:        " . %action->{object} . "\n";
  $content .= "Metric:        " . %action->{metric} . "\n";
  $content .= "Trigger:       " . %action->{trigger} . "\n";

  return $content;
}

sub incidentSubjectText
{
  my (%action) = @_;

  my $subject = " " . %action->{device};
  $subject .= " " . %action->{container};
  $subject .= " " . %action->{object};
  $subject .= " " . %action->{metric};
  $subject .= " " . %action->{trigger};
  $subject .= " at " . %action->{site};

  return $subject;
}

sub incidentContentHeaderText
{
  my (%action) = @_;
  return $content;
}

sub incidentContentFooterText
{
  my (%action) = @_;
  my $content = "";
  my $content = "This is an automated incident report from the " . %action->{customer} . " Lithium deployment.\n";
  $content .= "\n--\n\n";
  $content .= "Lithium 5.0 - LithiumCorp - http://lithium5.com\n";
}

sub sendEmail ()
{
  %variables = &getVariables($ARGV[$#ARGV]);
  my ($subject) = $_[0];
  my ($textContent) = $_[1];

  # Set variables from XML
  my @recipients = split (/[,;]/, %variables->{recipients}->{value});
  my $replyto = %variables->{replyto}->{value};
  my $mailhost = %variables->{mailhost}->{value};
  my $sender = %variables->{sender}->{value};

  # Send email using SMTP
  $smtp = Net::SMTP->new("$mailhost");
  $smtp->mail("$sender");
  $smtp->recipient(@recipients, { SkipBad => 1 });
  $smtp->data();
  $smtp->datasend("From: $sender\n");
  $smtp->datasend("To: " . %variables->{recipients}->{value} . "\n");
  $smtp->datasend("Subject: $subject\n");
  $smtp->datasend("\n"); 
  $smtp->datasend("$textContent\n");
  $smtp->dataend();
  $smtp->quit;
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
               );
  return %action;
}
