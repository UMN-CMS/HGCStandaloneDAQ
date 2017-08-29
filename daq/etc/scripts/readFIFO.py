import uhal

uhal.setLogLevelTo(uhal.LogLevel.ERROR)

manager = uhal.ConnectionManager('file://etc/connections.xml')
rdout = manager.getDevice('hgcal.rdout0')

fifo = rdout.getNode('FIFO').readBlock(30787)
rdout.dispatch()

loc = 0
for word in fifo:
    print loc, hex(word)
    loc = loc + 1
