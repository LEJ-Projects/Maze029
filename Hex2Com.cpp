// Hex2Com.cpp : main project file.

/**********************************************************************
** 
** Program:   Hex2Com.cpp
**
** Purpose:  Read an Intel Hex file (.hex) and create a binary file (.com)
**           
**
** Author: L. Johnson,
** Created: 1/26/22
** Current Version: 1.2  
***********************************************************************
**  Revision   Date       Engineer       Description of Change
**  --------   --------   ------------   ------------------------------
**  1.0        1/26/22   L. Johnson     Initial Creation
**  1.1		   2/12/22   L. Johnson	    Process address jumps in hex file
**  1.2	       3/12/22   L. Johnson     Minor cleanup
**********************************************************************/
#define TRUE  1
#define FALSE 0            

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void cont();

int main(int argc, char** argv) {


  FILE  *fi;
  FILE  *fo;
  char  fi_name[256];      /* input file name */
  char  fo_name[256];      /* output file name */	
  char  str_in[256];       /* Input String */	
  char	hchar[3];	// Hex Character
  unsigned char c;		// hex digit
  int rec_cnt = 0;  // Record Count
  int rbytecnt;	// Record Byte Count
  int old_rbytecnt;  // Old Record Byte Count
  int  tbytecnt = 0; // Total byte Count
  int  	done;			// Flag indicating we are done processing the input
  unsigned int old_addr = 0;
  unsigned int new_addr;
  int target_bytecnt;  // Target byte count...
  char	*eptr;
  long	value;  
  int cksum;  // Checksum of record
  int c_off;  //Character offset...
  int i; // loop increment
  
   printf("Hex2Com - Version 1.2\n");
  
  printf("Enter input file name: ");
  scanf("%s",fi_name);
  fi = fopen(fi_name,"r");
  if (fi==NULL) {
    printf("Unable to open %s for input.\n",fi_name);
    cont();
	exit(1);    
  }
  
  printf("Enter output file name: ");
  scanf("%s",fo_name);
  fo = fopen(fo_name,"wb");
  if (fo==NULL) {
    printf("Unable to open %s for output.\n",fo_name); 

	exit(1);
  } 
  


  done = FALSE;
while (done == FALSE) {            
            
    if (fgets(str_in,256,fi) != NULL){
      rec_cnt++;
    	
    	
	  // printf("%s",str_in);
	  cksum=0;

	  // Number of data bytes
	  strncpy( hchar,&str_in[1],3);
	  hchar[2]='\0';
	  // printf("Byte Count = %s\n",hchar);
	  value= strtol(hchar, &eptr,16);
	  rbytecnt = value; 
	  cksum=value;
	  
	  // Address 1st Byte
	  strncpy( hchar,&str_in[3],3);
	  hchar[2]='\0';
	  value= strtol(hchar, &eptr,16);
	  cksum+= value; 
	  new_addr = value * 256;
	  
	  // Address 2nd Byte
	  strncpy( hchar,&str_in[5],3);
	  hchar[2]='\0';
	  value= strtol(hchar, &eptr,16);
	  cksum+= value;  
	  new_addr = new_addr + value;
	  
	  //Record Type
	  strncpy( hchar,&str_in[7],3);
	  hchar[2]='\0';
	  value= strtol(hchar, &eptr,16);
	  cksum+= value;  
	  
	  // Check for Address Jump
	  if (((old_addr+old_rbytecnt) <new_addr) && (rec_cnt>1)) {
      // We hit an address jump.   Need to run fill characters until new_addr is reached
      // Note: address jumps don't count on the first record.
  	    for (i=0;i<(new_addr -(old_addr+old_rbytecnt));i++){
	      c=0;  // Use 0 for fill character...  
	      fputc(c,fo);
	      tbytecnt++;  
       } //  for (i=0;i<(new_addr -(old_addr+rbytecnt));i++) {
    } // if (  ((old_addr+rbytecnt) <new_addr)) && (rec_cnt>1) {
    old_addr=new_addr;  
    old_rbytecnt = rbytecnt;
	    
	  // printf("Record Byte Count = %d\n",rbytecnt);
	  c_off=0;  
	  if (rbytecnt>0){
	  	for (i=0;i<rbytecnt;i++){
		  strncpy( hchar,&str_in[9+(2*i)],3);
	  	  hchar[2]='\0';
	  	  // printf(" %s ",hchar);
	  	  value= strtol(hchar, &eptr,16);
	      c = value;	
		  // printf (  "%02x",c);
		  fputc(c,fo);
		  tbytecnt++;  
		  cksum+=value;
       } //  for i=0;i<rbytecnt;i++){
       strncpy( hchar,&str_in[9+(2*rbytecnt)],3); // Get Checksum
	   hchar[2]='\0';
	   // printf(" %s ",hchar);
	   value= strtol(hchar, &eptr,16);   // Value is now the record checksum
       if (((value+cksum )& 0xff) != 0) {
         printf("%s",str_in);  	 
		 printf("Checksum Error! \n") ;
       	 printf("Record Checksum =0X%02X Calculated Checksum=0X%02X\n",
			value&0xff,0x0100-(cksum &0xff));
   	     cont();
	   }
	   // cont();
      // if (tbytecnt>0x800) cont();
	  }	//  if (rbytecnt>0){
	}else {
	  done=TRUE;
	  target_bytecnt = ((tbytecnt%128)+1) * 128;
	  if (tbytecnt<target_bytecnt){
	    for (i=tbytecnt;i<target_bytecnt;i++) {
		  c = 0;	
		  fputc(c,fo);
        }   //  for (i=tbytecnt;i<target_bytecnt;i++) {
	  } // if (tbytecnt<target_bytecnt){
	  fclose(fo);
	}  // if (fgets(str_in,256,fi) != NULL){
       
  }
  cont();	
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
  printf("Press enter to continue:\n");
  getchar();   
}

