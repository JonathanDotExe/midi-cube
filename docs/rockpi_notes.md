#Rock Pi installation

---
**Note:** These are just notes I made during the installation. A proper tutorial will be made later (hopefully).

---

Used OS: Ubuntu Server 18 for Rock Pi 4B

##Update apt repository
sudo apt-get install wget
export DISTRO=buster-stable
wget -O - apt.radxa.com/$DISTRO/public.key | sudo apt-key add -
sudo apt update
sudo apt upgrade

##Change keyboard layout
sudo apt install keyboard-configuration
sudo dpkg-reconfigure keyboard-configuration

##Create user
sudo useradd midicube
sudo passwd midicube
sudo usermod -a -G audio midicube

sudo mkhomedir_helper midicube

##Install dependencies
sudo apt install git
sudo apt install meson ninja-build

sudo apt install libsndfile-dev libsfml-dev librtaudio-dev librtmidi-dev libboost-all-dev libfluidsynth-dev

##Install xorg
sudo apt install xorg

##Install midi-cube
git clone https://github.com/JonathanDotExe/midi-cube
cd midi-cube
mkdir build
meson --buildtype release ./ build
cd build
ninja
cd ..
mkdir data
mkdir data/soundfonts

#Make script and start
nano start-midi-cube.sh

./build/midi-cube [number-of-output-device] [number-of-input-device] > midicube_log.txt

chmod +rwx start-midicube.sh
startx ./start-midicube.sh

##Sources
* https://wiki.radxa.com/Rockpi4/FAQs#Radxa_APT_public_key_is_not_available
* https://askubuntu.com/questions/434849/change-keyboard-layout-english-uk-on-command-line-to-english-us
* https://askubuntu.com/questions/335961/create-default-home-directory-for-existing-user-in-terminal
