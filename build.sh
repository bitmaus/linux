#!/bin/sh

OPTIND=1

drive=""
user=""
password=""
root=""

packages='base base-devel syslinux'
packages+=' xorg-server xorg-xrandr xorg-xinit mesa xf86-video-intel xterm fluxbox'
#packages+=' sudo vim plasma-meta dolphin kdialog kfind konsole dolphin-plugins kwallet kwalletmanager'
packages+=' sudo vim git dialog wpa_supplicant openssh'
packages+=' wireless_tools networkmanager network-manager-applet parted dosfstools'
packages+=' firefox blender gimp docky jdk8-openjdk virtualbox'
#include 0ad or qemu?

specials='visual-studio-code-bin gnuchess'

while getopts "d:u:p:r:ab" opt; do
    case "$opt" in
    a)
		source
		exit 0
		;;
    b)
		target
		exit 0
		;;
    d)
        drive=$OPTARG
        ;;
    u)  user=$OPTARG
        ;;
    p)  password=$OPTARG
        ;;
    r)  root=$OPTARG
	;;
    esac
done

source() {
	parted -s "$drive" \
		mklabel msdos \
		mkpart primary ext2 1 100M \
		set 1 boot on \
		mkpart primary ext4 100M 100% \
		set 2 LVM on
	
	pvcreate /dev/"$drive"
	vgcreate vg0 /dev/"$drive"
	lvcreate -C y -L8G vg0 -n swap
	lvcreate -l +100%FREE vg0 -n root
	vgchange -ay

	mkfs.ext2 -L boot "$drive"1
	mkfs.ext4 -L root /dev/vg0/root
	mkswap /dev/vg0/swap
	
	mount /dev/vg0/root /mnt
	mkdir /mnt/boot
	mount "$drive"1 /mnt/boot
	swapon /dev/vg0/swap

	pacstrap /mnt base base-devel
	pacstrap /mnt syslinux
	
	genfstab -pU /mnt >> /mnt/etc/fstab
	echo 'tmpfs    /tmp    tmpfs    defaults,noatime,mode=1777    0    0' >> /mnt/etc/fstab
	
	cp /mnt/etc/mkinitcpio.conf /mnt/etc/mkinitcpio.conf.bak
	cp /tranport/mkinitcpio.conf /mnt/etc/mkinitcpio.conf
	
	cp /mnt/boot/syslinux/syslinux.cfg /mnt/boot/syslinux/syslinux.cfg.bak
	cp /transport/syslinux.cfg /mnt/boot/syslinux/syslinux.cfg
	cp /transport/splash.png /mnt/boot/syslinux/splash.png

	cp $0 /mnt/arch.sh
	arch-chroot /mnt ./arch.sh -d $drive -u $user -p $password -r $root -b

	umount /mnt/boot
	umount /mnt
	swapoff /dev/mapper/vg0-swap
	vgchange -an

	#reboot
}

