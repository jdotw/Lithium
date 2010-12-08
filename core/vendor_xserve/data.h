typedef struct v_data_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *sysinfo;
  struct i_metric_s *hardware;
  struct i_metric_s *network_state;
  struct i_metric_s *xserve;
  struct i_metric_s *afp;
  struct i_metric_s *appserver;
  struct i_metric_s *dhcp;
  struct i_metric_s *dirserv;
  struct i_metric_s *dns;
  struct i_metric_s *ftp;
  struct i_metric_s *ipfilter;
  struct i_metric_s *ichat;
  struct i_metric_s *mail;
  struct i_metric_s *nat;
  struct i_metric_s *netboot;
  struct i_metric_s *nfs;
  struct i_metric_s *print;
  struct i_metric_s *qtss;
  struct i_metric_s *smb;
  struct i_metric_s *swupdate;
  struct i_metric_s *vpn;
  struct i_metric_s *web;
  struct i_metric_s *webobjects;
  struct i_metric_s *xsan;
  struct i_metric_s *xsan_settings;
  struct i_metric_s *xsan_localproperties;
  struct i_metric_s *xsan_cpu_history;
  struct i_metric_s *xsan_network_history;
  struct i_metric_s *xsan_fibre_history;
  struct i_metric_s *xsan2_history;

} v_data_item;

typedef struct v_data_ipmi_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *chassis;
  struct i_metric_s *serialnum;
  struct i_metric_s *sdr;
  struct i_metric_s *ipmi_cpu;
  struct i_metric_s *ipmi_drive_static;
  struct i_metric_s *ipmi_drive_dynamic;
  struct i_metric_s *ipmi_network_static;
  struct i_metric_s *ipmi_network_dynamic;
  struct i_metric_s *ipmi_ram_static;
  struct i_metric_s *ipmi_ram_dynamic;
} v_data_ipmi_item;

struct i_container_s* v_data_cnt ();
struct v_data_item_s* v_data_static_item ();
struct v_data_ipmi_item_s* v_data_static_ipmi_item ();
int v_data_enable (i_resource *self);
int v_data_disable (i_resource *self);

v_data_item* v_data_item_create ();
void v_data_item_free (void *itemptr);

v_data_ipmi_item* v_data_ipmi_item_create ();
void v_data_ipmi_item_free (void *itemptr);

/* data_info_state.c */
int v_data_info_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_info_state_plistcb ();

/* data_info_hardware.c */
int v_data_info_hardware_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_info_hardware_plistcb ();
int v_data_info_hardware_process_cpuarray ();
int v_data_info_hardware_process_volumearray ();
int v_data_info_hardware_process_ifacearray ();

/* data_network_state.c */
int v_data_network_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_network_state_plistcb ();
int v_data_network_state_process_ifacearray ();

/* data_xserve_state.c */
int v_data_xserve_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_xserve_state_plistcb ();

/* data_xserve_connectivity.c */
int v_data_xserve_process_connectivity ();
int v_data_xserve_process_connectivity_detail ();

/* data_xserve_controls.c */
int v_data_xserve_process_controls ();
int v_data_xserve_process_fan_control ();

/* data_xserve_drives.c */
int v_data_xserve_process_drives ();
int v_data_xserve_process_drives_detail ();

/* data_xserve_ram.c */
int v_data_xserve_process_memory ();

/* data_xserve_identity.c */
int v_data_xserve_process_identity ();

/* data_xserve_sensors.c */
int v_data_xserve_process_sensors ();
int v_data_xserve_process_temp_sensor ();
int v_data_xserve_process_power_sensor ();
int v_data_xserve_process_voltage_sensor ();
int v_data_xserve_process_current_sensor ();

/* data_afp_state.c */
int v_data_afp_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_afp_state_plistcb ();

/* data_appserver_state.c */
int v_data_appserver_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_appserver_state_plistcb ();

/* data_dhcp_state.c */
int v_data_dhcp_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_dhcp_state_plistcb ();

/* data_dirserv_state.c */
int v_data_dirserv_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_dirserv_state_plistcb ();

/* data_dns_state.c */
int v_data_dns_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_dns_state_plistcb ();

/* data_ftp_state.c */
int v_data_ftp_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ftp_state_plistcb ();

/* data_ipfilter_state.c */
int v_data_ipfilter_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipfilter_state_plistcb ();

/* data_ichat_state.c */
int v_data_ichat_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ichat_state_plistcb ();

