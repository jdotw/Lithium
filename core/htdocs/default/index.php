<?php

/* Determine Lithium root and subdir */
$subcust = substr($_SERVER['REQUEST_URI'], 0, strrpos($_SERVER['REQUEST_URI'], '/') + 1);
$lithroot = substr($subcust, 0, strpos($subcust, '/', 1));

/* Check to make sure this deployment is setup */
if(!file_exists("profile.php"))
{
  header("Location: " . $_SERVER['REQUEST_URI'] . "admin/setup.php?step=1\n\n");
  exit;
}

include ("profile.php");
include ("../include/standard.php");

/* Get/Check username/password is supplied */
$username = authenticate ();

/* Get customer resource address */
$custaddr = customer_resaddr ($customer_id_str);

/* Setup variables */
if ($action == "form_get") { $menu_res = $resaddr; $menu_ent = $entaddr; }              /* Form Get */
if ($action == "form_submit") { $menu_res = $dstresaddr; $menu_ent = $dstentaddr; }     /* Form Submit */
if (!isset($menu_res)) { $menu_res = $custaddr; $menu_ent = NULL; }                     /* Default to customer resource */

/* Handle Actions */
switch ($action)
{
  case "form_get": 
    $formget_start = time ();
    $form_output = a_form_get ($resaddr, $entaddr, $formname, $refsec, $passdata, $ret_val);
    $formget_end = time ();
    $formget_time = $formget_end - $formget_start;
    break;
  case "form_submit":
    $form_output = a_form_submit ($srcresaddr, $srcresent, $srcform, $srcrefsec, $srcpassdata, $dstresaddr, $dstentaddr, $dstform, $dstrefsec, $dstpassdata, $_POST, $ret_val);
    break;
  default:
    if ($custaddr) $form_output = a_form_get ($custaddr, NULL, "main", 0, "", $ret_val);
}

/*
 * Error Checkibng
 */

/* Check for success */
if ($ret_val == 0)
{
  /* Render page */
  standard_start ($menu_res, $menu_ent);
  print $form_output;
  standard_end ();
  exit;
}

/* Check for auth failure */
if ($ret_val == 15 || $ret_val == 1)
{
  /* Re-prompt for auth */
  authenticate_headers ();
  exit;
}

/* Main error handling */
if ($ret_val != 0)
{
  switch ($ret_val)
  {
    case 2: /* LEGACY */
      /* Legacy Conn failure */
      print "ERROR: Connection to Lithium Server failed -- LEGACY ERROR. Please report this to support@lithiumcorp.com";
      break;
    case 10: /* ERROR_BADARGS */
      print "ERROR: Incorrect or insufficient parameters passed to actuator<BR>";
      print "Please report this to support@lithiumcorp.com<BR>";
      break;
    case 11: /* ERROR_NOCONN */
      print "ERROR: Failed to connect to Lithium Core<BR>";
      print "Please check to see that Lithium Core is running.<BR>";
      break;
    case 12: /* ERROR_SENDFAILED */
      print "ERROR: Failed send request to Lithium Core<BR>";
      print "Please check to see that Lithium Core is running.<BR>";
      break;
    case 13: /* ERROR_INTERNAL */
      print "ERROR: Unexpected internal error occurred<BR>";
      print "Please report this to support@lithiumcorp.com.<BR>";
      break;
    case 14: /* ERROR_BADADDRESS */
      print "ERROR: Malformed entity or resource address in request<BR>";
      print "Please report this to support@lithiumcorp.com.<BR>";
      break;
    case 16: /* ERROR_AUTHTIMEOUT */
      print "ERROR: Timeout occurred in authentication<BR>";
      print "The server may be overloaded or busy. Please try again shortly.<BR>";
      break;
    case 17: /* ERROR_AUTHERROR */
      print "ERROR: Non-Timeout error occurred in authentication<BR>";
      print "The server may be overloaded or busy. Please try again shortly.<BR>";
      break;
    case 18: /* ERROR_OPERROR */
      print "ERROR: The requested operation failed<BR>";
      print "Please report this to support@lithiumcorp.com.<BR>";
      break;
    case 19: /* ERROR_ROUTEFAILED */
      print "ERROR: Failed to route request to destination module/process<BR>";
      print "The server may be overloaded or busy.<BR>";
      print "If you have recently re-started Lithium the requested process may not be started yet.<BR>";
      print "Please try again shortly.<BR>";
      break;
    case 20: /* ERROR_EXIST */
      print "ERROR: The requested data or document does not exist or is not supported<BR>";
      print "Please check to see your version of Lithium Core is up to date.<BR>";
      break;
  }
  
  print "<BR>Visit <a href=http://support.lithium5.com>http://support.lithium5.com</a> for assistance.<BR>";
}
    
?>
