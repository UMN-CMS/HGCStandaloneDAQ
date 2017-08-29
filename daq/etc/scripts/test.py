import uhal

uhal.setLogLevelTo(uhal.LogLevel.ERROR)

manager = uhal.ConnectionManager('file://etc/connections.xml')
rdout = manager.getDevice('hgcal.rdout0')

print "Reading FIFO"
fifo = rdout.getNode('FIFO').readBlock(30787)
rdout.dispatch()
print fifo.value()

print "Sending RDOUT_DONE"
rdout.getNode('RDOUT_DONE').write(0xABCD4321)
rdout.dispatch()
