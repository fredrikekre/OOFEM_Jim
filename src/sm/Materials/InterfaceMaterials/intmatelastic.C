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
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * intmatelastic.C
 *
 *  Created on: Jan 10, 2015
 *      Author: erik
 */

#include "intmatelastic.h"

#include "gausspoint.h"
#include "floatmatrix.h"
#include "floatarray.h"
#include "mathfem.h"
#include "datastream.h"
#include "contextioerr.h"
#include "classfactory.h"
#include "dynamicinputrecord.h"

namespace oofem {
REGISTER_Material(IntMatElastic);

IntMatElastic::IntMatElastic(int n, Domain *d) : StructuralInterfaceMaterial(n, d) {

}

IntMatElastic::~IntMatElastic() {

}

int
IntMatElastic :: hasMaterialModeCapability(MaterialMode mode)
{
    // returns whether receiver supports given mode
    if ( mode == _3dInterface ) {
        return 1;
    } else {
        return 0;
    }
}

void
IntMatElastic :: giveFirstPKTraction_3d(FloatArray &answer, GaussPoint *gp, const FloatArray &jumpVector,
                                                  const FloatMatrix &F, TimeStep *tStep)
{
	StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus(gp) );

    this->initTempStatus(gp);

    answer.resize( jumpVector.giveSize() );

    answer.at(1) = k*jumpVector.at(1);
    answer.at(2) = k*jumpVector.at(2);
    answer.at(3) = k*jumpVector.at(3);

    status->letTempJumpBe(jumpVector);
    status->letTempFirstPKTractionBe(answer);

}

void
IntMatElastic :: give3dStiffnessMatrix_dTdj(FloatMatrix &answer, MatResponseMode rMode, GaussPoint *gp, TimeStep *tStep)
{
	StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus(gp) );

    FloatArray jumpVector;

    jumpVector = status->giveTempJump();

    answer.resize(3, 3);
    answer.zero();

    answer.at(1, 1) = k;
    answer.at(2, 2) = k;
    answer.at(3, 3) = k;
}

IRResultType
IntMatElastic :: initializeFrom(InputRecord *ir)
{
    IRResultType result;                    // Required by IR_GIVE_FIELD macro

    IR_GIVE_FIELD(ir, k, _IFT_IntMatElastic_kn);

    StructuralInterfaceMaterial :: initializeFrom(ir);
    return IRRT_OK;
}

void IntMatElastic :: giveInputRecord(DynamicInputRecord &input)
{
    StructuralInterfaceMaterial :: giveInputRecord(input);

    input.setField(k, _IFT_IntMatElastic_kn);
}

} /* namespace oofem */
