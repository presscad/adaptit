#!/bin/bash
# Usage: virtual-box-setup.sh 
# virtual-box-setup.sh -- adds the VirtualBox repo to the sources.list, 
# downloads and adds the VirtualBox public key, installs VirtualBox-5.1 
# and ensures the host is member of the vboxusers and vboxsf groups.
# Installs the appropriate version of the virtualbox extension pack.
# Author: Bill Martin <bill_martin@sil.org>
# Date: 2017-01-25
# Requires an Internet connection to retrieve VirtualBox keys and use apt-get

# Test for Internet connection
echo -e "GET http://google.com HTTP/1.0\n\n" | nc google.com 80 > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "Internet connection detected."
else
    echo "No Internet connection detected. You must have an Internet connection"
    echo "to use this script. Aborting..."
    exit 1
fi

# Test if 'VirtualBox' itself and/or a 'VirtualBox' VM is currently a running 
# process. If the main VirtualBox program or a VM is currently running there 
# will be both a 'VBoxSVC' process and a 'VirtualBox' process running in the 
# system. If either of those processes are running, tell user to close VirtualBox
# and any running VMs down, and run this script again.
if [ -z "$(pgrep VirtualBox)" ]; then
  echo "VirtualBox is not currently running. Ok to proceed..."
else
  echo "VirtualBox is currently running. Please close down Virtual Box and any"
  echo "VirtualBox guest VMs, then run this script again. Aborting..."
  exit 1
fi

# Setup VirtualBox
echo "Seting up VirtualBox..."

VBOX_PKG_NAME="virtualbox-5.1"
DKMS_PKG_NAME="dkms"
supportedDistIDs="LinuxMint Ubuntu"
supportedCodenames="maya qiana rebecca rafaela rosa sarah precise trusty utopic vivid wily xenial yakkety"
echo -e "\nDetermine if system is LinuxMint or Ubuntu and its Codename"
# Determine whether we are setting up a LinuxMint/Wasta system or a straight Ubuntu system
# The 'lsb_release -is' command returns "LinuxMint" on Mint systems and "Ubuntu" on Ubuntu systems.
distID=`lsb_release -is`
echo "  This system is: $distID"
# Determine what the Codename is of the system
# The 'lsb_release -cs' command returns "maya", "rafaela", "rosa", "sarah", "serena" on Mint LTS systems, 
#   and "precise", "trusty", "utopic", "vivid", "wily", "xenial", "yakkety" on Ubuntu systems"
distCodename=`lsb_release -cs`
echo "  The Codename is: $distCodename"
if echo "$supportedDistIDs" | grep -q "$distID"; then
  echo "$distID is a system supported by this script"
  if echo "$supportedCodenames" | grep -q "$distCodename"; then
    echo "The $distCodename Codename is supported by this script"
  else
    echo "But this script does not support setup on $distID $distCodename"
    echo "Aborting..."
  fi
else
  echo "This script does not support setup on $distID"
  echo "Aborting..."
  exit 1
fi

# Get the current Codename.
case $distCodename in
  "maya")
  distCodename="precise"
  ;;
  "qiana")
  distCodename="trusty"
  ;;
  "rebecca")
  distCodename="trusty"
  ;;
  "rafaela")
  distCodename="trusty"
  ;;
  "rosa")
  distCodename="trusty"
  ;;
  "sarah")
  distCodename="xenial"
  ;;
  "serena")
  distCodename="xenial"
  ;;
esac
echo "  The Modified Codename for Deveopment is: $distCodename"

# whm Note: the add-apt-repository command seems to have differing behaviors on
# different Ubuntu distributions - storing software sources list(s) always in
# sources.list in precise, but in separate *.list in sources.list.d on trusty
# and xenial - at least under Wasta-Linux. Using a bash grep command, as done
# below, should do the job without resulting in duplicates, since it won't change
# anything on Wasta-Linux distributions.
APT_DEB_VB_REPO_URL="deb http://download.virtualbox.org/virtualbox/debian xenial contrib"
# The add-apt-repository command seems to result in duplicates
#sudo add-apt-repository "$APT_DEB_VB_REPO_URL"
# Note: Grep returns 0 if selected lines are found, 1 if selected lines are not found.
# command1 || command2 - Command2 is executed if, and only if, command1 returns 
# a non-zero exit status.
grep -qR "$APT_DEB_VB_REPO_URL" /etc/apt/sources.list* \
  || echo "$APT_DEB_VB_REPO_URL" | sudo tee -a /etc/apt/sources.list

