/* gcc -Wall -Wextra develo_fw_check.c -o develo_fw_check */
/* usage: ./develo_fw_check fw1 ... */
/* made for http://reverseengineering.stackexchange.com/questions/8521/reverse-engeneering-devolo-firmware */
/* plus additional work done by ... */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

#define FourCC2Str(code) (char[5]){(code >> 24) & 0xFF, (code >> 16) & 0xFF, (code >> 8) & 0xFF, code & 0xFF, 0}

uint8_t develo_magic[] = { 0x86, 0x64, 0x56, 0x4c, 0x0d, 0x0a, 0x1a, 0x0a };

static uint32_t crc_table[256] = {
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
	0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
	0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
	0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
	0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
	0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
	0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
	0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
	0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
	0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
	0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
	0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
	0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
	0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
	0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
	0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
	0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
	0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
	0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
	0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
	0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
	0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4};

uint32_t crc32 (uint32_t start_val, uint8_t *data, int len)
{
	int i;
	uint32_t crc = start_val;

	for (i=0; i<len; i++)
		crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *data++) & 0xff];

	return crc;
}


typedef struct section_header {
  uint32_t length;
  uint32_t fourcc;
} section_header;

typedef struct section_info {
  uint32_t fourcc;
  int print;
  const char *desc;
  uint32_t enc_mode;
  const char enc_key[20];
} section_info;


// encoding loop error! there seems to be an bug that was inherited and carried forward to keep compatibility
// key is starting from offset 0, but reset to offset 1 and iterated over including the zero
// reset (starting at 0 again) on buffer size/wrap 65536 !

section_info sections[32] = {
  { 0x46535459, 1, "Filesystem Type", 1,
    { 0x6b, 0x47, 0x38, 0x42, 0x35, 0x26, 0x43, 0x32, 0x6d, 0x2e, 0x6b, 0x4f,
      0x30, 0x23, 0xc3, 0x96, 0x2b, 0x34, 0x00 },
  },
  { 0x4456434f, 1, "DeviceType", 0, { 0x00 },
  },

  { 0x56524e54, 1, "OemVariant", 0, { 0x00 },
  },
  { 0x56524e58, 1, "OemVariant crypted", 1,
    { 0x2d, 0x37, 0x2f, 0x4d, 0x6f, 0x4d, 0x26, 0x6d, 0x46, 0x6b, 0x57, 0x24,
      0x6b, 0x45, 0x33, 0x32, 0x23, 0x00 },
  },

  { 0x4b524e4c, 0, "Kernel", 0, { 0x00 },
  },
  { 0x484b524e, 0, "Kernel with flash header", 2, "14.*RnElFiLe&94(2"
  },

  { 0x464c5359, 3, "Filesystem", 0, { 0x00 },
  },
  { 0x48464c53, 3, "Filesystem with flash header", 2, "Fi#eS,StEm)76"
  },
  { 0x46535459, 3, "Filesystem", 2,
    { 0x6b, 0x47, 0x38, 0x42, 0x35, 0x26, 0x43, 0x32, 0x6d, 0x2e, 0x6b, 0x4f, 0x30, 0x23, 0xc3, 0x96, 0x2b, 0x34, 0x00 }
  },


  { 0x48524b4e, 0, "Rescue kernel", 2, "g4*(BT8@6$,ca3#0R"
  },
  { 0x48524653, 0, "Rescue filesystem", 2, 
    { 0x46, 0x5e, 0x32, 0x33, 0x6a, 0x77, 0x70, 0x6f, 0x29, 0x2f, 0xc2, 0xa7, 0x6e, 0x48, 0x50, 0x3a, 0x61, 0x3b, 0x00 },
  },
  { 0x48464b4e, 0, "Full kernel", 2, "gb3%(kQgJ1#_<Fq9S"
  },
  { 0x48464653, 0, "Full filesystem", 2, 
    { 0x6b, 0x34, 0x6e, 0x2e, 0xc2, 0xa7, 0x32, 0x30, 0x6d, 0x53, 0x67, 0x52, 0x37, 0x7b, 0x20, 0x53, 0x66, 0x39, 0x00 },
  },

  { 0x53524e4d, 3, "Serial number of device", 2,
    { 0x2c, 0x5d, 0x69, 0x4b, 0xc3, 0xa4, 0x2d, 0x65, 0x36, 0x53, 0x27, 0x34,  0x45, 0x69, 0x59, 0x39, 0x33, 0x3a, 0x00 },
  },

  { 0x434e4647, 3, "Firmware file configuration", 2, "%Co+Fi~uRaT:On$38", 
  },
  { 0x434e4649, 1, "Flash file configuration", 2, 
    { 0x4a, 0x38, 0x2c, 0xc2, 0xa7, 0x2d, 0x2a, 0x70, 0x3f, 0x6d, 0x2f, 0x43, 0x76, 0x7a, 0x59, 0x3e, 0x34, 0x40, 0x00  },
  },


  { 0x4456434f, 0, "Device type", 0, { 0x00 },
  },
  { 0x44564358, 0, "Device type crypted", 2,
    { 0x29, 0x31, 0x28, 0x4f, 0x69, 0x4c, 0x64, 0x57, 0x21, 0x73, 0x53, 0x64, 0x49, 0x2e, 0x2e, 0x38, 0x32, 0x00 },
  },

  { 0x52424342, 0, "RedBoot config block", 2, "_5iDcV9&+!mAwi)`4",
  },
  { 0x52424342, 0, "RedBoot config block", 0, { 0x00 },
  },

  { 0x6e494d47, 0, "NAND flash image", 0, { 0x00 },
  },

  { 0x4d4e5652, 1, "Minimum FW version this file applies to", 0, { 0x00 },
  },
  { 0x4d585652, 1, "Maximum FW version this file applies to", 0, { 0x00 },
  },

  { 0x6e56524e, 1, "Environment to be stored", 2, "2TmUcKgSb8!fIuE92"
  },

  { 0x5652534e, 1, "Version crypted", 1,
    { 0x50, 0x4a, 0x69, 0x6b, 0x55, 0x2e, 0x37, 0x33, 0x69, 0x4a, 0x39, 0x29,
      0x28, 0x58, 0x27, 0x23, 0x73, 0x00 },
  },

  { 0x424c4452, 0, "Bootloader", 0, { 0x00 },
  },
  { 0x424c4458, 0, "Bootloader crypted", 2, 
    { 0x44, 0x68, 0x31, 0x64, 0x38, 0x4f, 0x30, 0x37, 0x2d, 0x39, 0x70, 0x23, 0x74, 0x34, 0x28, 0x78, 0xc2, 0xa7, 0x00 },
  },

  { 0x69445341, 2, "Signature DSA", 0, { 0x00 },
  },

  { 0, 0, NULL, 0, { 0x00 } },
};






