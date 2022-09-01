# Uboot Docs

- The SOC used in BBB is a Sitara Based AM3358x one, but there is no folder(arch/arm/) based on Sitara arch
- Instead there is another Architeture based folders called DaVinci which is also from TI
- The DaVinci Arch is for devices with Video Application
- Since AM3358x(Sitara Based) and OMAP2(DaVinci Based) or OMAP3 or OMAP5 are all based on same peripharal IPs like
    - I2Cx2
    - UARTx4
    - RAM
    - Crypto
    - Timers 
    - HDMI
- We can use the Architeture of DaVinci on Sitara Based SOC also

# Linux Source Tree

- A single OS will work on all devices by just modifying the board.c file irrespective of the devices configuration
    - The Beagle Bone Black uses board-omap3beagle.c
    - The Panda board uses board-omap3panda.c
    - The TouchBook board uses board-omap3panda.c
- These Board files contains code related to 
    - Configuration of the Ports like HDMI, GPIOs, USB, platform device
    - Init sequence like partition, i2c, Memory
    - Configuration of peripharals like mmc/ high speed mmc

# Drivers

 | Boards | SOC | Linux Driver subsystem |
 |--------|-----|------------------------|
 | MOEMS/GPS/SENSORS | 6xUART | drivers/tty/serial/omap-serial.c |
 | MMC/SD/SDIOcards | 3xHsmmc | drivers/mmc/host/omap_hsmmc.c |
 | SENSORS | 3xMcSPI | drivers/spi/spi-omap2-mcspi.c |
 | SENSORS/EEPROM/ | 3xI2C | drivers/i2c/buses/i2c-omap.c |
 | NAND Flash/NOR Flash/External SRAM | GPMC | drivers/memory/omap-gpmc.c |
 | NONE | RTC | drivers/rtc/rtc-omap.c |
 | NONE | Watch Dog Timer | drivers/watchdog/omap_wdt.c |