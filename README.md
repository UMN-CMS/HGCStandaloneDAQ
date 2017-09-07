# HGCal Standalone DAQ

To run, execute `start_daq` in the main directory. This defaults to run number 0 with 1000 events and real triggers. You can change the default settings by supplying the following arguments to `start_daq`:
```
./start_daq [RUN NUMBER] [EVENTS] [PEDESTAL]
```
Pedestals are not currently supported, so the PEDESTAL argument must be 0.

Before running, update the `RPI_IP_PREFIX` and `IPBUS_PREFIX` variables in `start_daq` to match the IP prefix (first 3 octets, so "192.168.222.") for the Raspberry Pis and IPBus, respectively. The RPi IPs must have already been set, but the IPBus IPs are set on each run. The files `etc/rdout_ips.txt` and `etc/sync_ips.txt` must also be updated with the fourth octet of each RDOUT and SYNC board in use. For each board, place a number on a new line in the file, and make sure there are no blank lines in the file. The number placed in the file should be the last octet of the IP address used to connect to the Raspberry Pi. Again, the Raspberry Pis' IPs must already be assigned!

Data files are outputted to the `data/` directory. An example filename is as follows: RUN0_05092017_154159.raw. If pedestals are used, the filename starts with PED_. The first set of digits is the date in ddmmyyyy format, and the second is the time of the run in hhmmss format.
