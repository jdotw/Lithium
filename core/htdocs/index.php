<?php

session_start();

if(file_exists("default/profile.php"))
{
  include_once("default/admin/functions.php");

  admin_start();

  echo "<h2>Configured customers:</h2>";

  cf_getdb('lithium');

  $customers = pg_query("SELECT descr, name from customers");

  $uripath = substr($_SERVER['REQUEST_URI'], 0, strrpos($_SERVER['REQUEST_URI'], '/') + 1);
  
  while($r = pg_fetch_row($customers))
  {
    echo "<h3><a href=\"" . $_SERVER['REQUEST_URI'] . $r[1] . "\">" . $r[0] . "</a></h3>";
  }

  admin_end();
}
else
{
  header("Location: " . $uripath . "default/");
  exit;
}

?>
