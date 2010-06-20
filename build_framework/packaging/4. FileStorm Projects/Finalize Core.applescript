on run argv
	set buildNumber to item 1 of argv
	tell application "FileStorm"
		open "/Users/jwilson/Source/Lithium/build_framework/packaging/4. FileStorm Projects/Lithium Core.fsproj"
		tell document "Lithium Core.fsproj"
			set the disk image name to "/Users/jwilson/Source/Lithium/build_framework/packaging/Disk Images/LithiumCore-" & buildNumber & ".dmg"
			set the file path of file 1 to "/Users/jwilson/Source/Lithium/build_framework/packaging/1. Applications/Core Installer/Lithium-Core-" & buildNumber & ".pkg"
			finalize image with rebuilding
			repeat while building
				delay 0.5
			end repeat
		end tell
		quit
	end tell
end run
