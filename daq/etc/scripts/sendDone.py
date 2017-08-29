import uhal

uhal.setLogLevelTo(uhal.LogLevel.ERROR)

manager = uhal.ConnectionManager('file://etc/connections.xml')
rdout = manager.getDevice('hgcal.rdout0')

rdout.getNode('RDOUT_DONE').write(0xABCD4321)
rdout.dispatch()
