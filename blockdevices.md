\# cat /etc/devolo/variants 
ProductName=dLAN 500 WiFi
==========================

    \# cat /proc/cmdline
    console=ttyS0,57600 root=31:04 rootfstype=squashfs init=/sbin/init mtdparts=ar7240-nor0:128k(Bootloader),64k(Config1),64k(Config2),960k(Kernel),6912k(RootFS),64k(ART),7872k@0x40000(Firmware1) mem=64M 

    mtdblock3: [Kernel] u-boot legacy uImage, Linux Kernel Image, Linux/MIPS, OS Kernel Image (lzma), 892150 bytes
    mtdblock4: [RootFS] Squashfs filesystem, little endian, version 4.0, 5405085 bytes, 754 inodes, blocksize: 65536 bytes


