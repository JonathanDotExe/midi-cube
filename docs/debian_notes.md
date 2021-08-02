#Rock Pi installation

---
**Note:** These are just notes I made during the installation. A proper tutorial will be made later (hopefully).

---

Used OS: Debian (without Desktop + SHH)

##Update apt repository
apt install sudo (as root)
usermod -a -G sudo midicube (as root)
sudo apt update
sudo apt upgrade

## Install dependencies
sudo apt install git
sudo apt install meson ninja-build g++

sudo apt install libsndfile-dev libsfml-dev librtaudio-dev librtmidi-dev libboost-all-dev libfluidsynth-dev portaudio19-dev

## Install xorg
sudo apt install xinit

## Install midi-cube
git clone https://github.com/JonathanDotExe/midi-cube
cd midi-cube
mkdir build
meson --buildtype release ./ build
cd build
ninja
cd ..
mkdir data
mkdir data/soundfonts

## Make script
nano start.sh

```bash
./build/midi-cube \[number-of-output-device\]  \[number-of-input-device\] > midicube_log.txt 2>&1
```

chmod +x start.sh

## Autologin

sudo mkdir /etc/systemd/system/getty@tty1.service.d
sudo nano /etc/systemd/system/getty@tty1.service.d/override.conf

```
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin midicube --noclear %I $TERM
Type=idle
```

## Start MIDICube after login
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

## Performance governor
sudo linux-cpupower
sudo cpupower frequency-set -g performance

##Sources
* https://wiki.ubuntuusers.de/Autologin/
* https://www.youtube.com/watch?v=ae5TYhnhp3w&feature=emb_title
* https://wiki.debian.org/CpuFrequencyScaling
