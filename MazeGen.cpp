/////////////////////////////////////////////////////////////////////////
//  Name: MazeGen.cpp
//  Purpose: Generate a 10x10 maze in a format that can be included in a
//           8080 assembly language program written for an Altair 8800 
//           running CPM.
//  Created by L. Johnson  12/25/2021 from Val_Maze
//  
//  03/04/2022 - L. Johnson - Modified to randomize roll column that
//							start and finish rooms are located.   Updated
//                          to ensure finish (exit) room has only 1 door.
//
/////////////////////////////////////////////////////////////////////////



#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string.h>

int main(int argc, char** argv) {

  #define FALSE 0
  #define TRUE  1
  
  unsigned short maze [100] = {

    
  }  ;
  // This will contain the data to be validated
  int i;  // this will be the primary maze index and will be controlled by a for loop.
  int j;  // temporary variable for border checking
  int exitCnt;  // count the number of exits for the room
  int borderFlag = FALSE;
  int connectFlag = FALSE;
  int exitFoundFlag = FALSE;  //Flag indicating the exit was found
  int mainPathFound = FALSE;  // Flag indicating the main path was found
  int pathInWork = 0;  // 0 is start path, 1 is exit path
  int startIndex;  //  Will be adjusted as start path is generated
  int exitIndex;  // Will be adjusted as exit path is generated
  int wIndex;   // working index (room being evaluated now)
  int availDirections[4] ;  // Will be populated when new room is worked
  int availRooms[100] ; // Will be populated just prior to picking a random room
  int numAvailRooms;   //  Number of available rooms.  
  int numPathRooms;  // Number of rooms on new path
  int selDirection;   // Direction Selected Randomly from available directions
  
  //  Zero out maze
  for (i=0;i<100;i++) maze[i] =0;
  
  /* initialize random seed: */
  srand (time(NULL));
  
  // *******************************************************
  //  Phase 1 - Create path from starting room to exit room 
  // *******************************************************
  startIndex=rand()%10;
  exitIndex=90+(rand()%10);
  wIndex = startIndex;
  
  while (exitFoundFlag == FALSE)  {
  
    // Add to start path  

    // Count & Identify available directions
    exitCnt = 0;   // initialize exit count

    // Check north 
    if (wIndex>9) { // Room is not on the north border of maze
      // if not a north border then verify room to the north does not already have doors
      if (maze[wIndex-10] == 0)  { 
        // Room to the north is fair game.   
        availDirections[exitCnt ] =8 ; // 8 = north
        exitCnt ++ ;   
      }
    }
      
    // Check South 
    if (wIndex<90) { // Room is not on the south border of maze
      // if not a south border then verify room to the south does not already have doors
      if (maze[wIndex+10] == 0)  { 
        // Room to the south is fair game.   
        availDirections[exitCnt ] =4 ; // 4 = south
        exitCnt ++ ;   
      }
    }      
      
    // Check East 
    if ((wIndex+1) %10 != 0) { // Room is not on the east border of maze
      // if not a south border then verify room to the east does not already have doors
      if (maze[wIndex+1] == 0)  { 
        // Room to the east is fair game.   
        availDirections[exitCnt ] =2 ; // 2 = east
        exitCnt ++ ;   
      }
    }            
    
    // Check West 
    if (wIndex %10 != 0) { // Room is not on the west border of maze
      // if not a south border then verify room to the west does not already have doors
      if (maze[wIndex-1] == 0)  { 
        // Room to the west is fair game.   
        availDirections[exitCnt ] =1 ; // 1 = west
        exitCnt ++ ;   
      }
    }  

    if (exitCnt>0) {  // Choose a direction randomly from the availDirections
      selDirection = rand() % exitCnt;
      switch(availDirections[selDirection]) {
      case 8:  // North
        maze[wIndex] = maze[wIndex]  + 8;  // Add North door to current room
        wIndex = wIndex - 10;   // Set new working index
        maze[wIndex] = maze[wIndex] + 4;  // Add south door to new room
        break;
      case 4:  // South
        maze[wIndex] = maze[wIndex]  + 4;  // Add south door to current room
        wIndex = wIndex +10;   // Set new working index
        maze[wIndex] = maze[wIndex] + 8;  // Add north door to new room
        break;
      case 2:  // East
        maze[wIndex] = maze[wIndex]  + 2;  // Add east door to current room
        wIndex = wIndex +1;   // Set new working index
        maze[wIndex] = maze[wIndex] + 1;  // Add west door to new room
        break;    
      case 1:  // West
        maze[wIndex] = maze[wIndex]  + 1;  // Add west door to current room
        wIndex = wIndex -1;   // Set new working index
        maze[wIndex] = maze[wIndex] + 2;  // Add east door to new room
        break;
      }    
        
      // Check if maze exit found
      if (wIndex == exitIndex)  {
      	exitFoundFlag=TRUE;
		maze[exitIndex]=maze[exitIndex]+0x80;	

	  }         
    }

    // If current room has no exits - select a new current room randomly from the already
    // created path -
    if (exitCnt == 0) {
      numAvailRooms = 0;
      for (i=0;i<100;i++) {
        if (maze[i]> 0) {
          availRooms[numAvailRooms] = i; // Room has exits so it is on path & available
          numAvailRooms++;
        }      
      }
      // Select Room randomly from available rooms
      wIndex = availRooms[ rand()% numAvailRooms] ;
    } 

  }

  
  
  
  //  Phase 2 - Populate rest of maze - 
  //    Randomly pick a room with no exits
  //    Randomly add an exit to the room such that it does not go outside maze
  //       or connect to its own path
  //      Keep adding rooms to the new path (using upper nibble) until connected
  //      with the main path.
  //    Repeat the process until there are no rooms left in the maze with zero exits     
  numAvailRooms = -1;
  while (numAvailRooms != 0) {
    numAvailRooms = 0;
    for (i=0;i<100;i++) {
      if (maze[i] == 0) {
        availRooms[numAvailRooms] = i; // Room has no exits so it is available
        numAvailRooms++;
      }      
    }  
    if (numAvailRooms > 0) {
      // There are available rooms - pick a room and build a path to the main path
      wIndex = availRooms[rand() % numAvailRooms] ;
      mainPathFound=FALSE;
      while (mainPathFound == FALSE) {
        
        // Count & Identify available directions
        exitCnt = 0;   // initialize exit count

        // Check north 
        if (wIndex>9) { // Room is not on the north border of maze
        // if not a north border then verify room to the north is not part of current path
          if (maze[wIndex-10] < 16)  { 
            // Room to the north is fair game.   
            availDirections[exitCnt ] =8 ; // 8 = north
            exitCnt ++ ;   
          }
        }
      
        // Check South 
        if (wIndex<90) { // Room is not on the south border of maze
          // if not a south border then verify room to the south does not already have doors
          if (maze[wIndex+10] <16)  { 
            // Room to the south is fair game.   
            availDirections[exitCnt ] =4 ; // 4 = south
            exitCnt ++ ;   
          }
        }      
      
        // Check East 
        if ((wIndex+1) %10 != 0) { // Room is not on the east border of maze
          // if not a south border then verify room to the east does not already have doors
          if (maze[wIndex+1] <16)  { 
            // Room to the east is fair game.   
            availDirections[exitCnt ] =2 ; // 2 = east
            exitCnt ++ ;   
          }
        }            
    
        // Check West 
        if (wIndex %10 != 0) { // Room is not on the west border of maze
          // if not a south border then verify room to the west does not already have doors
          if (maze[wIndex-1] <16)  { 
            // Room to the west is fair game.   
            availDirections[exitCnt ] =1 ; // 1 = west
            exitCnt ++ ;   
          }
        }  

        if (exitCnt>0) {  // Choose a direction randomly from the availDirections
          selDirection = rand() % exitCnt;
          switch(availDirections[selDirection]) {
          case 8:  // North
            maze[wIndex] = maze[wIndex]  + 0x80 ;  // Add North door to current room
            wIndex = wIndex - 10;   // Set new working index
            if (maze[wIndex] == 0) maze[wIndex] = maze[wIndex] + 0x40;
            else { 
              maze[wIndex] = maze[wIndex] + 4;  
              mainPathFound = TRUE;    
            }    
            break;
          case 4:  // South
            maze[wIndex] = maze[wIndex]  + 0x40;  // Add south door to current room
            wIndex = wIndex +10;   // Set new working index
            if (maze[wIndex] == 0) maze[wIndex] = maze[wIndex] + 0x80;
            else { 
              maze[wIndex] = maze[wIndex] + 8;  
              mainPathFound = TRUE;    
            }  
            break;
          case 2:  // East
            maze[wIndex] = maze[wIndex]  + 0x20;  // Add east door to current room
            wIndex = wIndex +1;   // Set new working index
            if (maze[wIndex] == 0) maze[wIndex] = maze[wIndex] + 0x10;
            else { 
              maze[wIndex] = maze[wIndex] + 1;  
              mainPathFound = TRUE;    
            }  
            break;  
          case 1:  // West
            maze[wIndex] = maze[wIndex]  + 0x10;  // Add west door to current room
            wIndex = wIndex -1;   // Set new working index
            if (maze[wIndex] == 0) maze[wIndex] = maze[wIndex] + 0x20;
            else { 
              maze[wIndex] = maze[wIndex] + 2;  
              mainPathFound = TRUE;    
            }  
            break;
          }    // switch(availDirections[selDirection])    
        } //  if (exitCnt>0) { 
        if (exitCnt == 0)   {
        //   Pick a different room on new path
          numPathRooms = 0;  
          for (i=0;i<100;i++) {
            if (maze[i] > 15) {
              availRooms[numPathRooms] = i;
              numPathRooms++;
            }
          }
          wIndex = availRooms[rand() % numPathRooms] ;          
        }  // if (exitCnt == 0) 
        

              
      } // while (mainPathFound == FALSE)
      

      for (i=0;i<100;i++) {
        if ((maze[i] >15 ) && (i!=exitIndex)) maze[i] = maze[i] >> 4; // add new path to main path
      }                      

    }  // if (numAvailRooms > 0)
  }  // while (numAvailRooms != 0) 
  
  maze[startIndex]=maze[startIndex]+0x10;  //Flag Entrance	
  
  //  Output maze array in C format 
  /* Not right now
  for (i=0;i<100;i++){
    printf("0X%02x,",maze[i]);
    if (((i+1)%10)==0) printf("\n");
  }
  */
  
  
  for (i=0;i<100;i++){  
  
    // Clear borderFlag in case it was set
      borderFlag = FALSE;  
      
    // clear exit counter
      exitCnt = 0;
        
    //  check north border
    if (i<10){
      // We are on the northern border - there better not be any doors going north
      if (( maze[i] & 0x08)> 0) {
        // There was!   Document it!
        borderFlag = TRUE;
        printf("North border violation for maze parameter %d!\n",i);
      }
    }
    
    //  check south border
    j = i/10;  // divide by 10 - throw away the remainder (if first digit is 9, then we are
               // on the southern border
    if (j==9){
      // We are on the south border - there better not be any doors going south
      if (( maze[i] & 0x04)> 0) {
        // There was!   Document it!
        borderFlag = TRUE;
        printf("South border violation for maze parameter %d!\n",i);
      }
    }
    
    //  check east border
    if ((i+1)%10 == 0){
      // We are on the eastern border - there better not be any doors going east
      if (( maze[i] & 0x02)> 0) {
        // There was!   Document it!
        borderFlag = TRUE;
        printf("East border violation for maze parameter %d!\n",i);
      }
    }
    
    //  check west border
    if ( i%10 == 0){
      // We are on the western border - there better not be any doors going east
      if (( maze[i] & 0x01)> 0) {
        // There was!   Document it!
        borderFlag = TRUE;
        printf("West border violation for maze parameter %d!\n",i);
      }
    }
    
    
    if (borderFlag == FALSE) {
    // No border problems - check doors to neigbor cells
    
      if (( maze[i] & 0x08) > 0) { // Is there a north door
        // There is - increment exit counter.
        exitCnt++;  
        // Does room to the north have a south door?
        if ((maze[i-10] & 0x04)  ==0 )   
          // No!   We have a discrepancy between rooms
          printf("Door discrepancy between maze parameters %d and %d!\n",i,i-10);  
      }
      
      if (( maze[i] & 0x04) > 0) { // Is there a south door
        // There is - increment exit counter.
        exitCnt++;        
        // Does room to the south have a north door?
        if ((maze[i+10] & 0x08) ==0 )   
          // No!   We have a discrepancy between rooms
          printf("Door discrepancy between maze parameters %d and %d!\n",i,i+10);  
      }      
      
      if (( maze[i] & 0x02) > 0) { // Is there a east door
        // There is - increment exit counter.
        exitCnt++;  
        // Does room to the east have a west door?
        if ((maze[i+1] & 0x01)  ==0 )   
          // No!   We have a discrepancy between rooms
          printf("Door discrepancy between maze parameters %d and %d!\n",i,i+1);  
      }      
      
      if (( maze[i] & 0x01) > 0) { // Is there a west door
        // There is - increment exit counter.
        exitCnt++;        
        // Does room to the east have a east door?
        if ((maze[i-1] & 0x02)  ==0 )   
          // No!   We have a discrepancy between rooms
          printf("Door discrepancy between maze parameters %d and %d!\n",i,i-1);  
      }  
      
      switch (exitCnt){
        case 0:
          printf("Room at %d has no exits!\n",i);
          break;
        case 1:
          // printf("Room at %d has one exit!\n",i);
          maze[i]=maze[i] | 0x20;   //Set single exit flag 
          break;
        default:
          maze[i] = maze[i] & 0xdf;  //Clear single exit flag 
      }
            
    }    
    
  }
  
  //  Output maze array in 8080 Assembly Language format 
  for (i=0;i<100;i++){
    if (((i)%10)==0) printf("  DB  ") ;
    if (maze[i]>0x9f) printf("%03XH",maze[i]);
    else printf("%02XH",maze[i]);
    if (((i+1)%10)==0) printf("\n"); else printf(",");
  }
  
  // Top Border
  for (i=0;i<10;i++){
    printf("+---");
  }  
  printf("+\n") ;
  
  
  for (j=0;j<10;j++) {
  
    // Middle of Cell (West Exits)
    for (i=0;i<10;i++) {
      if ((maze[i+j*10] & 0x01)==0) printf("|"); else printf(" ");
      if ((maze[i+j*10] & 0x10)==0x10) printf(" S "); 
      if ((maze[i+j*10] & 0x80)==0x80) printf(" F "); 
      if ((maze[i+j*10] & 0x90)==0) printf("   ");
    }
    printf("|\n") ;
  
    // Bottom of Cell  (South Exits)
    for (i=0;i<10;i++) {
      if ((maze[i+j*10] & 0x04)==0) printf("+---"); else printf("+   ");
    }
    printf("+\n") ; 
  }
  
  
  return 0;
}


