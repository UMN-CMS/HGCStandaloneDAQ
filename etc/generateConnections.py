import sys

if len(sys.argv) != 3:
    print 'Incorrect arguments. Should be [IP PREFIX] [RDOUT FILE]'
    exit(1)

ip_prefix = sys.argv[1]
rdout_file = open(sys.argv[2], 'r')

connection_file = open('daq/etc/connections.xml', 'w')

connection_file.write('<?xml version="1.0" encoding="UTF-8"?>\n\n')
connection_file.write('<connections>\n')

for index, line in enumerate(rdout_file):
    ip = ip_prefix + line.rstrip('\n')

    conn_line =	'\t<connection id="hgcal.rdout' + str(index) + '" uri="chtcp-2.0://localhost:10203?target=' + ip + ':50001" address_table="file://rdout.xml"/>\n'
    connection_file.write(conn_line)

connection_file.write('</connections>\n')
connection_file.close()
