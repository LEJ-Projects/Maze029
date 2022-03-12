/////////////////////////////////////////////////////////////////////////
//  Name: DMP2HEX.cpp
//  Purpose: Convert data from a CP/M dump of a .COM file into a hex 
//           file that can be downloaded to a CP/M machine and 
//           converted into a .COM file via the CP/M LOAD command.
//
//  Revision   Date       Engineer       Description of Change
//  --------   --------   ------------   ------------------------------
//  1.0        2/1/22    L. Johnson     Initial Development
//  1.1        2/16/22   L. Johnson     Fixed short record problem
/////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define FALSE	0
#define TRUE 	1

#include <iostream>


void cont();

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char** argv) {
  FILE  *fi, *fo;
  int i;                         /* Loop index */
  char fi_name[256];             /* Input file name */
  char fo_name[256];             /* Output file name */
  char str_in[256];                /* Input String */
  char str[256];		// Temporary String
  char recstr[80];		// Record String
  char datstr[80];		// Data String
  char hchar[3];
  int addr = 0x0100;	// Initial address in Hex File.
  int rbytecnt = 16;	// Record Byte Count default is 16
  int arbytecnt;		// Actual byte count
  int cksum;		// Computed Checksum
  int ocksum;		// Old cksum (debug variable)
  char *eptr;
  int value;
  

  printf("DMP2HEX - Version 1.1\n");
  
  printf("Enter input file name: ");
  scanf("%s",fi_name);
  fi = fopen(fi_name,"r");
  if (fi==NULL) {
    printf("Unable to open %s for input.\n",fi_name);
    for (i=0;i<16;i++) {
    	
    	
	}
	return 0;
  }


  printf("Enter output file name: ");
  scanf("%s",fo_name);
  fo = fopen(fo_name,"w");
  if (fo==NULL) {
    printf("Unable to open %s for output.\n",fo_name);
	return 0;
  }

  printf("\n\n");
  printf("Input file:%s   Output file:%s \n\n",fi_name,fo_name);

	

  while (fgets(str_in,256,fi) != NULL){	
    
    printf("%s\n",str_in);
    
    cksum=0;	// Initialize to Number of bytes (Assumed to be 16).
    strcpy(recstr,":10");  // Initialize the record string (record byte count = 16)
    sprintf(str,"%04X00",addr);  //Address and Record Type of 00
    strcat(recstr,str);
    ocksum=cksum;
    cksum=cksum+(addr>>8) + (addr&0xff);
    // printf("old cksum = %04x Checksum = %04x %04x %02x %02x \n",ocksum,cksum, addr, addr>>8, addr&0xff);
    
  	for (i=0;i<rbytecnt;i++){
	  strncpy( hchar,&str_in[5+(3*i)],3);
  	  hchar[2]='\0';
  	  // printf(" %s ",hchar);
  	  value= strtol(hchar, &eptr,16);
	  cksum+=value;
	  if (hchar[0]==' ') { // We have a short string...
	     // update record length
	     arbytecnt = i;  // byte count for chksum calculation
	     sprintf(str,"%02X",arbytecnt);
	     recstr[1]=str[0]; // Adjust byte count in record string - nibble 1
	     recstr[2]=str[1]; // Adjust byte count in record string - nibble 2
		 i= rbytecnt;  // Break out of loop  	
	  }  else {
	  	strcat(recstr,hchar);	
	  	arbytecnt=0x10;
	  }
    } //  for i=0;i<rbytecnt;i++){
	cksum = cksum+=arbytecnt;  
    cksum = cksum & 0xff;
    sprintf(str,"%02X",(0x100 - cksum) & 0xff);	 
    strcat(recstr,str);
    // printf("Checksum = %04x\n",cksum);  // Debug print

    printf("%s\n",recstr);
    fprintf(fo,"%s\n",recstr);
    addr=addr+16;
    // printf("New Address = %04x\n",addr);
    
   //  cont();
  }  // while (fgets(str_in,256,fi) != NULL){

  printf(":0000000000\n");
  fprintf(fo,":0000000000\n");
  cont();	
	fclose(fo);
	fclose(fi);
	return 0;
}

/****************************************************************
** Function:  cont()
** 
** Description:  Prompts operator to press enter to continue.
**
****************************************************************/

void cont() {

  getchar();   /* Seems to be necessary to flush stdin */
  printf("Press enter to continue:");
  getchar();   /* This is the one that counts */
}
