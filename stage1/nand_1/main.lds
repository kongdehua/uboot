OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")                                                                 
/*OUTPUT_FORMAT("elf32-arm", "elf32-arm", "elf32-arm")*/
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{     
	 first 0x30000000: { *.o }
      
}
