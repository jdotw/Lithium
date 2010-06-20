<?php

include ($_SERVER["DOCUMENT_ROOT"] . "/include/standard.php");

standard_start(NULL, NULL);

if(sizeof($_POST) > 0)
{
	if(!(
		(is_numeric($_POST['IP-0']) && is_numeric($_POST['IP-1']) && is_numeric($_POST['IP-2']) && is_numeric($_POST['IP-3'])) &&
		(is_numeric($_POST['Netmask-0']) && is_numeric($_POST['Netmask-1']) && is_numeric($_POST['Netmask-2']) && is_numeric($_POST['Netmask-3'])) &&
                (is_numeric($_POST['Gateway-0']) && is_numeric($_POST['Gateway-1']) && is_numeric($_POST['Gateway-2']) && is_numeric($_POST['Gateway-3']))))
	{
		die("Invalid input in IP Address details. Expected a number but got something else! Please return and check data.");
	}
	else
	{

	
	
	$int = file('/etc/network/interfaces.testing') or die("Can't open Interfaces");

	echo "<span style=\"color: green;\">BEFORE</span> <pre>";

	echo $int[9];
	echo $int[10];
	echo $int[11];

	$int[9] = "\taddress " . $_POST['IP-0'] . "." . $_POST['IP-1'] . "." . $_POST['IP-2'] . "." . $_POST['IP-3'] . "\n";
	$int[10] = "\tnetmask " . $_POST['Netmask-1'] . "." . $_POST['Netmask-1'] . "." . $_POST['Netmask-2'] . "." . $_POST['Netmask-3'] . "\n";
	$int[11] = "\tgateway " . $_POST['Gateway-0'] . "." . $_POST['Gateway-1'] . "." . $_POST['Gateway-2'] . "." . $_POST['Gateway-3'] . "\n";

	echo "after\n";

	echo $int[9];
	echo $int[10];
	echo $int[11];

	$f = file_put_contents('/etc/network/interfaces.testing', $int) or die("Can't write to file.");

	}
}
else
{

$int = file('/etc/network/interfaces.testing') or die("Can't open Interfaces ro");

$numbers['IP'] = explode('.', str_replace('address ', '', $int[9]));
$numbers['Netmask'] = explode('.', str_replace('netmask ', '', $int[10]));
$numbers['Gateway'] = explode('.', str_replace('gateway ', '', $int[11]));

$iface = explode(' ', $int[8]);

echo "<p>Settings for network interface <b>$iface[1]</b></p>";

echo "<form action=\"$_SERVER[PHP_SELF]\" name=\"etc_network_interfaces\" method=\"POST\"><table>";

foreach($numbers as $k => $v)
{

$v[0] = trim($v[0]); $v[1] = trim($v[1]); $v[2] = trim($v[2]); $v[3] = trim($v[3]);

print <<<EndOfHTML

<tr><td>

$k</td><td>
	<input type="text" size="2" value="$v[0]" name="$k-0">.
	<input type="text" size="2" value="$v[1]" name="$k-1">.
	<input type="text" size="2" value="$v[2]" name="$k-2">.
	<input type="text" size="2" value="$v[3]" name="$k-3">
</td></tr>

EndOfHTML;
}

echo "</table><input type=\"submit\"></form>";

}

standard_end();

?>
