import sys

KEY = bytearray(b"%Co+Fi~uRaT:On$38")
DVLHEADER = bytearray([0x86,0x64,0x56,0x4C,0x0D,0x0A,0x1A,0x0A,0x00,0x00,0x1D,0xFE,0x43,0x4E,0x46,0x47])

def dvl_xor(data, key):
    l = len(key)
    for i in range(0, len(data)):

        # if an XOR operation would result in a null byte we leave it untouched
        # every $keylength byte we also leave untouched.
        if data[i] ^ key[i % l] == 0 or (i % len(key) == 0 and i != 0):
            continue
        data[i] = data[i] ^ key[i % l]
    return data

if len(sys.argv) != 2:
    sys.exit("Usage: %s /path/to/file/to/de/crypt.cfg" % sys.argv[0])

data = bytearray(open(sys.argv[1], 'rb').read())
if DVLHEADER != data[:16]:
    sys.exit("Invalid devolo config header")
f = dvl_xor(data[16:-35], KEY)

print("".join(map(chr, f)))




