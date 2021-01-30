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

./build/midi-cube \[number-of-output-device\]  \[number-of-input-device\] > midicube_log.txt

chmod +rwx start-midicube.sh
startx ./start-midicube.sh

##Autologin

sudo mkdir /etc/systemd/system/getty@tty1.service.d
sudo nano /etc/systemd/system/getty@tty1.service.d/override.conf

```
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin midicube --noclear %I $TERM
Type=idle
```

##Start MIDICube after login
nano .profile

```
if [ $(tty) = "/dev/tty1" ] ; then
    cd /home/midicube/midi-cube
    startx ./start.sh
    shutdown 0
fi

```

##Sources
* https://wiki.radxa.com/Rockpi4/FAQs#Radxa_APT_public_key_is_not_available
* https://askubuntu.com/questions/434849/change-keyboard-layout-english-uk-on-command-line-to-english-us
* https://askubuntu.com/questions/335961/create-default-home-directory-for-existing-user-in-terminal
* https://wiki.ubuntuusers.de/Autologin/
* https://www.youtube.com/watch?v=ae5TYhnhp3w&feature=emb_title