target() {
	pacman -Sy --noconfirm packages

	mkdir /temp
	cd /temp
	curl https://aur.archlinux.org/packages/pa/packer/packer.tar.gz | tar xzf -
	cd packer
	makepkg -si --noconfirm --asroot

	cd ..
	export TMPDIR=/temp
	packer -S --noconfirm specials
	unset TMPDIR
	rm -rf /temp

	echo "bolt" > /etc/hostname
	ln -sT "/usr/share/zoneinfo/America/Los_Angeles" /etc/localtime
	hwclock --systohc --utc
	echo 'LANG="en_US.UTF-8"' >> /etc/locale.conf
	echo 'LC_COLLATE="C"' >> /etc/locale.conf
	echo 'LANGUAGE="en_US"' >> /etc/locale.conf
	echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen
	locale-gen

	mkinitcpio -p linux

	systemctl enable sddm
	systemctl enable NetworkManager.service
	systemctl enable wpa_supplicant.service

	syslinux-install_update -iam

	useradd -m -g users -G wheel "$user"
	echo -en "$password\n$password" | passwd "$user"

	echo -en "$root\n$root" | passwd

	userdel alarm # (-r?)

	#nano /etc/sudoers (uncomment %wheel ALL=(ALL) ALL

	rm /arch.sh

	#exit

	#(lets encrypt for SSL key here...), Install the certbot package.
	#Request a certificate for domain.tld using /path/to/domain.tld/html/ as public accessible path:
	# certbot certonly --email email@example.com --webroot -w /path/to/domain.tld/html/ -d domain.tld
	#To add a (sub)domain, include all registered domains used on the current setup:
	# certbot certonly --email email@example.com --webroot -w /path/to/sub.domain.tld/html/ -d domain.tld,sub.domain.tld
	#To renew (all) the current certificate(s):
	# certbot renew
	# openssl req -new -x509 -nodes -newkey rsa:4096 -keyout server.key -out server.crt -days 1095
	# chmod 400 server.key
	# chmod 444 server.crt

	#end with game of chess (or 0 A.D.)
	#remove Arch packages using `pacman -Rns <name>`

	echo 'Enter a passphrase to encrypt the disk:'
            stty -echo
            read DRIVE_PASSPHRASE
       stty echo

	   encrypt_drive "$lvm_dev" "$DRIVE_PASSPHRASE" lvm

	   echo -en "$passphrase" | cryptsetup -c aes-xts-plain -y -s 512 luksFormat "$dev"
echo -en "$passphrase" | cryptsetup luksOpen "$dev" lvm

crypt="cryptdevice=/dev/disk/by-uuid/$lvm_uuid:lvm"

ABEL arch
	MENU LABEL Arch Linux
	LINUX ../vmlinuz-linux
	APPEND root=/dev/vg00/root ro $crypt resume=/dev/vg00/swap quiet
	INITRD ../initramfs-linux.img
LABEL archfallback
	MENU LABEL Arch Linux Fallback
	LINUX ../vmlinuz-linux
	APPEND root=/dev/vg00/root ro $crypt resume=/dev/vg00/swap
	INITRD ../initramfs-linux-fallback.img

	local encrypt=""
    if [ -n "$ENCRYPT_DRIVE" ]
    then
        encrypt="encrypt"
fi

HOOKS="base udev autodetect modconf block keymap keyboard $encrypt lvm2 resume filesystems fsck"

}

set -ex

...from old arch script...



#!/bin/sh

OPTIND=1

drive=""
user=""
password=""
root=""

packages='base base-devel syslinux'
packages+=' xorg-server xorg-xrandr xorg-xinit mesa xf86-video-intel xterm fluxbox'
#packages+=' sudo vim plasma-meta dolphin kdialog kfind konsole dolphin-plugins kwallet kwalletmanager'
packages+=' sudo vim git dialog wpa_supplicant'
packages+=' wireless_tools networkmanager network-manager-applet'
packages+=' firefox blender gimp docky jdk8-openjdk virtualbox'
#include 0ad or qemu?

specials='visual-studio-code-bin gnuchess'

while getopts "d:u:p:r:ab" opt; do
    case "$opt" in
    a)
		source
		exit 0
		;;
    b)
		target
		exit 0
		;;
    d)
        drive=$OPTARG
        ;;
    u)  user=$OPTARG
        ;;
    p)  password=$OPTARG
        ;;
    r)  root=$OPTARG
	;;
    esac
done

source() {
	parted -s "$drive" \
		mklabel msdos \
		mkpart primary ext2 1 100M \
		set 1 boot on \
		mkpart primary ext4 100M 100% \
		set 2 LVM on
	
	pvcreate /dev/"$drive"
	vgcreate vg0 /dev/"$drive"
	lvcreate -C y -L8G vg0 -n swap
	lvcreate -l +100%FREE vg0 -n root
	vgchange -ay

	mkfs.ext2 -L boot "$drive"1
	mkfs.ext4 -L root /dev/vg0/root
	mkswap /dev/vg0/swap
	
	mount /dev/vg0/root /mnt
	mkdir /mnt/boot
	mount "$drive"1 /mnt/boot
	swapon /dev/vg0/swap

	pacstrap /mnt base base-devel
	pacstrap /mnt syslinux
	
	genfstab -pU /mnt >> /mnt/etc/fstab
	echo 'tmpfs    /tmp    tmpfs    defaults,noatime,mode=1777    0    0' >> /mnt/etc/fstab
	
	cp /mnt/etc/mkinitcpio.conf /mnt/etc/mkinitcpio.conf.bak
	cp /tranport/mkinitcpio.conf /mnt/etc/mkinitcpio.conf
	
	cp /mnt/boot/syslinux/syslinux.cfg /mnt/boot/syslinux/syslinux.cfg.bak
	cp /transport/syslinux.cfg /mnt/boot/syslinux/syslinux.cfg
	cp /transport/splash.png /mnt/boot/syslinux/splash.png

	cp $0 /mnt/arch.sh
	arch-chroot /mnt ./arch.sh -d $drive -u $user -p $password -r $root -b

	umount /mnt/boot
	umount /mnt
	swapoff /dev/mapper/vg0-swap
	vgchange -an

	#reboot
}