void hexDump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if (!isprint(pc[i]))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}


section_info *get_si(uint32_t fourcc) {
  section_info *si = NULL;
  for (si = &sections[0]; si->fourcc != 0; si++) {
    if(si->fourcc == fourcc) return si;
  }
  return NULL;
}

// MUST increase in steps of 64k!
//uint8_t buffer[4096*1024*10];
uint8_t buffer[65536];


void do_section(section_header *sh, FILE *f) {
  uint32_t i;
  uint32_t crc;
  uint32_t stored_crc;
  uint32_t fourcc_nbo;
  section_info *si = get_si(sh->fourcc);
  if (!si) {
    printf("unknown section %04x %s (length 0x%08x %d); trying to skip\n", sh->fourcc, FourCC2Str(sh->fourcc), sh->length, sh->length);
    fseek(f, sh->length + 4, SEEK_CUR);
    return;
  }

  printf("section %04x %s %s (length 0x%08x %d): ", sh->fourcc, FourCC2Str(sh->fourcc), si->desc, sh->length, sh->length);
  crc = 0;
  fourcc_nbo = htonl(sh->fourcc);
  crc = crc32(crc, (uint8_t*)&fourcc_nbo, sizeof(fourcc_nbo));
  uint32_t bytes_to_go = sh->length;
  uint32_t enc_offset = 0;
  uint32_t enc_len = strlen(si->enc_key);
  FILE *output_fw;

    if (si->print==3) {
      output_fw = fopen(FourCC2Str(sh->fourcc), "wb");
    }

  while (bytes_to_go > 0) {

    uint32_t l = sizeof(buffer);
    if (l > bytes_to_go) {
      l = bytes_to_go;
    }

    fread(buffer, 1, l, f);

    if (si->enc_mode == 1) {
      for (i = 0; i < l; ++i) {
        if (i % 65536 == 0) {
          enc_offset = 0;
        }
        buffer[i] ^= si->enc_key[enc_offset];
        enc_offset++;
        if (enc_offset > enc_len) {
          enc_offset = 1;
        }
      }
    }

    if (si->enc_mode == 2) {
      for (i = 0; i < l; ++i) {
        if (i % 65536 == 0) {
          enc_offset = 0;
        }
	uint8_t decoded;
        decoded = buffer[i] ^ si->enc_key[enc_offset];
        if(decoded != 0 )
	  buffer[i] = decoded;
        enc_offset++;
        if (enc_offset > enc_len) {
          enc_offset = 1;
        }
      }
    }

    crc = crc32(crc, buffer, l);
    bytes_to_go -= l;
    if (si->print==1) {
      fwrite(buffer, 1, l, stdout);
    }
    if (si->print==2) {
      hexDump ("\n", &buffer, l);
    }
    if (si->print==3) {
      fwrite(buffer, 1, l, output_fw);
    }
  }

  uint32_t temp = sh->length + 4;
  while (temp) {
    uint8_t next_byte= temp & 0xff;
    crc = crc32(crc, &next_byte, 1);
    temp = temp >> 8;
  }

  crc = crc ^ 0xffffffff;

  fread(&stored_crc, 1, sizeof(stored_crc), f);
  stored_crc = ntohl(stored_crc);
  if (crc == stored_crc) {
    printf(" [CRC OK]\n");
  } else {
    printf(" [CRC BAD: got %08x, expected %08x]\n", stored_crc, crc);
  }

    if (si->print==3) {
      fclose(output_fw);
    }

}

int main(int argc, char *argv[]) {
  int i;
  FILE *input_fw;
  section_header sh;

  for (i=1; i<argc; ++i) {
    input_fw = fopen(argv[i], "rb");

    if (input_fw == NULL) {
      fprintf(stderr, "error: couldn't open '%s', skipping...\n", argv[i]);
      continue;
    }

    printf("info for '%s':\n", argv[i]);
    fread(buffer, sizeof(develo_magic), 1, input_fw);

    if (memcmp(buffer, develo_magic, sizeof(develo_magic)) != 0) {
      printf("incorrect magic\n");
      fclose(input_fw);
      continue;
    }

    while (!feof(input_fw)) {
      fread(&sh, sizeof(sh), 1, input_fw);
      if (feof(input_fw)) break;

      sh.length = ntohl(sh.length);
      sh.fourcc = ntohl(sh.fourcc);
      do_section(&sh, input_fw);
    }
    fclose(input_fw);
  }
  return 0;
}

