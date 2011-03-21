<?php

include ("profile.php");
include ("../include/standard.php");

/* Get/Check username and password */
authenticate ();

$xml_output = a_xml_get ("::5:0:$customer_id_str", "1:$customer_id_str", "entity_tree", "", $ret_val);

/* 
 * Error Checking 
 */

/* Check for success */
if ($ret_val == 0)
{
  /* Render output */
  print "OK";
  exit;
}
 
/* Check for auth-failure */
if ($ret_val == 15 || $ret_val == 1)
{
  print "FAILED";
  exit;
}

/* XML Error handling */
if ($ret_val != 0)
{
  print "ERROR";
  exit;
}

?>
