/*
 *  helpertool.c
 *  LCAdminTools
 *
 *  Created by James Wilson on 5/08/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/mman.h>

#include "helpertool.h"

int  execlp(const char *, const char *, ...);
int  setuid(uid_t);
int  setgid(gid_t);
unsigned int sleep(unsigned int seconds);
ssize_t read(int fildes, void *buf, size_t nbyte);
ssize_t write(int fildes, const void *buf, size_t nbyte);
int close(int fildes);
off_t lseek(int fildes, off_t offset, int whence);
int unlink(const char *path);

int main (int argc, char *argv[])
{
	if (argc < 2) return 0;
	
	setuid (0);
	setgid (0);
	
	/*
	 * Process Control
	 */
	
	if (strcmp(argv[1], "lithium") == 0)
	{
		if (strcmp(argv[2], "start") == 0) lithium_start();
		else if (strcmp(argv[2], "stop") == 0) lithium_stop();
		else if (strcmp(argv[2], "restart") == 0) 
		{
			lithium_stop();
			lithium_start();
		}
		fprintf (stdout, "OK\n");		
	}
	
	else if (strcmp(argv[1], "postgres") == 0)
	{
		if (strcmp(argv[2], "start") == 0) postgres_start();
		else if (strcmp(argv[2], "stop") == 0) postgres_stop();
		fprintf (stdout, "OK\n");		
	}

	else if (strcmp(argv[1], "clientd") == 0)
	{
		if (strcmp(argv[2], "start") == 0) clientd_start();
		else if (strcmp(argv[2], "stop") == 0) clientd_stop();
		fprintf (stdout, "OK\n");		
	}

	else if (strcmp(argv[1], "clientd") == 0)
	{
		if (strcmp(argv[2], "start") == 0) clientd_start();
		else if (strcmp(argv[2], "stop") == 0) clientd_stop();
		fprintf (stdout, "OK\n");		
	}

	else if (strcmp(argv[1], "all") == 0)
	{
		if (strcmp(argv[2], "start") == 0) all_start ();
		else if (strcmp(argv[2], "stop") == 0) all_stop ();
		fprintf (stdout, "OK\n");		
	}	
	
	/*
	 * Backup and Restore
	 */
	
	else if (strcmp(argv[1], "archive") == 0)
	{
		archive (argv[2], argv[3]);
	}
	
	else if (strcmp(argv[1], "restore") == 0)
	{
		restore (argv[2], argv[3]);
	}
	
	/*
	 * Config 
	 */

	else if (strcmp(argv[1], "writeconfig") == 0)
	{
		/* Create DB */
		system ("sudo -u lithium /Library/Lithium/LithiumCore.app/Contents/MacOS/createdb lithium");
		fprintf (stdout, "WRITECONFIG: OK");
		fflush (stdout);
		
		/* Create Path */
		char *dir;
		asprintf (&dir, "/Library/Application Support/Lithium/Monitoring Data/History/lithium/customer-%s", argv[2]);
		int res = mkdir (dir, 0775);
		if (res == -1)
		{
			fprintf (stderr, "Failed to create dir %s", dir);
		}
		free (dir);
		asprintf (&dir, "/Library/Application Support/Lithium/Monitoring Data/History/lithium/customer-%s/service_scripts", argv[2]);
		res = mkdir (dir, 0775);
		if (res == -1)
		{
			fprintf (stderr, "Failed to create dir %s", dir);
		}
		free (dir);
		asprintf (&dir, "/Library/Application Support/Lithium/Monitoring Data/History/lithium/customer-%s/action_scripts", argv[2]);
		res = mkdir (dir, 0775);
		if (res == -1)
		{
			fprintf (stderr, "Failed to create dir %s", dir);
		}
		free (dir);
		

		/* Write config */
		writeconfig ();
		
		/* Write profile.php file */
		if (argc > 2)
		{ writeprofile (argv[2], argv[3]); }
		
		/* Restart */
		system ("launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.lithium.plist");
		system ("launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.mars.plist");
		sleep (2);
		system ("launchctl load -w /Library/LaunchDaemons/com.lithiumcorp.mars.plist");
		system ("launchctl load -w /Library/LaunchDaemons/com.lithiumcorp.lithium.plist");
	}	
	
	/*
	 * Self-update 
	 */

	else if (strcmp(argv[1], "update") == 0)
	{
		update (argv[2], argv[3], argv[4]);
	}
	
	/*
	 * L49 Import 
	 */
	
	else if (strcmp(argv[1], "l49_database_start") == 0)
	{ 
		l49_database_start (); 
	}

	else if (strcmp(argv[1], "l49_database_stop") == 0)
	{ 
		l49_database_stop (); 
	}	
	
	else if (strcmp(argv[1], "l49_database_export") == 0)
	{ 
		l49_database_export (); 
	}	

	else if (strcmp(argv[1], "l50_database_start") == 0)
	{ 
		l50_database_start (); 
	}		
	
	else if (strcmp(argv[1], "l50_database_import") == 0)
	{ 
		l50_database_import (); 
	}		

	else if (strcmp(argv[1], "l50_database_stop") == 0)
	{ 
		l50_database_stop (); 
	}			

	else if (strcmp(argv[1], "l49_rrd_copy") == 0)
	{ 
		l49_rrd_copy(); 
	}			

	else if (strcmp(argv[1], "l49_clientd_copy") == 0)
	{ 
		l49_clientd_copy(); 
	}	
	
	else if (strcmp(argv[1], "l49_rename_root") == 0)
	{ 
		l49_rename_root();
		fprintf (stdout, "OK\n");
	}	

	else if (strcmp(argv[1], "l49_reinstate_root") == 0)
	{ 
		l49_reinstate_root ();
		fprintf (stdout, "OK\n");
	}	
	
	else if (strcmp(argv[1], "l50_finalize") == 0)
	{ 
		l50_finalize();
	}	
	
	/*
	 * Module Management
	 */
	
	else if (strcmp(argv[1], "mod_install") == 0)
	{ 
		mod_install(argv[2], argv[3]);
	}		

	else if (strcmp(argv[1], "mod_delete") == 0)
	{ 
		mod_delete(argv[2]);
	}		
	
	
	return 0;
}