/* data_mail_state.c */
int v_data_mail_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_mail_state_plistcb ();
int v_data_mail_process_protocols ();

/* data_nat_state.c */
int v_data_nat_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_nat_state_plistcb ();

/* data_netboot_state.c */
int v_data_netboot_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_netboot_state_plistcb ();

/* data_nfs_state.c */
int v_data_nfs_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_nfs_state_plistcb ();

/* data_print_state.c */
int v_data_print_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_print_state_plistcb ();

/* data_qtss_state.c */
int v_data_qtss_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_qtss_state_plistcb ();

/* data_smb_state.c */
int v_data_smb_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_smb_state_plistcb ();

/* data_swupdate_state.c */
int v_data_swupdate_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_swupdate_state_plistcb ();

/* data_vpn_state.c */
int v_data_vpn_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_vpn_state_plistcb ();
int v_data_vpn_process_servers ();

/* data_web_state.c */
int v_data_web_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_web_state_plistcb ();

/* data_webobjects_state.c */
int v_data_webobjects_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_webobjects_state_plistcb ();

/* data_xsan_state.c */
int v_data_xsan_state_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_xsan_state_plistcb ();

/* data_xsan_settings.c */
int v_data_xsan_settings_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_xsan_settings_plistcb ();
int v_data_xsan_process_disks ();
  
/* data_xsan_localproperties.c */
int v_data_xsan_localproperties_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_xsan_localproperties_plistcb ();
int v_data_xsan_process_visibledisks ();
  
/* data_xsan_volume.c */
int v_data_xsan_volume_refresh (i_resource *self, struct i_object_s *obj, int opcode);
int v_data_xsan_volume_plistcb ();
int v_data_xsan_process_volumeinfo ();
int v_data_xsan_process_volumestats ();
int v_data_xsan_process_stripegroups ();

/* data_xsan_cpu_history.c */
int v_data_xsan_cpu_history_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_xsan_cpu_history_plistcb ();
int v_data_xsan_cpu_process_samples ();

/* data_xsan_network_history.c */
int v_data_xsan_network_history_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_xsan_network_history_plistcb ();
int v_data_xsan_network_process_samples ();

/* data_xsan_fibre_history.c */
int v_data_xsan_fibre_history_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_xsan_fibre_history_plistcb ();
int v_data_xsan_fibre_process_samples ();

/* data_xsan_network_history.c */
int v_data_xsan2_history_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_xsan2_history_plistcb ();

/* data_ipmi_sdr.c */
int v_data_ipmi_sdr_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipmi_sdr_ipmicb ();
int v_data_ipmi_sdr_process_cpu (i_resource *self, struct i_list_s *row);
int v_data_ipmi_sdr_process_fbdimm (i_resource *self, struct i_list_s *row);
int v_data_ipmi_sdr_process_pci (i_resource *self, struct i_list_s *row);
int v_data_ipmi_sdr_process_psu (i_resource *self, struct i_list_s *row);
int v_data_ipmi_sdr_process_mainboard (i_resource *self, struct i_list_s *row);
int v_data_ipmi_sdr_process_fan (i_resource *self, struct i_list_s *row);
int v_data_ipmi_sdr_process_dimm (i_resource *self, struct i_list_s *row);

/* data_ipmi_drive_static.c */
int v_data_ipmi_drive_static_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipmi_drive_static_ipmicb ();

/* data_ipmi_drive_dynamic.c */
int v_data_ipmi_drive_dynamic_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipmi_drive_dynamic_ipmicb ();

/* data_ipmi_network_static.c */
int v_data_ipmi_network_static_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipmi_network_static_ipmicb ();

/* data_ipmi_network_dynamic.c */
int v_data_ipmi_network_dynamic_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipmi_network_dynamic_ipmicb ();

/* data_ipmi_ram_static.c */
int v_data_ipmi_ram_static_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipmi_ram_static_ipmicb ();

/* data_ipmi_ram_dynamic.c */
int v_data_ipmi_ram_dynamic_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipmi_ram_dynamic_ipmicb ();

/* data_ipmi_chassis.c */
int v_data_ipmi_chassis_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipmi_chassis_ipmicb ();

/* data_ipmi_serial.c */
int v_data_ipmi_serial_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipmi_serial_ipmicb ();

/* data_ipmi_cpu.c */
int v_data_ipmi_cpu_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ipmi_cpu_ipmicb ();




