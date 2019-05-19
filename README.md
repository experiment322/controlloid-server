[DOWNLOAD](https://github.com/experiment322/controlloid-server/archive/master.zip)

# controlloid-server
This is the server for the [Controlloid](https://github.com/experiment322/controlloid-client) application.

## Windoze instructions
* Go into `/dist/windows/` folder and run `start.bat`
* On first run it will install and configure required software ([vJoy](https://sourceforge.net/projects/vjoystick/))
* This takes a while and you should accept all the prompts for a successful install
* Press any key to close the setup window
* Now you can see available addresses on the server window (**KEEP IT OPEN**)
* Pick one in the same network as your phone and write it exactly in the application
* Alternatively, scan available servers in the application and pick one (should be only one)
* Done!

## Linux instructions
##### Fixing `error: failed to create virtual controller`
* This is caused by restrictive permissions on `/dev/uinput`
* Go into `/dist/linux/udev` folder and run `sudo ./setup.sh $USER`
* Wait for the script to finish, make sure that there were no errors and **reboot** the machine
* Done!