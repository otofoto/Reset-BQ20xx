#!/bin/bash

### Functions

bq2092_stuff() {
   clear
   echo "Connect your SMBC, SMBD and GND to the battery's external port."
   echo "Press 'y' and [ENTER] when ready."
   echo
   echo -en "   > "
   read LINE
   echo
   if [ "$LINE" = "y" ]; then
      echo "Okay, trying to reset the battery."
      sleep 2
      ./reset-bq2092
      echo
      echo "Reset attempt over."
      echo "I cannot really detect whether or not the reset was successful."
      echo "The 'reset' is an i2c write transaction that always fails."
      echo "If the indication LED's on your battery went cockoo, that's"
      echo "a good sign. To finish the reset, disconnect the controller PCB"
      echo "from the battery cells and reconnect, to power-cycle the chip."
      echo "Check out the SBS registers dump - the 'actual capacity' should"
      echo "already be all the way up or at least should not resist going up"
      echo "during the first charge cycle."
      echo "You still need to run the pack through a few charge cycles"
      echo "IN THE NOTEBOOK to see if the battery returns to full capacity."
      echo
      echo "Press ENTER to continue."
      read
         cat < discharge_warning.txt
      echo
      echo "Press ENTER to return to the menu."
      read
   else
      echo "Okay, giving up."
      sleep 2
   fi
}



bq2040_stuff() {
   clear
   echo "   Please select one of the following:  (and ENTER)"
   echo
   echo " 1    attempt to reset the BQ2040 using the 'normal' procedure (defunct?)"
   echo " 2    set a new 'actual capacity' value into the on-board Flash memory   "
   echo
   echo " q    cancel - return to the main menu                                   "
   echo 
   echo -n "    > "

   read LINE
   case $LINE in
      "1")
         echo
         echo "Please note that this soft reset doesn't seem to work with the "
         echo "BQ2040, despite being described in its data sheet.             "
         echo "Consider using the more complicated workaround via the onboad  "
         echo "CMOS Flash chip.                                               "
         echo
	 echo "Connect your SMBC, SMBD and GND to the battery's external port."
	 echo "Press 'y' and [ENTER] when ready."
	 echo
	 echo -en "   > "
	 read LINE
	 echo
	 if [ "$LINE" = "y" ]; then
	    echo "Okay, trying to reset the battery."
	    sleep 2
            ./reset-bq2040
	    echo
	    echo "Reset attempt over."
	    echo "I cannot really detect whether or not the reset was successful."
	    echo "The 'reset' is an i2c write transaction that always fails."
	    echo "If the indication LED's on your battery went cockoo, that's"
	    echo "a good sign. To finish the reset, disconnect the controller PCB"
	    echo "from the battery cells and reconnect, to power-cycle the chip."
            echo "Check out the SBS registers dump - the 'actual capacity' should"
            echo "already be all the way up or at least should not resist going up"
            echo "during the first charge cycle."
            echo "You still need to run the pack through a few charge cycles"
            echo "IN THE NOTEBOOK to see if the battery returns to full capacity."
	    echo
	    echo "Press ENTER to continue."
	    read
               cat < discharge_warning.txt
            echo
	    echo "Press ENTER to return to the menu."
	    read
	 else
           echo "Okay, giving up."
           sleep 2
         fi
      ;;
      "2")
         clear
         echo
         echo "Most smart battery controller IC's, including the BQ2040, use"
         echo "a stand-alone EEPROM memory chip (AKA CMOS Flash) to store their"
         echo "initial data and a few runtime values - one of them is the learned"
         echo "battery capacity. This allows us to use a simple workaround, if"
         echo "the regular reset doesn't work. We can reprogram the learned capacity"
         echo "to a higher value."
         echo
	 echo "This operation WILL NOT WORK using the SMBUS that's available on the"
	 echo "external connector of the battery pack. You have to wiretap the CMOS"
	 echo "Flash chip - the smaller (eight-legged) chip that's on the tiny PCB"
         echo "inside the battery pack, next to the battery controller IC."
         echo "While talking straight to the CMOS Flash, power down the battery"
         echo "controller chip, to prevent it from intervening - disconnect the"
         echo "controller PCB from the cells. The power to the CMOS Flash will be"
         echo "provided externally, e.g. from the +5V line of the keyboard port."
         echo
         echo "Press ENTER for more detailed instructions..."
         read
         echo "To sum up :"
         echo "Disconnect the cells from the gas-gauge PCB."
	 echo "Tap your SMBC (SCL), SMBD (SDA), +5V (from keyboard plug) and GND"
	 echo "to the respective pins of the onboard CMOS Flash chip (24C01, 24C02)."
         cat < schematic.txt
	 echo "Press 'y' and [ENTER] when ready."
	 echo
	 echo -en "   > "
	 read LINE
	 echo
	 if [ "$LINE" = "y" ]; then
	    echo "Please enter the new capacity to be programmed, in units of mAh."
	    echo "Add perhaps 10% or more above the nominal capacity of the new cells,"
	    echo "so that the controller has some workspace for the initial learning:"
            echo
	    echo -n "  New capacity: "
            read NEW_CAPACITY
            echo "New capacity requested to be programmed: $NEW_CAPACITY mAh"
	    sleep 2
            if ./bq2040_capacity -u $NEW_CAPACITY; then
               echo
               echo "The return code from bq2040_capacity signals success."
               sleep 3
               clear
               cat < discharge_warning.txt
            else
               echo
               echo "The return code from bq2040_capacity signals failure."
               echo "Please note the error messages for debugging purposes."
            fi
	    echo
	    echo "Press ENTER to return to the menu."
	    read
	 else
           echo "Okay, giving up."
           sleep 2
         fi
      ;;
      "q")
      ;;
   esac

}



