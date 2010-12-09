<?php

@session_start();

include("../profile.php");
include($_SERVER[DOCUMENT_ROOT] . $_SESSION['subdir'] . "include/standard.php");
include_once("functions.php");

admin_start();

global $customer_id_str;

echo "<h3 class=\"header\"><a href=\"" . $_SESSION['subdir'] . "default/admin/index.php\">Return to menu</a></h3>\n\n";

echo "<link rel=stylesheet type=text/css href=admin.css>";

$ps = shell_exec('ps auxww');
$lines = explode("\n", $ps);

echo "<div id=\"statuspage\">";

/* Lithium Core processes */

echo "<div id=\"lithium\">";

echo "<h2>LITHIUM|Core Processes</h2>\n";

echo "<table>\n";
echo "<tr class=\"psHeader\"><th>PID</th><th>CPU %</th><th>Memory %</th><th>Started at:</th><th>CPU Time</th><th>Command</th></tr>\n";
for($i = 1; $i < sizeof($lines)-1; $i++)
{
	$w = preg_split("/[\s]+/", $lines[$i]);

	if($w[0] == 'root' && (strstr($w[10], 'lithium') !== FALSE))
	{
		echo "<tr style=\"background: white; text-align: center;\"><td>$w[1]</td><td>$w[2]</td><td>$w[3]</td><td>$w[8]</td><td>$w[9]</td><td style=\"text-align: left;\">$w[10] $w[11] $w[12] $w[13] $w[14] $w[15] $w[16] $w[17] $w[18]</td></tr>\n";
	}
}
print "</table></div><br />\n";

echo "<div id=\"side\">";

/* RRDTool */

echo "<div id=\"rrdtool\">";

echo "<h2>RRDTool processes</h2>\n";

echo "<table>\n";
echo "<tr class=\"psHeader\"><th>PID</th><th>CPU %</th><th>Memory %</th><th>Started at:</th><th>CPU Time</th><th>Command</th></tr>\n";
for($i = 1; $i < sizeof($lines)-1; $i++)
{
        $w = preg_split("/[\s]+/", $lines[$i]);

	if($w[0] == 'root' && (strstr($w[10], 'rrdtool') !== FALSE))
	{
		echo "<tr style=\"background: white; text-align: center;\"><td>$w[1]</td><td>$w[2]</td><td>$w[3]</td><td>$w[8]</td><td>$w[9]</td><td style=\"text-align: left;\">$w[10] $w[11] $w[12] $w[13] $w[14] $w[15] $w[16] $w[17] $w[18]</td></tr>\n";
	}
}
print "</table></div>\n";

/* Apache, if we can find it */

echo "<div id=\"apache\">";
echo "<h2>Apache processes</h2>\n";
echo "<table>\n";
echo "<tr><th>PID</th><th>CPU %</th><th>Memory %</th><th>Started at:</th><th>CPU Time</th><th>Command</th></tr>\n";
for($i = 1; $i < sizeof($lines)-1; $i++)
{
        $w = preg_split("/[\s]+/", $lines[$i]);

	if((strstr($w[10], 'apache') !== FALSE) || (strstr($w[10], 'httpd') !== FALSE))
	{
		echo "<tr style=\"background: white; text-align: center;\"><td>$w[1]</td><td>$w[2]</td><td>$w[3]</td><td>$w[8]</td><td>$w[9]</td><td style=\"text-align: left;\">$w[10] $w[11] $w[12] $w[13] $w[14] $w[15] $w[16] $w[17] $w[18]</td></tr>\n";
	}
}
print "</table>\n";

echo "<h2>PostgreSQL processes</h2>\n";

echo "<table>\n";
echo "<tr><th>PID</th><th>CPU %</th><th>Memory %</th><th>Started at:</th><th>CPU Time</th><th>Command</th></tr>\n";
for($i = 1; $i < sizeof($lines)-1; $i++)
{
        $w = preg_split("/[\s]+/", $lines[$i]);

	if((strstr($w[10], 'postmaster') !== FALSE) || $w[0] == 'postgres')
	{
		echo "<tr style=\"background: white; text-align: center;\"><td>$w[1]</td><td>$w[2]</td><td>$w[3]</td><td>$w[8]</td><td>$w[9]</td><td style=\"text-align: left;\">$w[10] $w[11] $w[12] $w[13] $w[14] $w[15] $w[16] $w[17] $w[18]</td></tr>\n";
	}       
}
print "</table>\n";


echo "</div>\n</div>\n</div>\n";

admin_end();

?>
