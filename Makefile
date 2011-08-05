all:
	make -C bundles
	echo "Copying payloads to KINGSTON usb..."
	mv -f bundles/AppleTV2,1/8C154/iBoot/k66ap.iboot.payload /media/KINGSTON/k66ap.iboot.payload
	mv -f bundles/iPad1,1/8C148/iBoot/k48ap.iboot.payload /media/KINGSTON/k48ap.iboot.payload
	mv -f bundles/iPhone2,1/8C148/iBoot/n88ap.iboot.payload /media/KINGSTON/n88ap.iboot.payload
	mv -f bundles/iPhone3,1/8C148/iBoot/n90ap.iboot.payload /media/KINGSTON/n90ap.iboot.payload
	mv -f bundles/iPhone3,3/8E200/iBoot/n92ap.iboot.payload /media/KINGSTON/n92ap.iboot.payload
	mv -f bundles/iPod2,1/8C148/iBoot/n72ap.iboot.payload /media/KINGSTON/n72ap.iboot.payload
	mv -f bundles/iPod3,1/8C148/iBoot/n18ap.iboot.payload /media/KINGSTON/n18ap.iboot.payload
	mv -f bundles/iPod4,1/8C148/iBoot/n81ap.iboot.payload /media/KINGSTON/n81ap.iboot.payload
	echo "Done! :)"
clean:
	make clean -C bundles
