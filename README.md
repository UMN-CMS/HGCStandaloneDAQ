# HGCal Standalone DAQ

## Table of Contents
* [HGCal Standalone DAQ](#hgcal-standalone-daq)
  * [Table of Contents](#table-of-contents)
  * [Pre-Run Setup](#pre-run-setup)
  * [DAQ Execution](#daq-execution)
  * [Data Output](#data-output)
  * [Raspberry Pi Software/Firmware](#raspberry-pi-softwarefirmware)
  * [Main Script Overview](#main-script-overview)


## Pre-Run Setup
Before running, update the `RPI_IP_PREFIX` and `IPBUS_PREFIX` variables in `start_daq` to match the IP prefix (first 3 octets, so "192.168.222.") for the Raspberry Pis and IPBus, respectively. The RPi IPs must have already been set, but the IPBus IPs are set on each run. The files `etc/rdout_ips.txt` and `etc/sync_ips.txt` must also be updated with the fourth octet of each RDOUT and SYNC board in use. For each board, place a number on a new line in the file, and make sure there are no blank lines in the file. The number placed in the file should be the last octet of the IP address used to connect to the Raspberry Pi.

As an example, if you have a SYNC Pi at 192.168.111.0 and a RDOUT Pi at 192.168.111.1, and you'd like the IPBus IPs to begin with 192.168.112., then:
* `etc/rdout_ips.txt` should contain one line with "1" on it
* `etc/sync_ips.txt` should contain one line with "0" on it
* `RPI_IP_PREFIX` should be set to "192.168.111."
  * Don't forget the final dot in the prefix!
* `IPBUS_PREFIX` should be set to "192.168.112."

Again, the Raspberry Pis' IPs must already be assigned!


## DAQ Execution
To run, execute `start_daq` in the main directory. This defaults to run number 0 with 1000 events and real triggers. You can change the default settings by supplying the following arguments to `start_daq`:
```
./start_daq [RUN NUMBER] [EVENTS] [PEDESTAL]
```
Pedestals are not currently supported, so the PEDESTAL argument must be 0.


## Data Output
Data files are outputted to the `data/` directory. This can be changed in the `DATADIR` variable in `start_daq`. An example filename is as follows: RUN0_05092017_154159.raw. The digits immediately following "RUN" are the run number. If pedestals are used, the filename starts with "PED\_". The first set of digits is the date in ddmmyyyy format, and the second is the time of the run in hhmmss format.


## Raspberry Pi Software/Firmware
The software to be run on the Raspberry Pis as well as the firmware to program the ORMs on each board are contained in the `common/`, `rdout/`, and `sync/` folders. The `common/` folder contains software common to both readout and sync boards (an executable to program ORMs, common SPI routines, etc...) and is copied into both the `rdout/` and `sync/` folders. Because of this, the three folders must have the same basic structure:
* `rdout/ or sync/ or common/`
 * `bin/`: contains the executables
 * `etc/`: contains extra stuff
 * `src/`: contains source code
 * `fw/`: contains firmware files

The `rdout/` and `sync/` folders have their own compile scripts, which compile the required code and places the binaries in the `bin/` folder. The `common/` folder does not have a compile script; instead, the common files should be compiled in the rdout and sync compile scripts.

The `rdout/` folder is copied to all RDOUT  Pis, and the `sync/` folder is copied to all SYNC Pis using rsync. The `common/` folder is then copied into each using scp.

To add firmware to the `rdout/fw/` and `sync/fw/` directories, the hex files must be placed in the `fw/` directory inside the `rdout/` or `sync/` folder. The `run_rdout` and `run_sync` scripts should then be updated to use the correct firmware when programming ORMs.


## Main Script Overview
This section serves to outline exactly what happens when the `start_daq` script is run. The process is detailed below.

1. Argument Processing
    * Three arguments must be fed into the script: [RUN NUMBER] [EVENTS] and [PEDESTAL]. If these arguments are not supplied, the program defaults to run number 0 with 1000 events and real triggers.

2. File Copying to Raspberry Pis
    * The `~/rdout/` and `~/sync/` directories are created on the relevant Pis.
    * These directories are synchronized with the `rdout/` and `sync/` directories in the git repository using rsync.
    * The `common/` folder is copied into each using scp.
3. Firmware Install and Software Execution
    * The SYNCs are done first, then the RDOUTs.
    * The `run_rdout` or `run_sync` script is executed over ssh.
        1. Firmware is installed using the firmware files specified in the script.
        2. On RDOUTs, the IP/MAC addresses for IPBus are set.
        3. The rdout or sync helper process is executed.
    * Because the IPBus modules take a while to appear over the network, the script then waits until it gets a successful ping from each IPBus module.
4. DAQ Execution
    * The connections.xml file is generated with a python script.
    * The DAQ is compiled and executed.
5. Closing Actions
    * The helper processes are ended on the Pis.
        * A file named "stop.run.please" is created on the Pi, and its presence ends the helpers as they continually check for the presence of this file.
