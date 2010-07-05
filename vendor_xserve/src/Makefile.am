CFLAGS = @CFLAGS@ -g
LIBS = @LIBS@ 

moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = xserve_g5.so xserve_intel.so osx_server.so osx_client.so

common_SOURCES = csv.c \
				   ipmitool_proc.c \
				   ipmitool_util.c \
				   ipmitool_apple.c \
				   ipmitool_apple_req.c \
				   sysinfo.c \
				   sysinfo_item.c \
				   sysinfo_cntform.c \
				   sysinfo_objform.c \
				   mainboard.c \
				   mainboard_item.c \
				   mainboard_cntform.c \
				   mainboard_objform.c \
				   chassis.c \
				   chassis_item.c \
				   chassis_cntform.c \
				   chassis_objform.c \
				   pci.c \
				   pci_item.c \
				   pci_cntform.c \
				   pci_objform.c \
				   psu.c \
				   psu_item.c \
				   psu_cntform.c \
				   psu_objform.c \
				   cpu.c \
				   cpu_item.c \
				   cpu_cntform.c \
				   cpu_objform.c \
				   fans.c \
				   fans_item.c \
				   fans_cntform.c \
				   fans_objform.c \
				   network.c \
				   network_item.c \
				   network_cntform.c \
				   network_objform.c \
				   volumes.c \
				   volumes_item.c \
				   volumes_cntform.c \
				   volumes_objform.c \
				   drives.c \
				   drives_item.c \
				   drives_cntform.c \
				   drives_objform.c \
				   drives_smart.c \
				   services.c \
				   services_item.c \
				   services_cntform.c \
				   services_objform.c \
				   ram.c \
				   ram_item.c \
				   ram_cntform.c \
				   ram_objform.c \
				   enclosure.c \
				   enclosure_item.c \
				   enclosure_cntform.c \
				   enclosure_objform.c \
				   power.c \
				   power_item.c \
				   power_cntform.c \
				   power_objform.c \
				   plist.c \
				   plist_get.c \
				   plist_fdset.c \
				   plist_req.c \
				   plist_util.c \
				   xsan.c \
				   xsaninfo.c \
				   xsaninfo_item.c \
				   xsanvol.c \
				   xsanvol_item.c \
				   xsanvol_objfact.c \
				   xsanaffinity_item.c \
				   xsanaffinity_objfact.c \
				   xsansp.c \
				   xsansp_item.c \
				   xsansp_objfact.c \
				   xsannode.c \
				   xsannode_item.c \
				   xsannode_objfact.c \
				   xsanvisdisk.c \
				   xsanvisdisk_item.c \
				   xsanvisdisk_objfact.c \
				   data.c \
				   data_item.c \
				   data_info_state.c \
				   data_info_hardware.c \
				   data_network_state.c \
				   data_afp_state.c \
				   data_appserver_state.c \
				   data_dhcp_state.c \
				   data_dirserv_state.c \
				   data_dns_state.c \
				   data_ftp_state.c \
				   data_ichat_state.c \
				   data_ipfilter_state.c \
				   data_mail_state.c \
				   data_nat_state.c \
				   data_netboot_state.c \
				   data_nfs_state.c \
				   data_print_state.c \
				   data_qtss_state.c \
				   data_smb_state.c \
				   data_swupdate_state.c \
				   data_vpn_state.c \
				   data_web_state.c \
				   data_webobjects_state.c \
				   data_xserve_connectivity.c \
				   data_xserve_controls.c \
				   data_xserve_drives.c \
				   data_xserve_identity.c \
				   data_xserve_ram.c \
				   data_xserve_sensors.c \
				   data_xserve_state.c \
				   data_xsan_state.c \
				   data_xsan_volume.c \
				   data_xsan_settings.c \
				   data_xsan_localproperties.c \
				   data_xsan_cpu_history.c \
				   data_xsan_network_history.c \
				   data_xsan_fibre_history.c \
				   data_xsan2_history.c \
				   data_ipmi_sdr.c \
				   data_ipmi_drive_dynamic.c \
				   data_ipmi_drive_static.c \
				   data_ipmi_network_static.c \
				   data_ipmi_network_dynamic.c \
				   data_ipmi_ram_static.c \
				   data_ipmi_ram_dynamic.c \
				   data_ipmi_serial.c \
				   data_ipmi_cpu.c \
				   data_ipmi_chassis.c \
				   snmp.c

xserve_g5_so_SOURCES = xserve_g5.c \
						$(common_SOURCES)

xserve_intel_so_SOURCES = xserve_intel.c \
						  $(common_SOURCES)

osx_server_so_SOURCES = osx_server.c \
				 $(common_SOURCES)

osx_client_so_SOURCES = osx_client.c \
						$(common_SOURCES)