# Ensure Oracle keys for VirtualBox are installed
echo -e "\nEnsuring the VirtualBox keys are installed..."
# Call apt-key add, but only when the key is not already present.
VBKEYURL1="https://www.virtualbox.org/download/oracle_vbox_2016.asc"
VBKEYURL2="https://www.virtualbox.org/download/oracle_vbox.asc"
VBKey=`apt-key list | grep info@virtualbox.org`
if [ -z "$VBKey" ]; then
  echo "The VirtualBox key is NOT installed."
  mkdir -p ~/tmp
  sudo chown $USER:$USER ~/tmp 
  cd ~/tmp
  if [ -f ~/tmp/oracle_vbox_2016.asc ]; then
    echo "Found oracle_vbox_2016.asc already in ~/tmp/ so will use it"
  else
    # Key not in ~/tmp/ so retrieve the key from the external web site
    # Retrieve the old Oracle key and the new key and install both.
    echo "Retrieving the oracle_vbox_2016 key(s) from $VBKEYURL1 to ~/tmp/"
    wget --no-clobber --no-directories $VBKEYURL1
    if [ $? -ne 0 ]
    then
      echo "Unable to download oracle_vbox_2016.asc: wget error: $?"
      exit 1
    fi
    wget --no-clobber --no-directories $VBKEYURL2
    if [ $? -ne 0 ]
    then
      echo "Unable to download oracle_vbox.asc: wget error: $?"
      exit 1
    fi
  fi
  if [ -f ~/tmp/oracle_vbox_2016.asc ]; then
    echo "Installing the oracle_vbox_2016.asc key..."
    sudo apt-key add ~/tmp/oracle_vbox_2016.asc
  else
    echo "The VirtualBox 2016 key could not be retrieved from the website."
    echo "You will need to download and install the VirtualBox key later with:"
    echo "  sudo apt-key add <path>/oracle_vbox_2016.asc"
  fi
  if [ -f ~/tmp/oracle_vbox.asc ]; then
    echo "Installing the older oracle_vbox key.asc key..."
    sudo apt-key add ~/tmp/oracle_vbox.asc
  else
    echo "The older VirtualBox key could not be retrieved from the website."
    echo "You will need to download and install the VirtualBox key later with:"
    echo "  sudo apt-key add <path>/oracle_vbox.asc"
  fi
else
  echo "The VirtualBox key is already installed."
fi

# Refresh the sources list
sudo apt-get update

# When next LTS after xenial arrives we need to include it in test below
if [ "$distCodename" = "xenial" ]; then
  echo -e "\nInstalling VirtualBox $VBOX_PKG_NAME and $DKMS_PKG_NAME for $distCodename..."
  sudo apt-get install $VBOX_PKG_NAME $DKMS_PKG_NAME -y
  LASTERRORLEVEL=$?
  if [ $LASTERRORLEVEL != 0 ]; then
     echo -e "\nCould not install VirtualBox $VBOX_PKG_NAME and $DKMS_PKG_NAME."
     echo "Make sure that no other package manager is running and try again."
     return $LASTERRORLEVEL
  else
     echo -e "\nInstallation of $VBOX_PKG_NAME and $DKMS_PKG_NAME completed."
  fi
else
  echo -e "\nThis script currently only supports istalling VirtualBox 5.x to xenial systems..."
  echo "Aborting..."
  exit 1
fi

# For VirtualBox and its guest VMs to be able to access USB ports and access 
# shared folders the host machine must have two groups vboxusers and vboxsf. 
# The installer for VirtualBox generally takes care of creating the vboxusers 
# group on the host machine. But it is up to the user to add the user to add 
# himself to the vboxusers and vboxsf groups on the host machine, and also 
# (from within the VM) to add the VM's username to the vboxsf group.
# Here in this virtual-box-setup.sh script we can at least ensure that the 
# necessary groups are created on the host machine, and then at least remind 
# the user to add himself to the vboxsf group from within the running VM using: 
#    sudo usermod -a -G vboxsf $USER

