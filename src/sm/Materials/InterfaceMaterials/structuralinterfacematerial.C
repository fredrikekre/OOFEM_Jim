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

#include "structuralinterfacematerial.h"
#include "structuralinterfacematerialstatus.h"
#include "dynamicinputrecord.h"
#include "gausspoint.h"

namespace oofem {
StructuralInterfaceMaterial :: StructuralInterfaceMaterial(int n, Domain *d) : Material(n, d)
{
    this->useNumericalTangent = false;
}


int
StructuralInterfaceMaterial :: giveIPValue(FloatArray &answer, GaussPoint *gp, InternalStateType type, TimeStep *tStep)
{
    StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus(gp) );
    if ( type == IST_InterfaceJump ) {
        answer = status->giveJump();
        return 1;
    } else if ( type == IST_InterfaceTraction ) {
        answer = status->giveTraction();
        return 1;
    } else if ( type == IST_InterfaceFirstPKTraction ) {
        answer = status->giveFirstPKTraction();
        answer = status->giveTempFirstPKTraction();
        return 1;
    } else if ( type == IST_DeformationGradientTensor ) {
        answer.beVectorForm( status->giveF() );
        return 1;
    } else {
        return Material :: giveIPValue(answer, gp, type, tStep);
    }
}


IRResultType
StructuralInterfaceMaterial :: initializeFrom(InputRecord *ir)
{
    IRResultType result;                    // Required by IR_GIVE_FIELD macro

    IR_GIVE_OPTIONAL_FIELD(ir, this->useNumericalTangent, _IFT_StructuralInterfaceMaterial_useNumericalTangent);

    return IRRT_OK;
}


void
StructuralInterfaceMaterial :: giveInputRecord(DynamicInputRecord &input)
{
    Material :: giveInputRecord(input);
}


#if 1
void
StructuralInterfaceMaterial :: giveStiffnessMatrix_dTdj_Num(FloatMatrix &answer, MatResponseMode rMode, GaussPoint *gp, TimeStep *tStep)
{
    // Default implementation for computation of the numerical tangent
    // Computes the material stiffness using a central difference method

    StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus(gp) );
    if ( status ) {
        FloatMatrix F;
        F = status->giveTempF();
        int dim = F.giveNumberOfRows();
        answer.resize(dim, dim);
        answer.zero();
        const double eps = 1.0e-9;
        FloatArray T, TPlus, TMinus;

        FloatArray jump, jumpPlus, jumpMinus, Kcolumn;
        jump = status->giveTempJump();
        for ( int i = 1; i <= dim; i++ ) {
            jumpPlus = jumpMinus = jump;
            jumpPlus.at(i)  += eps;
            jumpMinus.at(i) -= eps;
            this->giveFirstPKTraction_3d(TPlus, gp, jumpPlus, F, tStep);
            this->giveFirstPKTraction_3d(TMinus, gp, jumpMinus, F, tStep);

            Kcolumn = ( TPlus - TMinus );
            answer.setColumn(Kcolumn, i);
        }
        answer.times( 1.0 / ( 2 * eps ) );
        this->giveFirstPKTraction_3d(T, gp, jump, F, tStep); // reset temp values by recomputing the stress
    }
}
#endif


void
StructuralInterfaceMaterial :: give1dStiffnessMatrix_Eng(FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep)
{
    FloatMatrix answer3D;
    give3dStiffnessMatrix_Eng(answer3D, mode, gp, tStep);
    IntArray mask;
    answer.resize(1,1);
    answer.at(1,1) = answer3D.at(3,3);

}


void
StructuralInterfaceMaterial :: give2dStiffnessMatrix_Eng(FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep)
{
    FloatMatrix answer3D;
    give3dStiffnessMatrix_Eng(answer3D, mode, gp, tStep);
    IntArray mask = {1, 3};
    answer.beSubMatrixOf(answer3D, mask, mask);

#if 0
    //debug
    printf("analytical tangent \n");
    answer3D.printYourself();

    FloatMatrix answerNum;
    StructuralInterfaceMaterial :: giveStiffnessMatrix_dTdj_Num(answerNum, mode, gp, tStep);
    printf("numerical tangent \n");
    answerNum.printYourself();

    FloatMatrix comp;
    comp = answer3D;
    comp.subtract(answerNum);
    printf("difference in numerical tangent to mat method \n");
    comp.printYourself();
#endif
}

void
StructuralInterfaceMaterial :: give3dStiffnessMatrix_Eng(FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep)
{
    // Default implementation. Will use large deformation dT/dj as stiffness
    give3dStiffnessMatrix_dTdj(answer, mode, gp, tStep);
}

