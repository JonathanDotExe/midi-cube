# Raspberry Pi installation

---
**Note:** These are just notes I made during the installation. A proper tutorial will be made later (hopefully).

---

Used OS: Raspberry Pi OS Lite

## Deactivate GUI


## Change keyboard layout
sudo apt update
sudo dpkg-reconfigure keyboard-configuration

## Create user
sudo useradd midicube
sudo passwd midicube
sudo usermod -a -G audio midicube

sudo mkhomedir_helper

## Install dependencies
sudo apt install git
sudo apt install meson ninja-build

sudo apt install libsndfile-dev libsfml-dev librtaudio-dev librtmidi-dev libboost-all-dev libfluidsynth-dev

## Select USB audio card
aplay -l
sudo nano /etc/asound.conf

pcm.!default {
    type hw
    card 2
}

ctl.!default {
    type hw
    card 2
}


## Screen resolution
Add to /boot/config.txt

disable_overscan=1

hdmi_cvt=1024 600 60 3 0 0 0
hdmi_group=2
hdmi_mode=87

## Install midi-cube
git clone https://github.com/JonathanDotExe/midi-cube
cd midi-cube
mkdir build
meson ./ build
cd build
ninja
cd ..
mkdir data
mkdir data/soundfonts

# Make script and start
nano start-midi-cube.sh

./build/midi-cube [number-of-device]

startx ./build/midi-cube > midicube_log.txt

chmod +rwx start-midicube.sh
startx ./start-midicube.sh



## Sources
* https://askubuntu.com/questions/434849/change-keyboard-layout-english-uk-on-command-line-to-english-us
* https://unix.stackexchange.com/questions/264393/how-to-disable-x-server-autostart-in-debian-jessie
* https://superuser.com/questions/626606/how-to-make-alsa-pick-a-preferred-sound-device-automatically
* https://askubuntu.com/questions/335961/create-default-home-directory-for-existing-user-in-terminal
* https://www.raspberrypi.org/forums/viewtopic.php?t=14914
