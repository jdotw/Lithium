<?php

function a_script_upload ($resaddr, $entaddr, $type, $name, &$ret_val)
{
  if ($_FILES['scriptfile'] && is_uploaded_file($_FILES['scriptfile']['tmp_name']))
  { $path = $_FILES['scriptfile']['tmp_name']; }
  else
  { $path = ""; }

  return actuate ("a_script_upload", "\"$resaddr\" \"$entaddr\" \"$type\" \"$name\" \"$path\"", &$ret_val);
}

?>
