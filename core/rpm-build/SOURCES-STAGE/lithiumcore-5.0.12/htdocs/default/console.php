<?php

  include ("profile.php");
  include ("../include/defaults.php");

  $f = file($default_config_path . "/node.conf") or die("Can't open config file for reading.");
  $cluster = trim(str_replace(array("plexus ", '"'), "", $f[1]));
  $node = trim(str_replace(array("node ", '"'), "", $f[2]));
  $baseurl = "http://" . $_SERVER['SERVER_NAME'] . substr($_SERVER['REQUEST_URI'], 0, strrpos($_SERVER['REQUEST_URI'], '/') + 1);
  
  print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  print "<customers>";
  print "<customer>";
  print "<name>$customer_id_str</name>";
  print "<baseurl>$baseurl</baseurl>";
  print "<cluster>$cluster</cluster>";
  print "<node>$node</node>";
  print "</customer>";
  print "</customers>";

?>
