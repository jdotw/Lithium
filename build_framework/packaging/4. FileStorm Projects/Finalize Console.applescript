on run argv
	set buildNumber to item 1 of argv
	tell application "FileStorm"
		open "/Users/jwilson/Source/Lithium/build_framework/packaging/4. FileStorm Projects/Lithium Console.fsproj"
		tell document "Lithium Console.fsproj"
			set disk image name to "/Users/jwilson/Source/Lithium/build_framework/packaging/Disk Images/LithiumConsole-" & buildNumber & ".dmg"
			finalize image with rebuilding
			repeat while building
				delay 0.5
			end repeat
		end tell
		close "Lithium Console.fsproj" with saving
		quit
	end tell
end run