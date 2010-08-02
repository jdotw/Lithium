#!/usr/bin/env ruby
#

# Usage
#

if ARGV.length < 3 then
  puts "Usage: test_email_action.rb <mail server> <sender> <recipient> [smtp_username] [smtp_password]"
  exit
end

# Write out a dummy action script block of XML
#

f = File.new("/var/tmp/action-script-test", File::CREAT|File::TRUNC|File::RDWR, 0644)
f.write("<?xml version=\"1.0\"?>\n")
f.write("<config_variables>")
f.write("<variable><name>sender</name><value>#{ARGV[1]}</value></variable>")
f.write("<variable><name>mailhost</name><value>#{ARGV[0]}</value></variable>")
f.write("<variable><name>recipients</name><value>#{ARGV[2]}</value></variable>")
f.write("<variable><name>smtp_username</name><value>#{ARGV[3]}</value></variable>") if ARGV.length > 3
f.write("<variable><name>smtp_password</name><value>#{ARGV[4]}</value></variable>") if ARGV.length > 4

f.write("<variable><name>customer_url</name><value>test</value></variable>")
f.write("</config_variables>")
f.close

# Execute the action script
#

`/usr/bin/env PERL5LIB=/Library/Lithium/LithiumCore.app/Contents/Resources/Perl perl /Library/Lithium/LithiumCore.app/Contents/Resources/ActionScripts/action_scripts/email_alert.pl report 1234 cust:site:dev:container:object:metric:trigger 3 cust site device container object metric trigger 0 1280491256 0 3 3 3 none 1280491256 1280491256 1 lithium://test/cust/site/dev/container/object/metric/trigger lithium://test/cust/site/dev/container/object/metric /var/tmp/action-script-test`

# Clean up
# 

File.delete("/var/tmp/action-script-test")
