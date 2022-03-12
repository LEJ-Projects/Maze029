/**********************************************************************
** 
** Program:   fil_dmp.cpp
**
** Purpose:  Reads data from a file and displays it.
**
** Author: L. Johnson,
** Created: 10/3/97 
** Current Version: 1.0   
***********************************************************************
**  Revision   Date       Engineer       Description of Change
**  --------   --------   ------------   ------------------------------
**  1.0        10/3/97    L. Johnson     Initial Release
**  1.1		   01/30/22	  L. Johnson	 Modified to look like a CP/M dump
**  1.2        02/02/22   L. Johnson     Updated to do upper case hex
**********************************************************************/


#define TRUE  1
#define FALSE 0            

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void prtline(FILE *fo, int *addr, char *hex_txt, char *txt);
void cont();

int main(int argc, char** argv) {


  FILE  *fi, *fo;
  int i;                         /* Loop index */
  char fi_name[256];             /* Input file name */
  char fo_name[256];             /* Output file name */
  int strt_byte;                 /* Starting byte location */
  int num_byte;                  /* Number of bytes being displayed */
  char hex_txt[80];              /* ascii string consisting of 16 bytes
								    separated by spaces */
  char hex_byte[4];              /* ascii string representing a byte
								    followed by a space */
  char txt[17];                  /* Text representation of the 16 bytes */
  unsigned char c;                        /* A single character */
  int byte_cnt=0;                /* Number of bytes read */
  int cnt=0;                     /* Number of bytes in output line */
  int start;                     /* Flag indicating starting byte has been
								    located */
  int addr=0;  // Address 
  printf("File Dump - Version 1.2\n");


  printf("Enter input file name: ");
  scanf("%s",fi_name);
                                                              
  fi = fopen(fi_name,"rb");
  if (fi==NULL) {
    printf("Unable to open %s for input.\n",fi_name);
    cont();
	exit(1);    
  }
  
 
  printf("Enter output file name: ");
  scanf("%s",fo_name);
  fo = fopen(fo_name,"w");
  if (fo==NULL) {
    printf("Unable to open %s for output.\n",fo_name); 
    cont();
	exit(1);
  } 



//  printf("Enter initial byte (1 is good): ");
//  scanf("%d",&strt_byte);
  strt_byte=1;

           
//  printf("Enter number of bytes: ");
//  scanf("%d",&num_byte);
 
  strcpy(hex_txt,"");
  start = FALSE;
  for (i=0;i<17;i++) txt[i]=NULL;  

  while (1){
    
    c= fgetc(fi);  /* Get a character */
    if (feof(fi) !=0) break;

   // printf("%04x ",byte_cnt);

	byte_cnt++;
	if (byte_cnt==strt_byte) start=TRUE;
	if (start==TRUE) {
		sprintf(hex_byte,"%02X ",c);
		strcat(hex_txt,hex_byte);
        if (isprint(c)!=0) txt[cnt++]=c; else txt[cnt++]='.';
		if (cnt==16) {
			prtline(fo, &addr, hex_txt, txt);
			cnt=0;
			addr+=16;
 			strcpy(hex_txt,"");
    		for (i=0;i<17;i++) txt[i]=NULL;
		}
	}
//	if (byte_cnt-strt_byte+1 == num_byte) break;  No break on byte count
  }


  if (strlen(txt)>0) prtline(fo, &addr,hex_txt, txt);
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
/****************************************************************
** Function:  prtline()
** 
** Description:  Prints a line of text to output file and screen
**
****************************************************************/


void prtline(FILE *fo, int *addr, char *hex_txt, char *txt)
{
	while (strlen(hex_txt)<48) strcat(hex_txt," ");
	printf ("%04X %s %s \n",*addr,hex_txt,txt);
	fprintf(fo, "%04X %s %s \n",*addr,hex_txt,txt);

}

