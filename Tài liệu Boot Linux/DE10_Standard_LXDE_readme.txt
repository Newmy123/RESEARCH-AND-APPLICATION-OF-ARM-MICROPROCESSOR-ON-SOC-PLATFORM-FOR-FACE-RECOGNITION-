Prerequisite: you need at least a 4Gb microSD card

Setup steps:
1. Unzip the image file
2. Insert the microSD card to the host PC and write the image file into the microSD with the Win32DiskImager tool
3. Insert the programmed microSD card to the DE10-Standard board
4. Set the MSEL[4:0] on your DE10-Standard to 01010
5. Connect a HDMI monitor to the DE10-Standard board
6. Conect USB mouse and keyboard to the USB ports on the board
7. Power on the board and you will see the LXDE graphical environment

Additional information:
1. You can read the DE10-Standard_Getting_Started_Guide.pdf in the system CD for more details about setting up the Board
2. The Quartus project is built in Quartus II v16.1. The project is located at
/Demonstrations/SoC_FPGA/ControlPanel/Quartus
3. The Linux kernel version is 4.5. You can get the kernel source code from https://github.com/terasic/linux-socfpga
4. The default root is passwordless. Please press the Enter key when the terminal is asking for the password.