# Add current user to the vboxuser and vboxsf groups
echo -e "\nAdding USER $USER to the vboxusers group..."
sudo usermod -a -G vboxusers $USER
LASTERRORLEVEL=$?
if [ $LASTERRORLEVEL != 0 ]; then
  echo -e "\nWARNING: Could not add user: $USER to the vboxusers group."
  exit $LASTERRORLEVEL
fi
echo "User $USER successfully added to the vboxusers group."
# When VirtualBox is installed, it automatically adds a vboxusers group
# If the user hasn't actually shared any folders in a VM, there won't yet
# be a vboxsf group, so we can add a vboxsf group and ensure that the user
# is also a member of that group here.
echo "Adding the vboxsf group if it doesn't already exist..."
sudo addgroup vboxsf
# If vboxsf already exists, the addgroup command above will say:
# "addgroup: The group 'vboxsf' already exists"
echo "Adding USER $USER to the vboxsf group..."
sudo usermod -a -G vboxsf $USER
LASTERRORLEVEL=$?
if [ $LASTERRORLEVEL != 0 ]; then
  echo -e "\nWARNING: Could not add user: $USER to the vboxsf group."
  exit $LASTERRORLEVEL
fi
echo -e "\nUser $USER successfully added to the vboxsf group on this host machine."

# Note: We can't really add the Guest Additions module as it must be done
# from within an existing virtual machine (VM). This script doesn't create any
# virtual machine guest(s) - a job left to the user/developer.

# Install the virtualbox extension pack which is needed for multiple monitor support.
# Once VirtualBox is installed, calling VBoxManage --version returns the 
# version and revision number of the installed virtualbox in the
# form: 5.1.14r112924
# We can parse out the version number (5.1.14) with sed and compose the name of the
# extensionpack that we need to download from download.virtualbox.org/virtualbox/
# The name of the extension pack filename is in the form:
# Oracle_VM_VirtualBox_Extension_Pack-5.1.14.vbox-extpack
# which the download site stores in a .../5.1.14/... folder so that the
# download URL using wget would be:
# http://download.virtualbox.org/virtualbox/5.1.14/Oracle_VM_VirtualBox_Extension_Pack-5.1.14.vbox-extpack
# The command to install the extension pack using VBoxManage from command line is:
# VBoxManage extpack install --replace <name of extension pack file>
VBOXVER=`VBoxManage --version | sed -r 's/([0-9])\.([0-9])\.([0-9]{1,2}).*/\1.\2.\3/'`
echo "The installed VirtualBox version is: $VBOXVER"
# Save current working dir
CURDIR=$(pwd)
# Check if the extpack already exists in ~/Downloads
cd ~/Downloads
if [ -f "Oracle_VM_VirtualBox_Extension_Pack-$VBOXVER.vbox-extpack" ]; then
  echo "The extension pack for version $VBOXVER already exists in Downloads, using it..."
else
  echo -e "\nDownloading the VirtualBox Extension Pack..."
  wget -N "http://download.virtualbox.org/virtualbox/$VBOXVER/Oracle_VM_VirtualBox_Extension_Pack-$VBOXVER.vbox-extpack"
fi
echo "Installing the VirtualBox Extension Pack..."
VBoxManage extpack install --replace Oracle_VM_VirtualBox_Extension_Pack-$VBOXVER.vbox-extpack

cd $pwd

echo -e "\nThe virtual-box-setup.sh script completed successfully if no errors"
echo "   were reported above."
echo -e "\nREMINDERS: "
echo "   1. You should add the Guest Additions from within each VM guest you create"
echo "      by selecting the VM's Devices menu and selecting the menu item:"
echo "      'Insert Guest Additions CD image...'"
echo "   2. You should add the VM's user to the vboxsf group from within each VM."
echo "      You can do so from within the VM by running this command in a Terminal:"
echo "      sudo usermod -a -G vboxsf \$USER"



