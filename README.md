C++ Routines for Interfacing with Tektronix Scopes through the VXI-11 protocol.

-------------
Dependencies:
-------------

vxi11: 
  Library by S. Sharples which provides a C API to send and receive commands with instruments using the VXI-11 protocol. 
  Code can be found here: http://optics.eee.nottingham.ac.uk/vxi11/
  Development done with version 1.10

ROOT:
  Available here: root.cern.ch
  Provides file format for compressed storage.
  Also, drawing and plotting tools.
  In principle, tekvxi11 should be written so that it can compile without ROOT. 
  But right now, very dependent on it.  Display and writing to disk relies on the package.

-----------------------
Hardware Configuration
-----------------------

The controller computer and scopes are to be plugged into a common router.
Probably other ways to setup the network.

-------------
Compilation
-------------

Need to set the location of VXI11 library and include files in VXILIB and VIXDIR, respectively, in the GNUmakefile.
Enable building with or without ROOT using ENABLEROOT variable, also found in the GNUmakefile.

-------------
Executables
-------------

test_tkvscope:
This is a working space for me to test out new features as I implement them.

run_synced_acq:
This program was written for a specific application.  
The goal is to record waveforms using two synchronized scopes.  
Synchronization is assisted using an Arduino which vetoes the trigger going into both scopes until both scopes have been armed to record.
Once the scopes finishing recording their traces, the waveforms are transferred via Ethernet, and the veto is turned back on.
The scopes are once again armed, and then triggered together.  
Repeating this sequence allows the scopes to stay in sync.

The program was written with the following network configuration in mind (diagram actually look like something in text editors):
         [router]
	     |
  ---------------------------------------------- ...
  |                          |         |
[controlling computer]    [scope 1]  [scope 2]   ...
  |                         |   |      |   |
[Arduino via USB/serial]    |   |      |   |
  |                         |   |      |   |
  | (veto)                  |   |      |   |
[trigger]  -------------------\ | /------\ | /--- ...
  |                             |          |
  |                             |          |
[measurement instruments]------------------------- ...