#pragma mark "Process Control"

int lithium_start ()
{
	system ("/bin/launchctl load -w /Library/LaunchDaemons/com.lithiumcorp.mars.plist");
	system ("/bin/launchctl load -w /Library/LaunchDaemons/com.lithiumcorp.lithium.plist");
	return 0;
}

int lithium_stop ()
{
	system ("/bin/launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.lithium.plist");
	system ("/bin/launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.mars.plist");
	return 0;
}

int postgres_start ()
{
	system ("/bin/launchctl load -w /Library/LaunchDaemons/com.lithiumcorp.database.plist");
	return 0;
}

int postgres_stop ()
{
	system ("/bin/launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.database.plist");
	return 0;
}

int clientd_start ()
{
	system ("/bin/launchctl load -w /Library/LaunchDaemons/com.lithiumcorp.clientd.plist");
	return 0;
}

int clientd_stop ()
{
	system ("/bin/launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.clientd.plist");
	return 0;
}

int all_start ()
{
	postgres_start();
	lithium_start();
	clientd_start();
	return 0;
}

int all_stop ()
{
	clientd_stop();
	lithium_stop();
	postgres_stop();
	return 0;
}

#pragma mark "Backup and Restore"

int archive (char *uuid, char *archive_file)
{
	/*
	 * Stop Lithium
	 */
	
	system ("launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.lithium.plist");
	system ("launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.mars.plist");
	fprintf (stdout, "STOP: OK\n");		
	
	/*
	 * Database Dump and Archive
	 */
	
	/* Create tmp dir */
	char *tmp_dir;
	asprintf (&tmp_dir, "/tmp/%s", uuid);
	if (mkdir (tmp_dir, 0700) == -1)
	{ printf ("DB: ERROR Failed to create temporary directory %s\n", tmp_dir); exit (0); }
	
	/* Dump DB */
	int retval;
	char *db_file;
	char *command_str;
	asprintf (&db_file, "%s/db", tmp_dir); 
	asprintf (&command_str, "cd /Library/Lithium/LithiumCore.app/Contents/MacOS; su lithium -c './pg_dumpall -c' >  %s", db_file);
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("DB: ERROR Failed to dump database to %s\n", db_file); exit (0); }
	
	/* GZIP DB */
	asprintf (&command_str, "gzip %s", db_file);
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("DB: ERROR Failed to gzip database fule %s\n", db_file); exit (0); }
	
	/* DB Finished */
	fprintf (stdout, "DB: OK\n");
	fflush(NULL);
	
	/*
	 * Data and Config Archive 
	 */
	
	char *tgz_file;
	asprintf (&tgz_file, "%s/data.tar.gz", tmp_dir);
	asprintf (&command_str, "cd '/Library/Application Support/Lithium/Monitoring Data'; tar zcf '%s' 'History'", tgz_file);
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("DATA: ERROR Failed to archive metric history data"); exit (0); }

	asprintf (&tgz_file, "%s/config.tar.gz", tmp_dir);
	asprintf (&command_str, "cd '/Library/Preferences'; tar zcf '%s' 'Lithium'", tgz_file);
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("DATA: ERROR Failed to archiveconfiguration"); exit (0); }
	
	printf ("DATA: OK\n");
	fflush(NULL);

	
	
	/*
	 * Web Files 
	 */
	
	asprintf (&tgz_file, "%s/web.tar.gz", tmp_dir);
	asprintf (&command_str, "cd '/Library/Application Support/Lithium/ClientService/Resources'; tar zcf '%s' `find htdocs/%s | grep /profile`", tgz_file, "*"); 
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("WEB: ERROR Failed to archive web profile.php files"); exit (0); }
	fprintf (stdout, "WEB: OK\n");
	fflush(NULL);
	 
	/* 
	 * Write XML
	 */
	 
	char *xml_file;	 
	asprintf (&xml_file, "%s/info.xml", tmp_dir);
	asprintf (&command_str, "echo '<?xml version=\"1.0\"?>' > '%s'", xml_file);
	system (command_str);
	asprintf (&command_str, "echo '<archive>' >> '%s'", xml_file);
	system (command_str);
	asprintf (&command_str, "echo \"<system>`uname -a`</system>\" >> '%s'", xml_file);
	system (command_str);
	asprintf (&command_str, "echo \"<timestamp>`date`</timestamp>\" >> '%s'", xml_file);
	system (command_str);
	asprintf (&command_str, "echo '</archive>' >> '%s'", xml_file);
	system (command_str);

	/* 
	 * Create final archive 
	 */
	
	asprintf (&command_str, "cd %s; tar zcf '%s' *", tmp_dir, archive_file);
	system (command_str);
	fprintf (stdout, "ARCHIVE: OK\n");
	fflush(NULL);
	
	/*
	 * Restart Lithium
	 */
	
	system ("launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.lithium.plist");
	system ("launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.mars.plist");
	sleep (2);
	system ("launchctl load -w /Library/LaunchDaemons/com.lithiumcorp.lithium.plist");
	system ("launchctl load -w /Library/LaunchDaemons/com.lithiumcorp.mars.plist");
	fprintf (stdout, "RESTART: OK\n");
	
	return 0;
}

