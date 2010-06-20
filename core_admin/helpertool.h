/*
 *  helpertool.h
 *  LCAdminTools
 *
 *  Created by James Wilson on 5/08/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#pragma mark "Process Control"
int lithium_start ();
int lithium_stop ();
int postgres_start ();
int postgres_stop ();
int clientd_start ();
int clientd_stop ();
int all_start ();
int all_stop ();

#pragma mark "Backup and Restore"
int archive (char *uuid, char *archive_file);
int restore (char *uuid, char *archive_file);

#pragma mark "Configuration Management"
int writeconfig ();
int writeprofile (char *name, char *desc);

#pragma mark "Self Update"
int update (char *archive_file, char *extract_path, char *pkg_file);

#pragma mark "Lithium 4.9 Upgrade Process"
int l49_database_start ();
int l49_database_stop ();
int l49_database_export ();
int l49_database_import ();
int l49_rrd_copy ();
int l49_clientd_copy ();
int l49_rename_root (); 
int l50_finalize ();
int l49_reinstate_root ();
int l50_database_start ();
int l50_database_import ();
int l50_database_stop ();

#pragma mark Module Management
int mod_install (char *sourcepath, char *filename);
int mod_delete (char *filename);
