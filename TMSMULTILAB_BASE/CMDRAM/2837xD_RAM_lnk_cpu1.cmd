-stack 0x400
-heap 0x200
MEMORY
{
PAGE 0 :
   /* BEGIN is used for the "boot to SARAM" bootloader mode   */

   BEGIN           	: origin = 0x000000, length = 0x000002
   RAMM0           	: origin = 0x000122, length = 0x0002DE
   RAML          	: origin = 0x009100, length = 0x006f00

   RESET           	: origin = 0x3FFFC0, length = 0x000002

PAGE 1 :

   BOOT_RSVD       : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
   RAMM1           : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */

   RAMG     : origin = 0x010000, length = 0x00b000

   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400

   CANA_MSG_RAM     : origin = 0x049000, length = 0x000800
   CANB_MSG_RAM     : origin = 0x04B000, length = 0x000800
  }


SECTIONS
{
   codestart        : > BEGIN,     PAGE = 0
   .text            : > RAML ,   PAGE = 0
   .cinit           : > RAML,     PAGE = 0
   .pinit           : > RAML,     PAGE = 0
   .switch          : > RAML,     PAGE = 0
   .reset           : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */

   .stack           : > RAMM1,     PAGE = 1
   .ebss            : > RAMG,    PAGE = 1
   .econst          : > RAMG,    PAGE = 1
   .esysmem         : > RAMG,    PAGE = 1
   Filter_RegsFile  : > RAMG,	   PAGE = 1

   ramgs0           : > RAMG,    PAGE = 1
   ramgs1           : > RAMG,    PAGE = 1
   ramfunc          : > RAML,      PAGE = 0
   .fprog           : > RAML,      PAGE = 0

  IQmathTables      : > RAML, PAGE = 0
  QmathTablesRam    : load = RAMG, PAGE = 1
  IQmath            : load = RAML, PAGE = 0

 .ppdata  : > RAMG
 .cio   : > RAMG
   /* The following section definitions are required when using the IPC API Drivers */
    GROUP : > CPU1TOCPU2RAM, PAGE = 1
    {
        PUTBUFFER
        PUTWRITEIDX
        GETREADIDX
    }

    GROUP : > CPU2TOCPU1RAM, PAGE = 1
    {
        GETBUFFER :    TYPE = DSECT
        GETWRITEIDX :  TYPE = DSECT
        PUTREADIDX :   TYPE = DSECT
    }

    /* The following section definition are for SDFM examples */
   Filter1_RegsFile : > RAMG,	PAGE = 1, fill=0x1111
   Filter2_RegsFile : > RAMG,	PAGE = 1, fill=0x2222
   Filter3_RegsFile : > RAMG,	PAGE = 1, fill=0x3333
   Filter4_RegsFile : > RAMG,	PAGE = 1, fill=0x4444
   Difference_RegsFile : >RAMG, 	PAGE = 1, fill=0x3333
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
