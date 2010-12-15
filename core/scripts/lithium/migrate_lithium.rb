#!/usr/bin/env ruby1.8

require 'rubygems'
require 'net/ssh'
require 'net/scp'
require "highline/import"

def create_ssh_session(host, username)
  Net::SSH.start(host, username)
end

def get_host_type(session)
  session.exec!("uname").chomp
end

def dump_and_restore_database(session, host)
  # Dump the database on the remote host
  output = session.exec!("/Library/Lithium/LithiumCore.app/Contents/MacOS/pg_dumpall -c -U lithium | gzip > /tmp/lcpsql.migration.sql.gz")
  
  # Download the dump
  session.scp.download!("/tmp/lcpsql.migration.sql.gz", "/tmp/#{host}-lcpsql.migration.sql.gz")
  
  # Remove the remote dump
  output = session.exec!("rm -f /tmp/lcpsql.migration.sql.gz")      
  
  # Import
  output = `gzip -dc /tmp/#{host}-lcpsql.migration.sql.gz | psql -U lithium`      
  puts output
  
  # Remove the local dump
  File.unlink("/tmp/#{host}-lcpsql.migration.sql.gz")
end

def copy_and_update_node_conf(session, remote_host_type)
  
  local_host_type = `uname`.chomp
  
  # Set the paths depending on source/dest host
  if remote_host_type == "Darwin" then
    # Mac Source Path
    src_path = "/Library/Preferences/Lithium/lithium"
  else
    src_path = "/usr/etc/lithium"
  end
  if local_host_type == "Darwin" then
    # Mac Dest Path
    dest_path = "/Library/Preferences/Lithium/lithium"
  else
    dest_path = "/usr/etc/lithium"
  end    
  
  # Scp the node.conf and module_builder folders across
  puts "Source is #{src_path}/*"
  puts "Dest is #{dest_path}"
  session.scp.download!("#{src_path}/node.conf", "#{dest_path}/node.conf")
  session.scp.download!("#{src_path}/module_builder", "#{dest_path}", :recursive => true)
  
  # Update the config
  conf = File.new("#{dest_path}/node.conf", "r").read
  if local_host_type == "Darwin" and remote_host_type == "Linux" then
    # Translate from Mac-style config to Linux
  end
  if local_host_type == "Linux" and remote_host_type == "Darwin" then
    # Translate from Linux-style config to Mac
    conf.gsub!(/port "51132"/, "port \"5432\"")
    conf.gsub!(/host "\/tmp"/, "host \"localhost\"")
    conf.gsub!(/imageroot "\/Library\/Application Support\/Lithium\/ClientService\/Resources\/htdocs"/, "imageroot \"/usr/share/htdocs\"")
    
  end
  File.new("#{dest_path}/node.conf", "w").write(conf)  
  `chown root:www-data #{dest_path}/node.conf`
  `chmod g+w #{dest_path}/lithium/node.conf`

end

def migrate_customer(customer, session, host_type)
  
  puts "Migrating #{customer}"
  
  # Set customer and htdocs paths
  if File.directory?("/Library/Application Support/Lithium/Monitoring Data/History/lithium") then
    # Mac Source Paths
    dest_history_path = "/Library/Application Support/Lithium/Monitoring Data/History/lithium"
    dest_htdocs_path = "/Library/Application Support/Lithium/ClientService/Resources/htdocs"
  else
    # Linux Dest paths
    dest_history_path = "/usr/var/lithium"
    dest_htdocs_path = "/usr/share/htdocs"
  end
  if host_type == "Darwin" then
    # Mac Source paths
    src_history_path = "/Library/Application Support/Lithium/Monitoring Data/History/lithium"
    src_htdocs_path = "/Library/Application Support/Lithium/ClientService/Resources/htdocs"
  else
    # Linux Source paths
    src_history_path = "/usr/var/lithium"
    src_htdocs_path = "/usr/share/htdocs"
  end
  
  # Copy RRD files, etc
  puts "Copying from #{src_history_path}/customer-#{customer}"
  puts "Copying to #{dest_history_path}"
  session.scp.download!("#{src_history_path}/customer-#{customer}", dest_history_path, :recursive => true) do |ch, name, sent, total|
    puts "#{name}: #{sent}/#{total}"
  end

  # Create customer directory in web files (repair script will link files)
  Dir.mkdir("#{dest_htdocs_path}/#{customer}") unless File.directory?("#{dest_htdocs_path}/#{customer}")
  
  # Copy profile
  session.scp.download!("#{src_htdocs_path}/#{customer}/profile.php", "#{dest_htdocs_path}/#{customer}/profile.php")
  
  # Touch main profile to prevent a web-based setup being initiated 
  `touch #{dest_htdocs_path}/default/profile.php`
  
end

#
# Main Program
#

# Sanity Check
unless ENV['USER'] == "root" then
  puts "This script must be run as root"
  exit
end
if ARGV[0].nil? or ARGV[0].empty? then
  puts "Usage: migration_lithium.rb <source_hostname> [username]"
  exit
end

# Warn user about password request
puts "Unless you have SSH keys setup, you will now be prompted for the root password to SSH/SCP to the source host"

# Open SSH Session
host = ARGV[0]
username = ARGV[1] ||= "root"
session = create_ssh_session(host, username)

# Step 1 -- Determine host type
host_type = get_host_type(session)
puts "Host type is #{host_type}"

# Step 2 -- Dump and Restore Database
dump_and_restore_database(session, host)

# Step 3 -- Copy and translate config
copy_and_update_node_conf(session, host_type)

# Step 4 -- Migrate Customers
`psql -U lithium -c 'select name from customers' -t -A`.each_line do |customer|
  customer.chomp!
  migrate_customer(customer, session, host_type) unless customer.empty?
end

# Step 5 -- Run the repairweb script
`repairweb.sh`

# Step 6 -- Run the repairscript script
`repairscripts.sh`

# Step 7 -- Restart Lithium Core
if File.exists?("/Library/LaunchDaemons/com.lithiumcorp.lithium.plist") then
  `killall lithium`
else
  `/etc/init.d/lithiumcore restart`
end

# Cleanup
session.close
