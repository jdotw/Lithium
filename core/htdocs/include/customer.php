<?php

function customer_menu ($id_string)
{
  $addr_str = actuate ("a_customer_get_addr_by_id_str", $id_string, $ret_val);
  if (!$addr_str) return ("Customer Not Found!");
  
  $home_link = link_form_get ($addr_str, "main");
  $users_link = link_form_get ($addr_str, "user_list");
  
  return "
          <TABLE>
          <TR><TD><A HREF=$home_link>Home</A></TD></TR>
          </TABLE>
          ";
}

function customer_resaddr ($id_string)
{
  if ($id_string == "admin")
  {
    /* Admin special handling */
    return "::4:0:";
  }

  /* Standard handling */
  return "::5:0:" . $id_string;
}

?>