target() {
	pacman -Sy --noconfirm packages

	mkdir /temp
	cd /temp
	curl https://aur.archlinux.org/packages/pa/packer/packer.tar.gz | tar xzf -
	cd packer
	makepkg -si --noconfirm --asroot

	cd ..
	export TMPDIR=/temp
	packer -S --noconfirm specials
	unset TMPDIR
	rm -rf /temp

	echo "bolt" > /etc/hostname
	ln -sT "/usr/share/zoneinfo/America/Los_Angeles" /etc/localtime
	hwclock --systohc --utc
	echo 'LANG="en_US.UTF-8"' >> /etc/locale.conf
	echo 'LC_COLLATE="C"' >> /etc/locale.conf
	echo 'LANGUAGE="en_US"' >> /etc/locale.conf
	echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen
	locale-gen

	mkinitcpio -p linux

	systemctl enable sddm
	systemctl enable NetworkManager.service
	systemctl enable wpa_supplicant.service

	syslinux-install_update -iam

	useradd -m -g users -G wheel "$user"
	echo -en "$password\n$password" | passwd "$user"

	echo -en "$root\n$root" | passwd

	#nano /etc/sudoers (uncomment %wheel ALL=(ALL) ALL

	rm /arch.sh

	#exit
}

set -ex


...network script...

#!/bin/bash
#
# This version uses September 2017 august stretch image, please use this image
#

if [ "$EUID" -ne 0 ]
	then echo "Must be root"
	exit
fi

if [[ $# -lt 1 ]]; 
	then echo "You need to pass a password!"
	echo "Usage:"
	echo "sudo $0 yourChosenPassword [apName]"
	exit
fi

APPASS="$1"
APSSID="rPi3"

if [[ $# -eq 2 ]]; then
	APSSID=$2
fi

apt-get remove --purge hostapd -yqq
apt-get update -yqq
apt-get upgrade -yqq
apt-get install hostapd dnsmasq -yqq

cat > /etc/dnsmasq.conf <<EOF
interface=wlan0
dhcp-range=10.0.0.2,10.0.0.5,255.255.255.0,12h
EOF

cat > /etc/hostapd/hostapd.conf <<EOF
interface=wlan0
hw_mode=g
channel=10
auth_algs=1
wpa=2
wpa_key_mgmt=WPA-PSK
wpa_pairwise=CCMP
rsn_pairwise=CCMP
wpa_passphrase=$APPASS
ssid=$APSSID
ieee80211n=1
wmm_enabled=1
ht_capab=[HT40][SHORT-GI-20][DSSS_CCK-40]
EOF

sed -i -- 's/allow-hotplug wlan0//g' /etc/network/interfaces
sed -i -- 's/iface wlan0 inet manual//g' /etc/network/interfaces
sed -i -- 's/    wpa-conf \/etc\/wpa_supplicant\/wpa_supplicant.conf//g' /etc/network/interfaces
sed -i -- 's/#DAEMON_CONF=""/DAEMON_CONF="\/etc\/hostapd\/hostapd.conf"/g' /etc/default/hostapd

cat >> /etc/network/interfaces <<EOF
# Added by rPi Access Point Setup
allow-hotplug wlan0
iface wlan0 inet static
	address 10.0.0.1
	netmask 255.255.255.0
	network 10.0.0.0
	broadcast 10.0.0.255

EOF

echo "denyinterfaces wlan0" >> /etc/dhcpcd.conf

systemctl enable hostapd
systemctl enable dnsmasq

sudo service hostapd start
sudo service dnsmasq start

echo "All done! Please reboot"


...format script...

parted -s /dev/sda \
		mklabel msdos \
		mkpart primary ext2 1 100M \
		set 1 boot on \
		mkpart primary ext4 100M 100% \
		set 2 LVM on
	
	pvcreate /dev/sda
	vgcreate vg0 /dev/sda
	lvcreate -C y -L8G vg0 -n swap
	lvcreate -l +100%FREE vg0 -n root
	vgchange -ay