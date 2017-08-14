# HGCStandaloneDAQ/daq
Contains the DAQ source files.

The DAQ can be compiled by running `make` in this folder. Before starting a run, the environment must be set by using `source etc/env.sh`.

To run the DAQ, use `./bin/main [RUN NUMBER] [EVENTS]`. For convenience, a script named `run_test_daq` has been provided to set the environment, compile, and run the DAQ for 1000 events for testing.
