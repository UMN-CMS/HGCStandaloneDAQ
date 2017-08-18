import uhal

uhal.setLogLevelTo(uhal.LogLevel.ERROR)

manager = uhal.ConnectionManager('file://etc/connections.xml')
rdout = manager.getDevice('hgcal.rdout0')

empty = rdout.getNode('EMPTY').read()
full  = rdout.getNode('FULL').read()
fifo = rdout.getNode('FIFO').read()
blk_rdy = rdout.getNode('BLOCK_READY').read()
rdout_done = rdout.getNode('RDOUT_DONE').read()

rdout.dispatch()

if empty and full:
    print "FIFO is empty and full!"
elif empty:
    print "FIFO is empty"
elif full:
    print "FIFO is full"
else:
    print "FIFO is neither full nor empty"


print hex(fifo)
print hex(blk_rdy)
print hex(rdout_done)


data = rdout.getNode('FIFO').readBlock(30787)
rdout.dispatch()

empty = rdout.getNode('EMPTY').read()
rdout.dispatch()
print hex(empty)

rdout.getNode('RDOUT_DONE').write(0xABCD4321)
const0 = rdout.getNode('CONSTANT0').read()
rdout.dispatch()
print hex(rdout_done), hex(const0)