int restore (char *uuid, char *archive_file)
{
	/*
	 * Stop Lithium
	 */
	
	system ("launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.lithium.plist");
	system ("launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.mars.plist");
	fprintf (stdout, "STOP: OK\n");		
	
	/*
	 * DB Restore
	 */
	
	/* Create tmp dir */
	char *tmp_dir;
	asprintf (&tmp_dir, "/tmp/%s", uuid);
	if (mkdir (tmp_dir, 0755) == -1)
	{ printf ("DB: ERROR Failed to create temporary directory %s\n", tmp_dir); exit (0); }
	
	/* Move Archive */
	char *command_str;
	asprintf (&command_str, "cp '%s' '%s/archive.tar.gz'", archive_file, tmp_dir);
	int retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("DB: ERROR Moving archive to %s", tmp_dir); exit (0); }
	
	/* Untar */
	asprintf (&command_str, "cd %s; tar zxf archive.tar.gz", tmp_dir);
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("DB: ERROR Failed to unarchive %s", archive_file); exit (0); }
	
	/* Unzip */
	asprintf (&command_str, "cd %s; gzip -d db.gz", tmp_dir);
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("DB: ERROR Failed to unarchive db file"); exit (0); }

	/* DB Finished */
	fprintf (stdout, "ARCHIVE: OK\n");
	fflush(NULL);	
	
	/* Import DB */
	system ("sudo -u lithium /Library/Lithium/LithiumCore.app/Contents/MacOS/createdb lithium");
	char *db_file;
	asprintf (&db_file, "%s/db", tmp_dir); 
	asprintf (&command_str, "cd /Library/Lithium/LithiumCore.app/Contents/MacOS; sudo -u lithium ./psql -q -L /dev/null -o /dev/null -f %s", db_file);
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("DB: ERROR Failed to import database\n"); exit (0); }
	
	/* DB Finished */
	fprintf (stdout, "DB: OK\n");
	fflush(NULL);
	
	/*
	 * Data and Config Restore
	 */
	
	char *tgz_file;
	asprintf (&tgz_file, "%s/data.tar.gz", tmp_dir);
	asprintf (&command_str, "cd '/Library/Application Support/Lithium/Monitoring Data'; tar zxf '%s'", tgz_file);
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("DATA: ERROR Failed to restore metric history"); exit (0); }

	asprintf (&tgz_file, "%s/config.tar.gz", tmp_dir);
	asprintf (&command_str, "cd '/Library/Preferences'; tar zxf '%s'", tgz_file);
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("DATA: ERROR Failed to restore configuration"); exit (0); }
	
	printf ("DATA: OK\n");
	fflush(NULL);
	
	/*
	 * Web Files 
	 */
	
	asprintf (&tgz_file, "%s/web.tar.gz", tmp_dir);
	asprintf (&command_str, "cd '/Library/Application Support/Lithium/ClientService/Resources'; tar zxf '%s'", tgz_file); 
	retval = system (command_str);
	free (command_str);
	if (retval != 0)
	{ printf ("WEB: ERROR Failed to archive web profile.php files"); exit (0); }
	fprintf (stdout, "WEB: OK\n");
	fflush(NULL);
	
	system ("touch '/Library/Application Support/Lithium/ClientService/Resources/htdocs/Documents/default/profile.php'");
	system ("/Library/Lithium/LithiumCore.app/Contents/MacOS/repairweb.sh");
	system ("/Library/Lithium/LithiumCore.app/Contents/MacOS/repairscripts.sh");
	
	/*
	 * Restart Lithium
	 */
	
	system ("launchctl load -w /Library/LaunchDaemons/com.lithiumcorp.lithium.plist");
	system ("launchctl load -w /Library/LaunchDaemons/com.lithiumcorp.mars.plist");
	fprintf (stdout, "RESTART: OK\n");
	
	return 0;
}

