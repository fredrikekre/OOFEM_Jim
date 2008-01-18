/* $Header: /home/cvs/bp/oofem/oofemlib/src/symcompcol.h,v 1.3 2003/04/06 14:08:26 bp Exp $ */
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

// Inspired by SPARSELib++
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*             ********   ***                                 SparseLib++    */
/*          *******  **  ***       ***      ***               v. 1.5c        */
/*           *****      ***     ******** ********                            */
/*            *****    ***     ******** ********              R. Pozo        */
/*       **  *******  ***   **   ***      ***                 K. Remington   */
/*        ********   ********                                 A. Lumsdaine   */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                                           */
/*                                                                           */
/*                     SparseLib++ : Sparse Matrix Library                   */
/*                                                                           */
/*               National Institute of Standards and Technology              */
/*                        University of Notre Dame                           */
/*              Authors: R. Pozo, K. Remington, A. Lumsdaine                 */
/*                                                                           */
/*                                 NOTICE                                    */
/*                                                                           */
/* Permission to use, copy, modify, and distribute this software and         */
/* its documentation for any purpose and without fee is hereby granted       */
/* provided that the above notice appear in all copies and supporting        */
/* documentation.                                                            */
/*                                                                           */
/* Neither the Institutions (National Institute of Standards and Technology, */
/* University of Notre Dame) nor the Authors make any representations about  */
/* the suitability of this software for any purpose.  This software is       */
/* provided ``as is'' without expressed or implied warranty.                 */
/*                                                                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*        Compressed column sparse matrix  (O-based, Fortran)            */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#ifndef symcompcol_h

#include "compcol.h"

/**
 Implementation of symmetric sparse matrix stored using compressed column/row storage.
 Only the lower part is stored.
*/
class SymCompCol : public CompCol
{

protected:

public:
   /** Constructor. Before any operation an internal profile must be built.
    @see builInternalStructure
   */
      SymCompCol (int n) ;   
    /** Constructor. Before any operation an internal profile must be built.
     @see builInternalStructure
   */
    SymCompCol () ;
     /// Copy constructor
     SymCompCol(const SymCompCol &S);
    /// Destructor
      ~SymCompCol () {}                           // destructor
    /** Returns {\bf newly allocated} copy of receiver. Programmer must take 
     care about proper deallocation of allocated space.
     @return newly allocated copy of receiver */ 
    SparseMtrx* GiveCopy () const ;
      /** Evaluates a product of receiver with vector. 
        @param x array to be multiplied with receiver
        @param answer result of product of receiver and x parameter
     */
     void times (const FloatArray& x, FloatArray& answer) const;
     /** Multiplies receiver by scalar value.
        @param x value to multiply receiver
     */
     virtual void times (double x) ;
     /// Builds internal structure of receiver
     int buildInternalStructure (EngngModel*, int, EquationID) ; 
 /** Assembles receiver from local element contributions.
    @param loc location array. The values corresponding to zero loc array value are not assembled.
    @param mat contribution to be assembled using loc array.
   */
 int assemble (const IntArray& loc, const FloatMatrix& mat) ;
 /** Assembles receiver from local element contributions.
    @param rloc row location array. The values corresponding to zero loc array value are not assembled.
    @param cloc column location array. The values corresponding to zero loc array value are not assembled.
    @param mat contribution to be assembled using loc array.
   */
 int assemble (const IntArray& rloc, const IntArray& cloc, const FloatMatrix& mat) ;
 
 /// Determines, whether receiver can be factorized.
 int canBeFactorized () const {return 0;}
 /// Zeroes the receiver.
 virtual SparseMtrx* zero ();

 /// Returns coefficient at position (i,j). 1-based element access
 virtual double& at (int i, int j) ;
 /// Returns coefficient at position (i,j). 1-based element access
 virtual double at (int i, int j) const;

 SparseMtrxType  giveType() const {return SMT_SymCompCol;}
 int isAntisymmetric () const {return 0;}

 const double&      val(int i) const { return val_(i); }
 const int&         row_ind(int i) const { return rowind_(i); }
 const int&         col_ptr(int i) const { return colptr_(i);}
 
 int          dim(int i) const {return dim_[i];};
  
protected:
/*******************************/
/*  Access and info functions  */
/*******************************/

 double&      val(int i) { return val_(i); }
 int&         row_ind(int i) { return rowind_(i); }
 int&         col_ptr(int i) { return colptr_(i);}
 
 int          size(int i) const {return dim_[i];};
 int          NumNonzeros() const {return nz_;};
 int          base() const {return base_;}
 
/***********************************/
/*  General access function (slow) */
/***********************************/
 /// implements 0-based acess
 double       operator() (int i, int j) const;               
 /// implements 0-based acess
 double&      operator() (int i, int j);               

#ifdef IML_COMPAT
 /***********************************/
/*  Matrix/Vector multiply         */
/***********************************/

  FloatArray operator*(const FloatArray &x) const
  {FloatArray answer; this->times (x, answer) ; return answer;}
 FloatArray trans_mult(const FloatArray &x) const
  {FloatArray answer; this->times (x, answer) ; return answer;}

#endif


} ;


#define symcompcol_h
#endif
