
FORK

 This is a fork of serbert, located at https://serbert.sourceforge.net

 This fork is to capture minor updates and improvements to be available to be
 available as per GPL. All credit goes to the original author, David Clarke.

 The usage page from sourceforge is now duplicated in doc/usage.html


README for Serbert

 Serbert is a command line utility which performs a Bit Error Rate Test (BERT)
 on serial lines. It does this by transmitting bytes, and waiting for their
 uncorrupted return.

 Serbert, however, does not provide a true Bit Error Rate Test (BERT), as it
 does not check the individual bits returned. It uses the operating systems
 standard serial interface, which provides the status of each returned byte.

HOME ON THE NET

 The project is hosted by Sourceforge. You can find the latest release at
 http://serbert.sourceforge.net/. If you have any questions, or think you have
 found a bug, feel free to email me at <dwclarke@users.sourceforge.net>.

INSTALLATION

 Instructions to install Serbert are given in the file INSTALL, but in general,
 after un-tarring the package, run the following:

   ./configure
   make
   make install

 You must be root to run the 'make install'.

 Once installed, it is the users responsibility to gain permission to use the
 serial port. For instance, the user may need to be a member of the dialout group.

 On linux systems Serbert locks the port by creating the lock flag
 '/var/lock/LCK..PORTNAME' It is also the users responsibility to gain 
 permission for the lock flag. 

 The lock facility is optional. Comment out SERBERT_LOCK in serbert_config.h
 before compilation, if port locking is not required.


UPDATE

 Updating the version should have no effect.

 To see the current version number of Serbert, it is shown on the help:

 >serbert -h

 and on the diagnostics:

 >serbert /dev/ttyS0 -d

USAGE

 For usage read the man or info page. A summary of the options are given
 by using:

 >serbert

 or

 >serbert -h
