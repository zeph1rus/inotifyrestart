# inotifyrestart

This is a tool to restart a systemd unit if a file (usually a symlink) is updated.  The main use case is to restart jvm depenedent services when the jre is patched (ie. the symlink to the current jdk is changed)

## Usage

```
inrestart -p <path> -f <filename> -s <servicename>
-p <path> - Path to director to watch
-f <filename> - filename of file in directory to watch
-s <servicename> Systemd service name of service to restart (include.service, e.g teamcity.service)
```

*Note - this tool must run as uid0 (root), it is best to run it as a systemd unit*

## Installation

### Prerequisites

Firstly install the core developer tools (gcc/make etc), plus systemd headers

#### Centos/Fedora/RHEL

```sudo yum group install "Development Tools"```

```sudo yum install systemd-devel```

#### Debian/Ubuntu/Oracle

```sudo apt install build-essential```

```sudo apt install libsystemd-dev```

#### Install Manually

You will need to install the appropriate systemd header pacakage, and also ```gcc```, ```make```, and ```pkg-config```

### Clone and Build

Clone this repository somwhere on the system, any user directory is fine. Do the following commands

```make clean```

```make```

```sudo make install```

This will build the tool and copy it to /usr/local/bin

## Installing as a service

You can either edit the make file and change the parameters for the service, or pass them to make on the command line. Either of these solutions will see a service installed into systemd with the parameters and enabled (*but not started!*)

### Editing Parameters

Open ```Makefile``` in a text editor, and edit the following values:

```
SERVICENAME=teamcity.service
FPATH=/etc/alternatives
FFILE=jre
```

and then run ```sudo make serviceinstall```

### Passing in the command line

Run the following, changing the parameters as required

```sudo make FPATH=/etc/alternative FFILE=jre SERVICENAME=teamcity.service serviceinstall```
