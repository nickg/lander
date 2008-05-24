import array
import sys
import os
import stat

sys.argv.reverse()

# Discard script name
sys.argv.pop()

# Get output file
outfname = sys.argv.pop()

# Build a list of input files
files = [ s for s in sys.argv ]

offset_cnt = 0

def write_file(f, out):
    print "Writing data for", f
    fileobj = open(f, mode='rb')
    st = os.stat(f)
    out.write(fileobj.read(st[stat.ST_SIZE]))

def write_entry(f, out):
    global offset_cnt
    print "Writing index for", f
    st = os.stat(f)
    intvals = array.array('i')
    intvals.fromlist([ 140, offset_cnt, st[stat.ST_SIZE] ])
    intvals.tofile(out)

    pos = out.tell()
    svals = array.array('b')
    svals.fromstring(f + "\0")
    svals.tofile(out)
    while fileobj.tell() != pos + 128:
        out.write("\0")

    offset_cnt += st[stat.ST_SIZE]

# Open the output file
print "Creating output file", outfname
outfile = outfname
fileobj = open(outfile, mode='wb')

# Write the header
print "Writing file header"
outvals = array.array('i')
outvals.fromlist([ 8, len(files) ])
outvals.tofile(fileobj)
offset_cnt = 8

# Write the index
offset_cnt += 140 * len(files)
for f in files:
    write_entry(f, fileobj)

# Write the file data
for f in files:
    write_file(f, fileobj)

fileobj.close()
