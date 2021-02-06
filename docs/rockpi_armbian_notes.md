#Rock Pi installation

---
**Note:** These are just notes I made during the installation. A proper tutorial will be made later (hopefully).

---

Used OS: Armbian Focal with legacy kernel (4.4) for Rock Pi 4B

##Update
sudo apt update
sudo apt upgrade

##Install xorg
sudo apt install xorg

##Change keyboard layout
sudo dpkg-reconfigure keyboard-configuration

##Install dependencies
sudo apt install git
sudo apt install meson ninja-build

sudo apt install libsndfile-dev libsfml-dev librtaudio-dev librtmidi-dev libboost-all-dev libfluidsynth-dev

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

##Make script and start
nano start.sh

```bash
./build/midi-cube \[number-of-output-device\]  \[number-of-input-device\] > midicube_log.txt 2>&1
```

chmod +x start.sh
startx ./start.sh

##Optimize CPU frequency (optional)
sudo armbian-config
System > CPU > minimum frequency 1.4 GHz > maximum frequency 1.8 GHz > governor: performance

##Sources
* https://docs.armbian.com/User-Guide_Fine-Tuning/
* https://askubuntu.com/questions/434849/change-keyboard-layout-english-uk-on-command-line-to-english-us
* https://wiki.ubuntuusers.de/Autologin/
* https://www.youtube.com/watch?v=ae5TYhnhp3w&feature=emb_title
