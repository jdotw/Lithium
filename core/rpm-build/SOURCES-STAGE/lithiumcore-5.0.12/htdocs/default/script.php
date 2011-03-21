<?php

include ("profile.php");
include ("../include/standard.php");
include ("../include/script_upload.php");
include ("../include/script_download.php");

/* Get/Check username and password */
$username = authenticate ();

/* Handle actions */
switch ($action)
{
  case "upload": 
    $xml_output = a_script_upload ($_GET['resaddr'], $_GET['entaddr'], $_GET['type'], $_GET['name'], $ret_val);
    break;
  case "download": 
    $xml_output = a_script_download ($_GET['resaddr'], $_GET['entaddr'], $_GET['type'], $_GET['name'], $ret_val);
    break;
}

/* Check result */
switch ($ret_val)
{
  case 1:
    /* Auth failure, resend auth headers */
    authenticate_headers ();
    exit;
  case 2:
    /* Connection failure */
    print "Connection to Lithium Server failed. The server may be offline for maintenance.";
    exit;
}

/* Render output */
print $xml_output;

?>
