<?php

session_start();

$myloc = $_SERVER[DOCUMENT_ROOT] . $_SESSION['subdir'];

if(file_exists("../profile.php"))
  include ("../profile.php");
include($myloc . "include/standard.php");
include_once("functions.php");

global $customer_id_str;

$custdir = $customer_id_str;

admin_start();

$ls = `ls $_SERVER[DOCUMENT_ROOT]$_SESSION[subdir]`;

print <<<EndOfHTML

<script language=javascript src=customers.js>
</script>

EndOfHTML;
		

$item = explode("\n", $ls);
foreach($item as $it)
{
	if(is_link($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . $it))
	{
		if(file_exists($_SERVER[DOCUMENT_ROOT] . $_SESSION[subdir] . $it . "/profile.php"))
		{
			if($it != 'lcorp' && $it != 'admin')
				$custdir = $it;
		}
	}
}

if((empty($custdir) && $_GET['step'] == 1) || $_GET['op'] == 'add')
{
  if($_GET['step'] == 1)
  {
    print <<<EndOfHTML

<h2>Configuration: Step 1</h2>

Welcome to the Lithium Core configuration script.
<p>
Please enter a text description in the <span style="color: blue; font-weight: bold;">Customer Description</span> field for each customer to be added. <br />
</p>
<br />
<div id=greybox>
<form name="details" action="setup.php?step=2" method="post"  onSubmit="javascript:return ValidateForm(this);">

EndOfHTML;
  }
  else
  {
    if($_GET['task'] == 'suggest')
    {
      $_SESSION['custname'] = $_POST['custname'];

      echo "<form name=details action=writecustomers.php method=post onSubmit=\"javascript:return ValidateForm(this);\">\n";
      echo "<h2>Add new customers</h2>\n";
      echo "<input type=hidden name=op value=add>\n";

      $second = true;

      foreach($_POST['custname'] as $v)
      {
        $id = safeCustomerName($v);

        echo "<input type=text size=20 name=custname[] value=\"$v\" disabled=true> <input type=text size=20 name=custid[] value=\"$id\" onKeyPress=\"javascript:return keyStop(event);\">&nbsp; (e.g \"LithiumCorp\")<br />\n";
      }

      $base = $_POST['base'];

      if($base == '')
        $base = $_SERVER[SERVER_NAME];

      if(substr($base, -1, 1) != '/')
        $base .= '/';

      if(substr($base, 0, 7) != 'http://')
        $base = 'http://' . $base;


      print <<<EndOfHTML
      <br />
      Base URL:
      <br />
      <input type=text size=20 disabled=true value=$base>
      <input type=hidden name=vhostname value=$base>
      </div>
EndOfHTML;

      echo "<br /><input type=submit value=\"Add new customer(s)\">";

      admin_end();

      exit;
    }
    else
    {
      echo "<h2>Add new customers</h2>\n";
      echo "<form name=details action=$_SERVER[REQUEST_URI]&task=suggest method=post onSubmit=\"javascript:return ValidateForm(this);\">\n";
      echo "<input type=hidden name=op value=add>\n";
    }
  }

  print <<<EndOfHTML

<input type="hidden" name="new" value="yes">
<input type="hidden" value="1" id="startMarker">
<span style="position: relative; left: 10px;"><b>Customer Description:</b></span>
<br /><br />
1.<span style="position: relative; left: 10px;"><input name="custname[]" size="40" id=name0>&nbsp;(e.g "LithiumCorp")</span>
<br /><br />
<div id="inputTableStart">
</div>
<br />
<input type="submit" value="Continue">
</form>
<br />
<a href="javascript:;" onClick="addRow();">Add more customers (CoreMSP only)</a>

EndOfHTML;

}
else
{
  $db = cf_getdb('lithium');

  $customers = pg_query("SELECT * FROM customers");

  echo "<h3 class=\"header\"><a href=\"" . $_SESSION['subdir'] . "default/admin/index.php\">Return to menu</a></h3>\n\n";

if(pg_num_rows($customers) > 0)
{

  print <<<EndOfHTML

<h2>Customer information</h2>

<form name="details" action="writecustomers.php" method="post">

<table width="500" cellpadding="2" cellspacing="0">

<tr>
  <td>
    &nbsp;
  </td>
  <td width="250">
      Customer ID
  </td>
  <td width="250">
      Customer Description
  </td>
  <td>
    Delete?
  </td>
</tr>
EndOfHTML;

}

  $counter = 1; $index = 0;
  unset($_SESSION['originals']);

  while($cust = pg_fetch_row($customers))
  {
    $_SESSION['originals'][] = array($cust[0], $cust[1]);

    print <<<EndOfHTML
<tr>
  <td>
    $counter.
  </td>
  <td>
    <input type=text name=custid[$index] value=$cust[0] size=20 onKeyPress="javascript:return keyStop(event);">
  </td>
  <td>
    <input type=text name=custname[$index] value="$cust[1]" size=20>
  </td>
  <td style="text-align: center;">
    <input type=checkbox name=delete[$index]>
  </td>
</tr>

EndOfHTML;

    $counter++;
    $index++;

    $furl = $cust[2]; $uri = "/" . $cust[0];
  }

  if($counter > 1)
  {
    $base = str_replace($uri, '', $furl);

    if($base == '')
      $base = $_SERVER[SERVER_NAME];

    if(substr($base, -1, 1) != '/')
      $base .= '/';

    if(substr($base, 0, 7) != 'http://')
      $base = 'http://' . $base;

    print <<<EndOfHTML

</table>
<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Base URL:
<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type=text name=base size=20 value=$base>
<input type=hidden name=oldbase size=20 value=$base>
<br /><br />
<input type="Submit" value="Apply">
</form>

<br />

EndOfHTML;

  }
  else
    echo "<h3>No customers defined.</h3>";

  echo "<a href=customers.php?op=add>Add new customers</a>";

  admin_end();
}

?>
