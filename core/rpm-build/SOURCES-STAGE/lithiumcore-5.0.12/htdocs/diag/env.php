<?php
    
  print "<?xml version=\"1.0\">\n";
  print "<SERVER>\n";
  
  foreach($_SERVER as $key=>$val)
  {
  print "\t<$key>$val</$key>\n";
  } 
  
  print "</SERVER>\n";
?>

                
