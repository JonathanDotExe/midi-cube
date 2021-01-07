#Rock Pi installation

---
**Note:** These are just notes I made during the installation. A proper tutorial will be made later (hopefully).

---

Used OS: Debian 9 Desktop (Rock PI 4 Model A&B) from here: https://wiki.radxa.com/Rockpi4/downloads

##Deactivate GUI


##Change keyboard layout
sudo apt update
sudo dpkg-reconfigure keyboard-configuration

##Deactive GUI autostart
sudo systemctl set-default multi-user.target

##Create user
sudo useradd midicube
sudo passwd midicube
sudo usermod -a -G audio midicube

##Install dependencies
sudo apt install git
sudo apt install meson ninja-build
sudo apt install build-essentials

sudo apt install libsndfile-dev libsfml-dev librtaudio-dev librtmidi-dev libboost-all-dev libfluidsynth-dev

##Select USB audio card
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


##Install midi-cube
git clone https://github.com/JonathanDotExe/midi-cube
cd midi-cube
mkdir build
meson ./ build
cd build
ninja
cd ..
mkdir data
mkdir data/soundfonts
startx ./build/midi-cube



##Sources
* https://askubuntu.com/questions/434849/change-keyboard-layout-english-uk-on-command-line-to-english-us
* https://unix.stackexchange.com/questions/264393/how-to-disable-x-server-autostart-in-debian-jessie
* https://superuser.com/questions/626606/how-to-make-alsa-pick-a-preferred-sound-device-automatically
