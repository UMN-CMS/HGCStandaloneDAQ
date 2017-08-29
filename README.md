# HGCStandaloneDAQ
A standalone DAQ for HGCal

To run, execute `test_master_run` in the main directory. This defaults to run number 0 with 1000 events and real triggers. You can change the default settings by supplying the following arguments to `test_master_run`:
```
./test_master_run [RUN NUMBER] [EVENTS] [PEDESTAL]
```
Pedestals are not currently supported, so the PEDESTAL argument must be 0.

## MORE DOCS TO COME