eeprom_stuff() {
   # download contents of a 24C02 into a file
   clear
   echo "This doesn't work using the external port of the battery - you"
   echo "must wiretap the 24Cxx chip directly!!!"
   echo
   echo "Disconnect the cells from the gas-gauge PCB."
   echo "Tap your SMBC (SCL), SMBD (SDA), +5V (from keyboard plug) and GND"
   echo "to the respective pins of the onboard CMOS Flash chip (perhaps a 24C02)."
   cat < schematic.txt
   echo "Press ENTER when ready."
   read

   echo
   echo -n "Enter the EEPROM chip type (e.g. 24C02) : "
   read EEPROM_TYPE
   echo
   if [ "$EEPROM_TYPE" = "" ]; then
      echo "You have not entered a chip type. Assuming 24C02."
      EEPROM_TYPE="24C02"
      echo
   fi
  
   if ./eeprom -t $EEPROM_TYPE -f /var/battery/CMOS-dump.bin; then
      echo
      echo "The binary dump will be saved in a file at"
      echo "  /var/battery/CMOS-dump.bin"
      echo "where you can view it e.g. using the hex mode of"
      echo "the Midnight Commander (F3, F4)."
      echo "The 'eeprom' util called from this menu can also write an EEPROM"
      echo "image into the chip, e.g. after you have edited the image in 'mc'."
      echo "I want this to be done only by people who know what they're doing,"
      echo "therefore this option is not in the menu. The command to do that is"
      echo "  eeprom -p 1 -f -w <file_to_upload>"
   else
      echo
      echo "Failed to dump the 24Cxx contents."
   fi
   echo
   echo "Press ENTER to continue."
   read
}
  





### Script body

clear
echo "                        This is                       "
echo "                                                      "
echo "         a Smart Battery refurbishment suite          "
echo "                 for BQ2092 and BQ2040                "
echo "     by Frank Rysanek <Frantisek.Rysanek@post.cz>     "
echo
echo " This software is not guaranteed to work!             "
echo " The reset features are not a part of the SBS standard!"
echo " This software may ruin your battery alltogether!     "
echo " Use this buch of hacks at your own risk!             "
echo
echo " This CD works only using an external I2C adapter that"
echo " connects to the parallel port (printer port, LPT1).  "
echo
echo " This is a set of C programs, run from a menu operated"
echo " by a shell script. The operating system where all of "
echo " this is running is Linux. There are six consoles,    "
echo " accessible by Alt+F1 to Alt+F6. Feel free to switch  "
echo " to another console and run whatever you want. There's"
echo " mc and vim installed in the system, along with most  "
echo " of the common unix shell commands you would expect.  "
echo
echo "  Press ENTER to continue. "
read

DONE="0"

while [ "$DONE" = "0" ]; do
   clear

   echo "   Please select one of the following:  (and ENTER)"
   echo
   echo " 1    help                                         "
   echo " 2    BQ2092 stuff                                 "
   echo " 3    BQ2040 stuff                                 "
   echo " 4    dump the standard SBS registers              "
   echo " 5    save contents of the 24Cxx EEPROM to a file  "
   echo " 6    run the Midnight Commander (F10 to quit)     "
   echo " 7    mount a floppy at /mnt/floppy (DOS, ext2, minix)"
   echo " 8    un-mount the floppy from /mnt/floppy         "
   echo
   echo " q    quit"
   echo 
   echo -n "    > "

   read LINE
   case $LINE in
      "1")
         clear
         echo "Press 'q' to Quit from the help."
         sleep 3
         less help.txt
      ;;
      "2")
         bq2092_stuff
      ;;
      "3")
         bq2040_stuff
      ;;
      "4")
         # SBS Smart Battery dump
         clear
	 echo "Connect your SMBC, SMBD and GND to the battery's external port."
	 echo "Press ENTER when ready."
         read
         echo "Press 'q' to Quit from the listing."
         sleep 3
         ./read_batt 2>&1 | less
      ;;
      "5")
         eeprom_stuff
      ;;
      "6")
         mc
      ;;
      "7")
         mount /mnt/floppy
      ;;
      "8")
         umount /mnt/floppy
      ;;
      "q")
         echo
         echo "Bye :)"
         echo
         sleep 1
         exit 0
      ;;
   esac
done

# Whut? We should never get here.
exit 1



