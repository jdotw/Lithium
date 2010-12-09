<?php

session_start();

include_once("functions.php");
include($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "/include/defaults.php");

if($_POST['op'] == 'add')
{
  cf_getdb('lithium');

  $names = pg_query("SELECT name from customers");

  while($r = pg_fetch_row($names))
  {
    $existingids[] = $r[0];
  }
 
  if(sizeof($existingids) > 0)
  {
    foreach($_POST['custid'] as $v)
    {
      if(in_array($v, $existingids))
        lithium_error("Duplicate Customer ID $v found.", "0x00009004");
    }
  }

  admin_start();

  $vhost = (substr($_POST['vhostname'], -1) == '/') ? substr($_POST['vhostname'], 0, -1) : $_POST['vhostname'];

  foreach($_SESSION['custname'] as $k => $v)
  {
    echo "Copying system files for customer $v... ";

    if(cf_profilecreate_msp($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . "/default", $_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . $_POST['custid'][$k], $_POST['custid'][$k], $v, TRUE))
      echo "<span style=\"color: green;\">done.</span><br />";
    else
      echo "<span style=\"color: red;\">failed.</span><br />";

    echo "Adding database entry for customer $v... ";

    if(db_custreplace($_POST['custid'][$k], $v, $vhost . "/" . $_POST['custid'][$k]))
      echo "<span style=\"color: green;\">done.</span><br />";
    else
      echo "<span style=\"color: red;\">failed.</span><br />";

    echo "<br />";
  }

  echo "Click <a href=/default/admin/index.php>here</a> to continue.";

  admin_end();

  exit;
}

$basedir = $_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir];

foreach($_SESSION['originals'] as $k => $v)
{
  if(($v[0] != $_POST['custid'][$k] || $v[1] != $_POST['custname'][$k]) ||
      ($_POST['base'] != $_POST['oldbase']) ||
      is_array($_POST['delete']))
    $change = true;
}

if($change)
{
  $db = cf_getdb('lithium');

  $base = $_POST['base'];

  if(substr($base, -1, 1) != '/')
    $base .= '/';

  if(substr($base, 0, 7) != 'http://')
    $base = 'http://' . $base;

  foreach($_POST['custid'] as $k => $v)
  {
    $o = $_SESSION['originals'][$k][0]; // presented as [0] => name, [1] => descr
    $b = $base . $v;
    $d = $_POST['custname'][$k];

    if($_POST['delete'][$k] == 'on')
    {
      $q = pg_query("DELETE FROM customers WHERE name='$o'");
      system("rm -r " . $basedir . "/$o", $return);

      if($return > 0)
	lithium_error("Unable to remove directory for customer $o.", "0x00009009");
    }
    else
    {
      $q = pg_query("UPDATE customers SET name='$v', descr='$d', baseurl='$b' WHERE name='$o'");

      $origdir = $basedir . "/" . $o; $newdir = $basedir . "/" . $v;

      cf_profilecreate_msp($basedir . "/default", $newdir, $v, $d, FALSE, $o);
    }
  }
}

header("Location: index.php?success=modify");

unset($_SESSION['originals']);

?>