#pragma mark "Configuration Management"

int writeconfig ()
{
	char buf[4096];
	
	/* Read config */
	int num = read (0, buf, 4095);
	if (num < 1) 
	{
		fprintf (stderr, "Failed to open stdin to read");
		exit (1);
	}
	
	/* Open config file */
	int fd = open ("/Library/Preferences/Lithium/lithium/node.conf", O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if (fd == -1) 
	{
		fprintf (stderr, "Failed to open node.conf for writing");
		exit (1);
	}
	int written = write (fd, buf, num);
	if (written != num) 
	{
		fprintf (stderr, "Failed to write config"); 
		exit (1);
	}
	
	return 0;
}

int writeprofile (char *name, char *desc)
{
	char *profile;
	asprintf (&profile, "<?php\n\n$customer_id_str = \"%s\";\n$customer_name = \"%s\";\n\n?>\n", name, desc);
	
	/* Create Dir */
	char *dir;
	asprintf (&dir, "/Library/Application Support/Lithium/ClientService/Resources/htdocs/%s", name);
	int res = mkdir (dir, 0775);
	if (res == -1)
	{
		fprintf (stderr, "Failed to create dir %s", dir);
	}
	
	/* Open config file */
	char *file;
	asprintf (&file, "%s/profile.php", dir);
	int fd = open (file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if (fd == -1) 
	{
		fprintf (stderr, "Failed to open %s for writing", file);
	}
	int written = write (fd, profile, strlen(profile));
	if (written != strlen(profile)) 
	{
		fprintf (stderr, "Failed to write profile");
	}
	
	/* Repair web links */
	system ("touch '/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/profile.php'");
	system ("/Library/Lithium/LithiumCore.app/Contents/MacOS/repairweb.sh");
	system ("/Library/Lithium/LithiumCore.app/Contents/MacOS/repairscripts.sh");
	
	/* Create customer directory */
	asprintf (&dir, "/Library/Application Support/Lithium/Monitoring Data/History/lithium/customer-%s", name);
	res = mkdir (dir, 0775);
	if (res == -1)
	{
		fprintf (stderr, "Failed to create dir %s", dir);
	}
	
	return 0;
}

#pragma mark "Self Update"

int update (char *archive_file, char *extract_path, char *pkg_file)
{
	/* Unzip update */
	char *str;
	fprintf (stderr, "unzip -o '%s' -d '%s'\n", archive_file, extract_path);
	asprintf (&str, "unzip -o '%s' -d '%s'", archive_file, extract_path);
	system (str);
	free (str);
	
	/* Run Installer */
	execlp ("installer", "installer", "-verboseR", "-pkg", pkg_file, "-target", "/", NULL);
	
	return 0;
}

#pragma mark "Lithium 4.9 Upgrade Process"

int l49_database_start ()
{
	execlp ("bash", "bash", "/Library/Lithium/LithiumCore.app/Contents/MacOS/l49_database_up.sh", NULL);	
	return 0;
}

int l49_database_stop ()
{
	execlp ("bash", "bash", "/Library/Lithium/LithiumCore.app/Contents/MacOS/l49_database_down.sh", NULL);	
	return 0;
}

int l49_database_export ()
{
	execlp ("bash", "bash", "/Library/Lithium/LithiumCore.app/Contents/MacOS/l49_database_export.sh", NULL);	
	return 0;
}

int l50_database_start ()
{
	execlp ("bash", "bash", "/Library/Lithium/LithiumCore.app/Contents/MacOS/l50_database_up.sh", NULL);	
	return 0;
}

int l50_database_import ()
{
	execlp ("bash", "bash", "/Library/Lithium/LithiumCore.app/Contents/MacOS/l50_database_import.sh", NULL);	
	return 0;
}

int l50_database_stop ()
{
	execlp ("bash", "bash", "/Library/Lithium/LithiumCore.app/Contents/MacOS/l50_database_down.sh", NULL);	
	return 0;
}

int l49_rrd_copy ()
{
	execlp ("bash", "bash", "/Library/Lithium/LithiumCore.app/Contents/MacOS/l49_rrd_copy.sh", NULL);	
	return 0;
}

int l49_clientd_copy ()
{
	execlp ("bash", "bash", "/Library/Lithium/LithiumCore.app/Contents/MacOS/l50_clientd_copy.sh", NULL);	
	return 0;
}

int l49_rename_root ()
{
	return rename ("/Lithium", "/Lithium-4.9-Archive");
}

int l49_reinstate_root ()
{
	return rename ("/Lithium-4.9-Archive", "/Lithium");
}

int l50_finalize ()
{
	execlp ("bash", "bash", "/Library/Lithium/LithiumCore.app/Contents/MacOS/l50_finalize.sh", NULL);	
	return 0;
}

int mod_install (char *sourcepath, char *filename)
{
	int input, output;
	size_t filesize;
	char *destpath;

	asprintf (&destpath, "/Library/Preferences/Lithium/lithium/module_builder/%s", filename);
	
	if((input = open(sourcepath, O_RDONLY)) == -1)
		fprintf(stdout, "ERROR: opening file: %s\n", sourcepath), exit(1);
	
	if((output = open(destpath, O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1)
		fprintf(stdout, "ERROR: opening file: %s\n", destpath), exit(1);
	
	filesize = lseek(input, 0, SEEK_END);
	lseek(input, 0, SEEK_SET);
	lseek(output, 0, SEEK_SET);

	while (1)
	{
		char buf[1024];
		int numread = read(input, buf, 1024);
		if (numread > 0)
		{
			int numwritten = write(output, buf, numread);
			if (numwritten < numread)
			{ 
				perror("ERROR: Writing - ");
				exit (1);
			}
		}
		if (numread == -1)
		{
			perror("ERROR: Reading - ");
			exit (1);
		}
		if (numread < 1024)
		{
			break;
		}
	}
	
	close(input);
	close(output);

	printf("OK\n");
	
	return 0;
}

int mod_delete (char *filename)
{
	char *fullpath;
	asprintf (&fullpath, "/Library/Preferences/Lithium/lithium/module_builder/%s", filename);
	
	int result = unlink (fullpath);
	if (result != 0)
	{ perror("ERROR: "); }
	else 
	{ printf("OK\n"); }
	
	free (fullpath);
	
	return 0;
}
