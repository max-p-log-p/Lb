Lb - Browser in Less
====================

Why should I use this browser?
------------------------------

This browser has many capabilities due to its extensibility: the less interface is reused to support a text based web browser. It has limited support for javascript. It is also able to search for a pattern through all of history or through all tabs.

Basic Features
--------------
- Request a URL - w <url>
- Click link (get) - l <link number>
- Send a form (post) - p <link number>
- Tabs - f and b
- Go Back in History - b
- Go Forward in History - f
- Search forwards (same file) - / 
- Search forwards (all files) - ESC-/ 
- Search backwards (same file) - ?
- Search backwards (all files) - ESC-?
- Copy - tmux copy-mode
- Delete cookie - Remove cookie in cookie file
- Get raw html - c <url>
- Javascript - Get raw html, interpret manually and edit html, reparse with %!lb
- Inspect Element - Get raw html, edit html, reparse with %!lb
- Windows - open in a new window with tmux
- PDF, Images, Video, etc. - Install an application that handles the file format and reads from standard input. Pipe the output to the application.
- History - ~/.lesshst
- Bookmarks - Maintain a bookmarks file with links
- Hide Password - Cover with physical object.
- Debugging - add --trace
- File Upload - Use -F for each parameter in the file. See the section Links.

For more features read the less manpage.

## Installation
Build Dependencies: coreutils, util-linux, less  
Runtime Dependencies: python3, less (with command support), curl

## Building

$ `make`

## Configuration

Add the following lines to your shell's rc file:

export USER_AGENT='Mozilla/5.0 (Windows NT 10.0; rv:78.0) Gecko/20100101 Firefox/78.0'  
export JAR=~/.lb/cookie  
export OUT='\n%{url_effective}'

Add the following line to your /etc/fstab file:

Linux:  
tmpfs /home/$USER/.lb/pages tmpfs uid=$USER,gid=$USER,mode=700,noatime,nodev,nosuid,noexec 0 0  
touch ~/.lb/pages/{1..32}

OpenBSD:  
swap /home/$USER/.lb/pages mfs noatime,nodev,nosuid,noexec 0 0  
for i in $(jot 32); do touch ~/.lb/pages/$i; done

.lesskey
------
The l keybinding is used for clicking links. The p keybinding is used for http requests that send form data via a post request. The --data-raw option is necessary to send data in POST requests. To send a form with get data, use the l keybinding. The w keybinding is used to request a url. The c keybinding is used to request raw html. This allows the user to modify the html to fix errors or manually interpret javascript followed by a reparsing of the webpage. The i keybinding is used to make google search queries. After typing i, enter a urlencoded search query.

html macro
The user agent is provided in the -A option because some websites do not work with curl's user agent. The -L option is convenient because it enables curl to perform redirects. The JAR environment variable is provided in the -b and -c option if one wishes to maintain the state of an HTTP session. The --compressed option allows for faster requests and allows support for servers that send gzipped data regardless of the Accept: Encoding header. The option -w is necessary for the python script to work: it assumes that the last url that curl requested is in the last line of the data. For more information about the options, read the curl manpage. 

Links
-----
At the bottom of a parsed webpage are links.

https://www.google.com/search q=

Each link has a URL and data which are separated by the first space, ' '. Edit the data by pressing v to use it to make searches, login to websites, and more. For example, modifying the above to search google would look like this: 

https://www.google.com/search?q=how+do+I+install+w3m

The data must be urlencoded.

Each link has a link number which corresponds to the number of lines it is from the bottom of the file, starting with 1 as the last line of the file. Links are written in reverse so that the first link is at the bottom of the page and the last link is above every other link.

The last line of the file always corresponds to the current url of the webpage. This allows one to easily retrieve the raw html of the webpage by changing the url to "$(tail -1 %)". 

without form data (do not require user input) have the following format: ^[\<link number\>^] (e.g. ^[5^]). 

Links with form data (require user input) have the following format: ^[\<link number\> \<action\>^] [method] [enctype] (e.g. ^[5 /search^] get application/x-www-form-urlencoded).

The action is shown to give the user an idea of what the link is used for. In the example above, /search implies that the link is used for searching.

The method is shown to let the user know which method they should use to send data. If the method is get or if the method is not shown, the user should change the space to a ? when sending the data (see google example above). If the method is post, the user should only edit the data portion (after the space) and then press p followed by the link number.

The enctype value application/x-www-form-urlencoded or its absence is the default enctype which means that the user should press l followed by the link number to send the data. The enctype value multipart/form-data means that the user needs to manually specify parameters to send the data. If a parameter has a default value of '@', then the user should specify a file to send. 

For example, a link with a link number of 28 and enctype of multipart/form-data that looks like

http://www.tipjar.com/cgi-bin/test image=@&test=

would require the user to type 

!tail -28 | curl -s -b "$JAR" -c "$JAR" --compressed --proto =https -L -A "$USER_AGENT" -w "$OUT" "$(head -1)" -F image=@/tmp/test -F test=a

to send the file /tmp/test with the test parameter having a value of a.

A link with a link number of 5 and no enctype that looks like 

http://www.tipjar.com/cgi-bin/test image=test&test=test2

would require the user to press p followed by 5 as a shortcut to send the image parameter with a value of test and test parameter with a value of test2.

Warnings
--------
Many web pages have invalid html. It is possible to fix this by getting the raw html, manually fixing the error, then reparsing with %!lb.

Do not paste urls into the prompt: first paste them into an empty file, then use the l keybinding to request the url. Otherwise, less could potentially interpret some characters as shell metacharacters and execute commands.

It may be possible to use protocol smuggling to attempt a client side request forgery attack. If possible, use the web browser in a network namespace/virtual environment or ensure that services running have some sort of client side request forgery protection.

Work In Progress
----------------
- Rewrite in flex/bison for portability and correctness

Bugs
----
Please send issues to the email seL4+lb@disroot.org.
