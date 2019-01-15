#!/bin/bash
# run it using bash command like this:
# 32bit: sudo bash linux-package-exe.sh 32
# 64bit: sudo bash linux-package-exe.sh 64
echo "... timestamp ..."
echo $(date) $( echo "Revision :" ) $(sudo git rev-list --count HEAD ) > ${PWD}/../../bin/data/about/version.txt
echo "... data cleanup ..."
sudo rm -f -r ${PWD}/../../bin/data/state/*.*
echo "... tmp folder ..."
sudo rm -f -r ${PWD}/../tmp
sudo mkdir    ${PWD}/../tmp
sudo cp -r -f ${PWD}/../scopefun-linux ${PWD}/../tmp
echo "... DEBIAN/control ..."
sudo rm -f ${PWD}/../tmp/scopefun-linux/DEBIAN/control32
sudo rm -f ${PWD}/../tmp/scopefun-linux/DEBIAN/control64
sudo cp -f    ${PWD}/../scopefun-linux/DEBIAN/control$1 ${PWD}/../tmp/scopefun-linux/DEBIAN/control
echo "... copy data files ..."
sudo cp -r -f ${PWD}/../../bin/data   ${PWD}/../tmp/scopefun-linux/usr/lib/oscilloscope
echo "... copy locale ..."
sudo cp -r -f ${PWD}/../../bin/locale/ ${PWD}/../tmp/scopefun-linux/usr/share/
echo "... copy exe ..."
sudo mkdir ${PWD}/../tmp/scopefun-linux/usr/bin
sudo cp -r ${PWD}/../../bin/oscilloscope$1r ${PWD}/../tmp/scopefun-linux/usr/bin/oscilloscope
echo "... gzip compress changelog ..."
sudo gzip --best ${PWD}/../tmp/scopefun-linux/usr/share/doc/scopefun-linux/changelog
echo "... attributes ..."
function attributes() {
cd ${PWD}/../tmp
sudo find ./scopefun-linux -type d -print0 | xargs -0 sudo chmod 755 
sudo find ./scopefun-linux -type f -print0 | xargs -0 sudo chmod 644 
sudo chmod +x ./scopefun-linux/lib/udev/rules.d/51-oscilloscope.rules
sudo chmod +x ./scopefun-linux/usr/bin/oscilloscope
sudo chown -R root: ./scopefun-linux
}
attributes
echo "... package ..." 
function package() {
cd ${PWD}/../tmp
sudo fakeroot dpkg-deb --build scopefun-linux
}
package
echo "... copy .deb..."
sudo rm -f -r ${PWD}/../scopefun-linux$1.deb
sudo cp -f    ${PWD}/scopefun-linux.deb ${PWD}/../scopefun-linux$1.deb
echo "... remove tmp..."
sudo rm -f -r ${PWD}/../tmp/
echo "... verify ..."
function verify() {
cd ..
lintian scopefun-linux$1.deb
}
verify
echo "... finished ..."
	
