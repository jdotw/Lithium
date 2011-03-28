<?php

  include ("include/defaults.php");
  include_once("default/admin/functions.php");

  $f = file($default_config_path . "/node.conf") or die("Can't open config file for reading.");
  $cluster = trim(str_replace(array("plexus ", '"'), "", $f[1]));
  $node = trim(str_replace(array("node ", '"'), "", $f[2]));

  print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  print "<customers>";

  cf_getdb('lithium');
  $customers = pg_query("SELECT descr, name, uuid from customers ORDER BY name ASC");
  $uripath = substr($_SERVER['REQUEST_URI'], 0, strrpos($_SERVER['REQUEST_URI'], '/'));

  while($r = pg_fetch_row($customers))
  {
    /* Process each customer */
    print "<customer>";
    print "<name>" . $r[1] . "</name>";
    print "<desc>" . $r[0] . "</desc>";
    if ($_SERVER['HTTPS'] == "on")
    { $protocol = "https://"; }
    else
    { $protocol = "http://"; }
    $baseurl = $protocol . $_SERVER['SERVER_NAME'] . ":" . $_SERVER['SERVER_PORT'] . $uripath . "/" . $r[1];
    print "<uuid>".$r[2]."</uuid>";
    print "<baseurl>$baseurl</baseurl>";
    print "<cluster>$cluster</cluster>";
    print "<node>$node</node>";
    print "</customer>";
  }
  print "</customers>";

?>

