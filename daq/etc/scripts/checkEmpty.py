import uhal

uhal.setLogLevelTo(uhal.LogLevel.ERROR)

manager = uhal.ConnectionManager('file://etc/connections.xml')
rdout = manager.getDevice('hgcal.rdout0')

empty = rdout.getNode('EMPTY').read()
rdout.dispatch()

if empty:
    print "FIFO is empty"
else:
    print "FIFO is not empty"
