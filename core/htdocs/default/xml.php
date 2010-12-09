<?php

include ("profile.php");
include ("../include/standard.php");

/* Get/Check username and password */
$username = authenticate ();

/*
 * Sanity checking
 */

/* Make sure our script.php exists (As of 4.8.6) */
$ourpath = substr($_SERVER['SCRIPT_FILENAME'], 0, strrpos($_SERVER['SCRIPT_FILENAME'], '/'));
$lithiumpath = substr($ourpath, 0, strrpos($ourpath, '/'));
if (!file_exists($ourpath . "/script.php")) 
{ @symlink ($lithiumpath . "/default/script.php", $ourpath . "/script.php"); }

/* Make sure our diag path exists (As of 4.9.0) */
$ourpath = substr($_SERVER['SCRIPT_FILENAME'], 0, strrpos($_SERVER['SCRIPT_FILENAME'], '/'));
$lithiumpath = substr($ourpath, 0, strrpos($ourpath, '/'));
if (!file_exists($ourpath . "/diag_res.php")) 
{ @symlink ($lithiumpath . "/default/diag_res.php", $ourpath . "/diag_res.php"); }
$ourpath = substr($_SERVER['SCRIPT_FILENAME'], 0, strrpos($_SERVER['SCRIPT_FILENAME'], '/'));
$lithiumpath = substr($ourpath, 0, strrpos($ourpath, '/'));
if (!file_exists($ourpath . "/diag_auth.php")) 
{ @symlink ($lithiumpath . "/default/diag_auth.php", $ourpath . "/diag_auth.php"); }

/* End Sanity Checking */

/*
 * Action handling 
 */

switch ($action)
{
  case "xml_get":
    $xml_output = a_xml_get ($_GET['resaddr'], $_GET['entaddr'], $_GET['xmlname'], $_GET['refsec'], $ret_val);
    break;
}

/* 
 * Error Checking 
 */

/* Check for success */
if ($ret_val == 0)
{
  /* Render output */
  print $xml_output;
  exit;
}
 
/* Check for auth-failure */
if ($ret_val == 15 || $ret_val == 1)
{
  /* Re-prompt for auth */
  authenticate_headers ();
  exit;
}

/* XML Error handling */
if ($ret_val != 0)
{ 
  /* Non-Auth Error: Send XML header */
  print "<?xml version=\"1.0\" encoding=\"UTF-8\"?><error>";

  /* Set defaults */
  $error = "unknown";
  $message = "Unknown Error Occurred";

  switch ($ret_val)
  {
    case 2: /* LEGACY */
      /* Connection failure */
      print "Connection to Lithium Server failed. The server may be offline for maintenance.";
      break;
    case 10: /* ERROR_BADARGS */
      $error = "ERROR_BADARGS";
      $message = "Incorrect or insufficient arguments passed to actuator";
      break;
    case 11: /* ERROR_NOCONN */
      $error = "ERROR_NOCONN";
      $message = "Failed to connect to Lithium Core";
      break;
    case 12: /* ERROR_SENDFAILED */
      $error = "ERROR_SENDFAILED";
      $message = "Failed to send request to Lithium Core";
      break;
    case 13: /* ERROR_INTERNAL */
      $error = "ERROR_INTERNAL";
      $message = "Unexpected internal error occurred";
      break;
    case 14: /* ERROR_BADADDRESS */
      $error = "ERROR_BADADDRESS";
      $message = "Malformed resource or entity address";
      break;
    case 16: /* ERROR_AUTHTIMEOUT */
      $error = "ERROR_AUTHTIMEOUT";
      $message = "Authentication timeout occurred";
      break;
    case 17: /* ERROR_AUTHERROR */
      $error = "ERROR_AUTHERROR";
      $message = "Error occurred during authentication process";
      break;
    case 18: /* ERROR_OPERROR */
      $error = "ERROR_OPERROR";
      $message = "XML operation failed";
      break;
    case 19: /* ERROR_ROUTEFAILED */
      $error = "ERROR_ROUTEFAILED";
      $message = "Failed to route message to destination resource";
      break;
    case 20: /* ERROR_EXIST */
      $error = "ERROR_EXIST";
      $message = "Requested document/data does not exist or is not supported";
      break;
  }

  print "<error>$error</error>";
  print "<message>$message</message>";
  print "</error>";

  exit;
}

?>