void
StructuralInterfaceMaterial :: give3dStiffnessMatrix_dTdj(FloatMatrix &answer, MatResponseMode rMode, GaussPoint *gp, TimeStep *tStep)
{
    OOFEM_ERROR("not implemented ")
}


void
StructuralInterfaceMaterial :: giveEngTraction_1d(FloatArray &answer, GaussPoint *gp, const FloatArray &jump, TimeStep *tStep)
{
    FloatArray jump3D(3), traction3D;
    jump3D = { 0.0, 0.0, jump.at(3) };
    this->giveEngTraction_3d(traction3D, gp, jump3D, tStep);
    answer = FloatArray{ traction3D.at(3) };

#ifdef DEBUG
    if ( ( fabs( traction3D.at(1) ) > 1.0e-3 ) || ( fabs( traction3D.at(2) ) > 1.0e-3 )  ) {
        OOFEM_ERROR("Traction vector obtained from 3D state contains a nonzero shear stress component")
    }
#endif
}

void
StructuralInterfaceMaterial :: giveEngTraction_2d(FloatArray &answer, GaussPoint *gp, const FloatArray &jump, TimeStep *tStep)
{
    FloatArray jump3D(3), traction3D;
    jump3D = { jump.at(1), 0.0, jump.at(2) };
    this->giveEngTraction_3d(traction3D, gp, jump3D, tStep);
    answer = { traction3D.at(1), traction3D.at(3) };

#ifdef DEBUG
    if ( fabs( traction3D.at(2) ) > 1.0e-3 ) {
        OOFEM_ERROR("Traction vector obtained from 3D state contains a nonzero thickness stress component")
    }
#endif
}

void
StructuralInterfaceMaterial :: giveEngTraction_3d(FloatArray &answer, GaussPoint *gp, const FloatArray &jump, TimeStep *tStep)
{
    // Default implementation calls first PK version with F = I
    FloatMatrix F(3, 3);
    F.beUnitMatrix();
    giveFirstPKTraction_3d(answer, gp, jump, F, tStep);
}



// Numerical tangents
void
StructuralInterfaceMaterial :: giveStiffnessMatrix_dTdj_Num( FloatMatrix &answer, MatResponseMode rMode, GaussPoint *gp, TimeStep *tStep,
void( *giveTraction )( FloatArray, GaussPoint*, const FloatArray,const FloatMatrix, TimeStep* ) )
{
    // Default implementation for computation of the numerical tangent dTdj
    // Computes the material stiffness using a central difference method

    StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus( gp ) );
    if(status) {
        FloatMatrix F;
        F = status->giveTempF( );
        int dim = F.giveNumberOfRows( );
        answer.resize( dim, dim );
        answer.zero( );
        const double eps = 1.0e-9;
        FloatArray T, TPlus, TMinus;

        FloatArray jump, jumpPlus, jumpMinus, Kcolumn;
        jump = status->giveTempJump( );
        for(int i = 1; i <= dim; i++) {
            jumpPlus = jumpMinus = jump;
            jumpPlus.at( i ) += eps;
            jumpMinus.at( i ) -= eps;
            giveTraction( TPlus, gp, jumpPlus, F, tStep );
            giveTraction( TMinus, gp, jumpMinus, F, tStep );

            Kcolumn = ( TPlus - TMinus );
            answer.setColumn( Kcolumn, i );
        }
        answer.times( 1.0 / ( 2 * eps ) );
        giveTraction( T, gp, jump, F, tStep ); // reset temp values by recomputing the stress
    }
}


void
StructuralInterfaceMaterial::giveStiffnessMatrix_Eng_Num( FloatMatrix &answer, MatResponseMode rMode, GaussPoint *gp, TimeStep *tStep,
void( *giveTraction )( FloatArray &answer, GaussPoint *gp, const FloatArray &jump, TimeStep *tStep ) )
{
    // Default implementation for computation of the numerical tangent d(sig)/d(jump)
    // Computes the material stiffness using a central difference method

    StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus( gp ) );
    if(status) {
        const double eps = 1.0e-9;
        FloatArray t, tPlus, tMinus;
        FloatArray jump, jumpPlus, jumpMinus, Kcolumn;
        jump = status->giveTempJump( );
        int dim = jump.giveSize();
        answer.resize( dim, dim );
        answer.zero( );

        for(int i = 1; i <= dim; i++) {
            jumpPlus = jumpMinus = jump;
            jumpPlus.at( i ) += eps;
            jumpMinus.at( i ) -= eps;
            giveTraction( tPlus, gp, jumpPlus, tStep );
            giveTraction( tMinus, gp, jumpMinus, tStep );

            Kcolumn = ( tPlus - tMinus );
            answer.setColumn( Kcolumn, i );
        }
        answer.times( 1.0 / ( 2 * eps ) );
        giveTraction( t, gp, jump, tStep ); // reset temp values by recomputing the stress
    }
}





