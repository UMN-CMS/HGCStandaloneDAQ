import uhal

uhal.setLogLevelTo(uhal.LogLevel.ERROR)

manager = uhal.ConnectionManager('file://etc/connections.xml')
rdout = manager.getDevice('hgcal.rdout0')

block_ready = rdout.getNode('BLOCK_READY').read()
rdout.dispatch()

if block_ready:
    print "Block is ready"
else:
    print "Block is not ready"
