import uhal

uhal.setLogLevelTo(uhal.LogLevel.ERROR)

mgr = uhal.ConnectionManager('file://etc/connections.xml')
rdout = mgr.getDevice('hgcal.rdout0')

def check_blk_ready():
    blk_ready = rdout.getNode('BLOCK_READY').read()
    rdout.dispatch()
    return blk_ready.value()

for i in range(1000):
    print 'Event', i
    while (not check_blk_ready()) and (i != 0):
        continue
    vals = rdout.getNode('FIFO').readBlock(30787)
    rdout.dispatch()
    rdout.getNode('RDOUT_DONE').write(0xABCD4321)
    rdout.dispatch()