void
StructuralInterfaceMaterial :: give1dStiffnessMatrix_dTdj_Num( FloatMatrix &answer, MatResponseMode rMode, GaussPoint *gp, TimeStep *tStep )
{
    // Default implementation for computation of the numerical tangent
    // Computes the material stiffness using a central difference method

    StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus( gp ) );
    if(status) {
        FloatMatrix F;
        F = status->giveTempF( );
        int dim = F.giveNumberOfRows( );
        answer.resize( dim, dim );
        answer.zero( );
        const double eps = 1.0e-9;
        FloatArray T, TPlus, TMinus;

        FloatArray jump, jumpPlus, jumpMinus, Kcolumn;
        jump = status->giveTempJump( );
        for(int i = 1; i <= dim; i++) {
            jumpPlus = jumpMinus = jump;
            jumpPlus.at( i ) += eps;
            jumpMinus.at( i ) -= eps;
            this->giveFirstPKTraction_1d( TPlus, gp, jumpPlus, F, tStep );
            this->giveFirstPKTraction_1d( TMinus, gp, jumpMinus, F, tStep );

            Kcolumn = ( TPlus - TMinus );
            answer.setColumn( Kcolumn, i );
        }
        answer.times( 1.0 / ( 2 * eps ) );
        this->giveFirstPKTraction_1d( T, gp, jump, F, tStep ); // reset temp values by recomputing the stress
    }
}

void
StructuralInterfaceMaterial :: give2dStiffnessMatrix_dTdj_Num( FloatMatrix &answer, MatResponseMode rMode, GaussPoint *gp, TimeStep *tStep )
{
    // Default implementation for computation of the numerical tangent
    // Computes the material stiffness using a central difference method

    StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus( gp ) );
    if(status) {
        FloatMatrix F;
        F = status->giveTempF( );
        int dim = F.giveNumberOfRows( );
        answer.resize( dim, dim );
        answer.zero( );
        const double eps = 1.0e-9;
        FloatArray T, TPlus, TMinus;

        FloatArray jump, jumpPlus, jumpMinus, Kcolumn;
        jump = status->giveTempJump( );
        for(int i = 1; i <= dim; i++) {
            jumpPlus = jumpMinus = jump;
            jumpPlus.at( i ) += eps;
            jumpMinus.at( i ) -= eps;
            this->giveFirstPKTraction_2d( TPlus, gp, jumpPlus, F, tStep );
            this->giveFirstPKTraction_2d( TMinus, gp, jumpMinus, F, tStep );

            Kcolumn = ( TPlus - TMinus );
            answer.setColumn( Kcolumn, i );
        }
        answer.times( 1.0 / ( 2 * eps ) );
        this->giveFirstPKTraction_2d( T, gp, jump, F, tStep ); // reset temp values by recomputing the stress
    }
}

void
StructuralInterfaceMaterial :: give3dStiffnessMatrix_dTdj_Num( FloatMatrix &answer, MatResponseMode rMode, GaussPoint *gp, TimeStep *tStep )
{
    // Default implementation for computation of the numerical tangent
    // Computes the material stiffness using a central difference method

    StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus( gp ) );
    if(status) {
        FloatMatrix F;
        F = status->giveTempF( );
        int dim = F.giveNumberOfRows( );
        answer.resize( dim, dim );
        answer.zero( );
        const double eps = 1.0e-9;
        FloatArray T, TPlus, TMinus;

        FloatArray jump, jumpPlus, jumpMinus, Kcolumn;
        jump = status->giveTempJump( );
        for(int i = 1; i <= dim; i++) {
            jumpPlus = jumpMinus = jump;
            jumpPlus.at( i ) += eps;
            jumpMinus.at( i ) -= eps;
            this->giveFirstPKTraction_3d( TPlus, gp, jumpPlus, F, tStep );
            this->giveFirstPKTraction_3d( TMinus, gp, jumpMinus, F, tStep );

            Kcolumn = ( TPlus - TMinus );
            answer.setColumn( Kcolumn, i );
        }
        answer.times( 1.0 / ( 2 * eps ) );
        this->giveFirstPKTraction_3d( T, gp, jump, F, tStep ); // reset temp values by recomputing the stress
    }
}


