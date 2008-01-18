/* $Header: /home/cvs/bp/oofem/oofemlib/src/verbose.h,v 1.5.4.1 2004/04/05 15:19:44 bp Exp $ */
/*

                   *****    *****   ******  ******  ***   ***                            
                 **   **  **   **  **      **      ** *** **                             
                **   **  **   **  ****    ****    **  *  **                              
               **   **  **   **  **      **      **     **                               
              **   **  **   **  **      **      **     **                                
              *****    *****   **      ******  **     **         
            
                                                                   
               OOFEM : Object Oriented Finite Element Code                 
                    
                 Copyright (C) 1993 - 2000   Borek Patzak                                       



         Czech Technical University, Faculty of Civil Engineering,
     Department of Structural Mechanics, 166 29 Prague, Czech Republic
                                                                               
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                                                                              
*/

/*
  file VERBOSE.DEF

  initializes the variable VERBOSE, in order to get a few intermediate
  printings on screen : beginning and end of every time step, assembly of
  every element, assembly of every node's load vector.

  initializes the variable DETAILED_REPORT, in order to get a very detailed
  printings on screen.

 initializes the variable TIME_REPORT, in order to get a detailed
 time summary of solution (assembly time, factoriazation time, time per solution step, etc.).
*/

#ifndef VERBOSE
#define VERBOSE             // please activate or de-activate this line


#define VERBOSE_PRINTS(str,str1) OOFEM_LOG_INFO("%-30s %6s\n",str,str1);
#define VERBOSE_PRINT0(str,number) OOFEM_LOG_INFO("%-30s %6d\n", str , number);

#endif


#define TIME_REPORT        // please activate or de-activate this line

#ifndef DETAILED_REPORT
//#define DETAILED_REPORT  // please activate or de-activate this line
//#define VERBOSE          // please activate or de-activate this line
#define TIME_REPORT        // please activate or de-activate this line
#endif








