Lb - Browser in Less
====================
Build Dependencies: coreutils, util-linux, less  
Runtime Dependencies: python3, less (with command support), curl

## Building

$ `make`

## Installation

Add the following lines to your shell's rc file:  

Linux:  
`alias lb='cd ~/.lb/pages; touch {1..32}; LESSKEY=~/.lb/less less -XR {1..32}'`  
`export PATH=~/bin:"$PATH"`

For performance and security, you can add the following line to your /etc/fstab file:

Linux:  
`tmpfs $HOME/.lb/pages tmpfs uid=$USER,gid=$USER,mode=700,noatime,nodev,nosuid,noexec 0 0`

This will store web pages in memory, prevent updates to access times, and set strict access control permissions.

Uninstalling
------------
`$ make uninstall`

Work In Progress
----------------
- Rewrite in flex/bison for portability and correctness

Bugs
----
Please send issues to the email seL4 (plus) lb (at) disroot.org.
