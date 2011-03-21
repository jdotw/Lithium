<?php

function a_script_download ($resaddr, $entaddr, $type, $name, &$ret_val)
{
  $tempfile =  actuate ("a_script_download", "\"$resaddr\" \"$entaddr\" \"$type\" \"$name\"", &$ret_val);
  $handle = fopen ($tempfile, 'r');
  while ($handle && !feof($handle)) 
  {             
    $buffer = fgets($handle, 4096);                     
    echo $buffer;                         
  }
  fclose($handle);
}

?>
