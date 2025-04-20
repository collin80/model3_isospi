Model 3 IsoSPI Interface using
RP2040 dev board

Requires some sort of isoSPI interface board. I (Collin) made one
which was essentially copied verbatim from Damien Maguire's work.
Mine is not public but his is: 
https://github.com/damienmaguire/Tesla-Model-3-Battery-BMS/tree/master/FPGA

Yes, his assumes you are going to use his Spartan 6 FPGA based design.
Just don't populate the FPGA and wire it to an RP2040 board instead. Or, 
make your own board without the FPGA interface. In any event, all credit
for the hardware design goes to Damien, not me.

Currently this code doesn't do a lot for you. It can generate 
the Tesla specific isoSPI signals but does not yet read them
nor really produce proper BMS traffic. That will come in time.

The easiest approach to compiling this is to use VSCode. Download
the Raspberry Pi Pico extension. It is able to download all the SDKs
and compilers for you.