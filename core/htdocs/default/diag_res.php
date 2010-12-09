<?php
  
  include ("profile.php");
  include ("../include/standard.php");

  /* Get/Check username and password */
  authenticate ();

  /* Get Resource List */
  $xml_output = a_xml_get ("::0:0:", "", "resource_list", "0", $ret_val);

  print $xml_output;
?>

                