void
StructuralInterfaceMaterial :: give1dStiffnessMatrix_Eng_Num( FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep )
{
    // Default implementation for computation of the numerical tangent d(sig)/d(jump)
    // Computes the material stiffness using a central difference method

    StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus( gp ) );
    if(status) {
        const double eps = 1.0e-9;
        FloatArray t, tPlus, tMinus;
        FloatArray jump, jumpPlus, jumpMinus, Kcolumn;
        jump = status->giveTempJump( );
        int dim = jump.giveSize( );
        answer.resize( dim, dim );
        answer.zero( );

        for(int i = 1; i <= dim; i++) {
            jumpPlus = jumpMinus = jump;
            jumpPlus.at( i ) += eps;
            jumpMinus.at( i ) -= eps;
            this->giveEngTraction_1d( tPlus, gp, jumpPlus, tStep );
            this->giveEngTraction_1d( tMinus, gp, jumpMinus, tStep );

            Kcolumn = ( tPlus - tMinus );
            answer.setColumn( Kcolumn, i );
        }
        answer.times( 1.0 / ( 2 * eps ) );
        this->giveEngTraction_1d( t, gp, jump, tStep ); // reset temp values by recomputing the stress
    }
}

void
StructuralInterfaceMaterial :: give2dStiffnessMatrix_Eng_Num( FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep )
{
    // Default implementation for computation of the numerical tangent d(sig)/d(jump)
    // Computes the material stiffness using a central difference method

    StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus( gp ) );
    if(status) {
        const double eps = 1.0e-9;
        FloatArray t, tPlus, tMinus;
        FloatArray tempJump, jump, jumpPlus, jumpMinus, Kcolumn;
        tempJump = status->giveTempJump( );
        this->giveReducedJump(jump, tempJump, 2 );
        int dim = jump.giveSize( );
        answer.resize( dim, dim );
        answer.zero( );

        for(int i = 1; i <= dim; i++) {
            jumpPlus = jumpMinus = jump;
            jumpPlus.at( i ) += eps;
            jumpMinus.at( i ) -= eps;
            this->giveEngTraction_2d( tPlus, gp, jumpPlus, tStep );
            this->giveEngTraction_2d( tMinus, gp, jumpMinus, tStep );

            Kcolumn = ( tPlus - tMinus );
            answer.setColumn( Kcolumn, i );
        }
        answer.times( 1.0 / ( 2 * eps ) );
        this->giveEngTraction_2d( t, gp, jump, tStep ); // reset temp values by recomputing the stress
    }
}

void
StructuralInterfaceMaterial :: give3dStiffnessMatrix_Eng_Num( FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep )
{
    // Default implementation for computation of the numerical tangent d(sig)/d(jump)
    // Computes the material stiffness using a central difference method

    StructuralInterfaceMaterialStatus *status = static_cast< StructuralInterfaceMaterialStatus * >( this->giveStatus( gp ) );
    if(status) {
        const double eps = 1.0e-9;
        FloatArray t, tPlus, tMinus;
        FloatArray jump, jumpPlus, jumpMinus, Kcolumn;
        jump = status->giveTempJump( );
        int dim = jump.giveSize( );
        answer.resize( dim, dim );
        answer.zero( );

        for(int i = 1; i <= dim; i++) {
            jumpPlus = jumpMinus = jump;
            jumpPlus.at( i ) += eps;
            jumpMinus.at( i ) -= eps;
            this->giveEngTraction_3d( tPlus, gp, jumpPlus, tStep );
            this->giveEngTraction_3d( tMinus, gp, jumpMinus, tStep );

            Kcolumn = ( tPlus - tMinus );
            answer.setColumn( Kcolumn, i );
        }
        answer.times( 1.0 / ( 2 * eps ) );
        this->giveEngTraction_3d( t, gp, jump, tStep ); // reset temp values by recomputing the stress
    }
}



void 
StructuralInterfaceMaterial::giveReducedJump(FloatArray &answer, FloatArray &jump, const int size )
{
    if(size == jump.giveSize() ) {
        answer = jump;
    } else if( (size == 2) && (jump.giveSize() == 3) ) {
        answer = { jump.at(1), jump.at(3) };
    }
    else if(size == 1 && ( jump.giveSize( ) == 3 )) {
        answer = FloatArray{ jump.at(3) };
    }else{
        OOFEM_ERROR( "size must be 1, 2 or 3" );
    }

}

} // end namespace oofem
