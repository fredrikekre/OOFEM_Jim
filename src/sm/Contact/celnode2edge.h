/*
 *
 *                 #####    #####   ######  ######  ###   ###
 *               ##   ##  ##   ##  ##      ##      ## ### ##
 *              ##   ##  ##   ##  ####    ####    ##  #  ##
 *             ##   ##  ##   ##  ##      ##      ##     ##
 *            ##   ##  ##   ##  ##      ##      ##     ##
 *            #####    #####   ##      ######  ##     ##
 *
 *
 *             OOFEM : Object Oriented Finite Element Code
 *
 *               Copyright (C) 1993 - 2013   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef celnode2node_h
#define celnode2node_h

#include "contact/contactelement.h"

///@name Input fields for _IFT_Node2EdgeContact
//@{
#define _IFT_Node2EdgeContactP_Name "node2edgecontactp"

//@}

namespace oofem {
class Domain;
class ContactManager;
class ContactDefinition;
class SparseMtrx;
class TimeStep;
class DofManager;
class GaussPoint;
class UnknownNumberingScheme;
class FloatMatrix;
class IntegrationRule;
class ContactElement;
class ContactPairNode2Edge;
class ContactPair;

class OOFEM_EXPORT Node2EdgeContact : public ContactElement
{
protected:
    ContactDefinition *cDef;
    ContactPairNode2Edge *cPair;
    
private:
    
    // should be set by input:
    double area; // The area associated with the node (default = 1)- in order to represent some physical dimension.  
    
    //TODO store in a contact pair
    Element *masterElement;   // the element to which the edge belongs TODO what to do with shared edges? 
    int masterElementEdgeNum; // local edge number on the master element 
    
public:
    
    /// Constructor.
    Node2EdgeContact(int num, Domain *d, ContactDefinition *cDef, ContactPair *cPair);
    /// Destructor.
    virtual ~Node2EdgeContact(){};
    virtual int instanciateYourself(DataReader *dr);
    virtual void setupIntegrationPoints();
    
    virtual void computeGap(FloatArray &answer, TimeStep *tStep);
    virtual void computeContactTractionAt(GaussPoint *gp, FloatArray &t, FloatArray &gap, TimeStep *tStep);
    
    // Necessary methods - pure virtual in base class
    virtual void computeContactForces(FloatArray &answer, TimeStep *tStep);    
    
    virtual void computeContactTangent(FloatMatrix &answer, CharType type, TimeStep *tStep);

    
    virtual void giveLocationArray(IntArray &answer, const UnknownNumberingScheme &s);
    virtual const char *giveInputRecordName() const { return _IFT_Node2EdgeContactP_Name; }
    
    virtual void computeNmatrixAt(const FloatArray &lCoords, FloatMatrix &answer);
    virtual double computeCurrentAreaAround(GaussPoint *gp, TimeStep *tStep);
    virtual void computeCurrentNormalAt(const FloatArray &lCoords, FloatArray &normal, TimeStep *tStep);
    virtual void computeCurrentTransformationMatrixAt(const FloatArray &lCoords, FloatMatrix &answer, TimeStep *tStep);
    virtual void computeCovarBaseVectorAt(const FloatArray &lCoords, FloatArray &g, TimeStep *tStep );
};



} // end namespace oofem
#endif // celnode2node_h
