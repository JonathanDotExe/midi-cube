#Rock Pi installation

---
**Note:** These are just notes I made during the installation. A proper tutorial will be made later (hopefully).

---

Used OS: Armbian Focal for Rock Pi 4B

##Update
sudo apt update
sudo apt upgrade

##Change keyboard layout
sudo dpkg-reconfigure keyboard-configuration

##Change console resolution
sudo nano /boot/boot.cmd

disp.screen0_output_mode=1024x600p60

sudo mkimage -C none -A arm -T script -d /boot/boot.cmd /boot/boot.scr

or

sudo dpkg-reconfigure console-setup

##Install Jack
sudo apt install jackd2 
Allow realtime priority
sudo apt install a2jmidid

###Jack start commands


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
meson ./ build
cd build
meson configure --buildtype release
ninja
cd ..
mkdir data
mkdir data/soundfonts

#Make script and start
nano start.sh

./build/midi-cube \[number-of-output-device\]  \[number-of-input-device\] > midicube_log.txt 2>&1

chmod +x start-midicube.sh
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
nano wait.py

```Python

import signal

def handler(signum, frame):
        print('Starting MIDICube')
        exit(0)

signal.signal(signal.SIGALRM, handler)
signal.alarm(5)

print('Starting MIDICube in 5 seconds ...')
input('Press ENTER key to interrupt')
signal.alarm(0)
print('Interrupted')
exit(1)


```

nano .profile

```
if [ $(tty) = "/dev/tty1" ] ; then
    cd /home/midicube/midi-cube
    python3 wait.py && startx ./start.sh && shutdown 0
fi

```

##Sources
* https://docs.armbian.com/User-Guide_Fine-Tuning/
* https://wiki.radxa.com/Rockpi4/FAQs#Radxa_APT_public_key_is_not_available
* https://askubuntu.com/questions/434849/change-keyboard-layout-english-uk-on-command-line-to-english-us
* https://askubuntu.com/questions/335961/create-default-home-directory-for-existing-user-in-terminal
* https://wiki.ubuntuusers.de/Autologin/
* https://www.youtube.com/watch?v=ae5TYhnhp3w&feature=emb_title
