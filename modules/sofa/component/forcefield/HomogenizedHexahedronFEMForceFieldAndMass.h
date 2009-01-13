/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_FORCEFIELD_HomogenizedHEXAHEDRONFEMFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_HomogenizedHEXAHEDRONFEMFORCEFIELD_H


#include <sofa/component/forcefield/NonUniformHexahedronFEMForceFieldAndMass.h>


// #include <sofa/component/container/MechanicalObject.h>


// #include <sofa/component/mapping/BarycentricMapping.h>
// #include <sofa/component/mapping/HomogenizedMapping.h>

// #include <sofa/component/mapping/IdentityMapping.h>
// #include <sofa/core/componentmodel/behavior/MechanicalMapping.h>
// #include <sofa/component/topology/MeshTopology.h>



// for memory :
// SOFA_NEW_HEXA :
//
// 	     7----6
//      /|   /|
// 	   3----2 |
//     | 4--|-5
//     |/   |/
//     0----1


namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;
using sofa::helper::vector;

template<class DataTypes>
class HomogenizedHexahedronFEMForceFieldAndMass : public NonUniformHexahedronFEMForceFieldAndMass<DataTypes>
{
public:
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef VecCoord Vector;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;


    typedef NonUniformHexahedronFEMForceFieldAndMass<DataTypes> NonUniformHexahedronFEMForceFieldAndMassT;
    typedef HexahedronFEMForceFieldAndMass<DataTypes> HexahedronFEMForceFieldAndMassT;
    typedef HexahedronFEMForceField<DataTypes> HexahedronFEMForceFieldT;

    typedef typename NonUniformHexahedronFEMForceFieldAndMassT::ElementStiffness ElementStiffness;
    typedef typename NonUniformHexahedronFEMForceFieldAndMassT::MaterialStiffness MaterialStiffness;
    typedef typename NonUniformHexahedronFEMForceFieldAndMassT::MassT MassT;
    typedef typename NonUniformHexahedronFEMForceFieldAndMassT::ElementMass ElementMass;


    using NonUniformHexahedronFEMForceFieldAndMassT::sout;
    using NonUniformHexahedronFEMForceFieldAndMassT::serr;
    using NonUniformHexahedronFEMForceFieldAndMassT::sendl;

    typedef typename NonUniformHexahedronFEMForceFieldAndMassT::VecElement VecElement;

    typedef Mat<8*3, 8*3, Real> Weight;


// 		typedef core::componentmodel::behavior::MechanicalState<DataTypes> MechanicalStateT;
// 		typedef core::componentmodel::behavior::MechanicalMapping<MechanicalStateT, MechanicalStateT > MechanicalMappingT;
// 		typedef mapping::HomogenizedMapping< MechanicalMappingT > MappingT;
// // 		typedef mapping::BarycentricMapping< MechanicalMappingT > MappingT;
// // 		typedef mapping::IdentityMapping< MechanicalMappingT > IdentityMappingT;
// 		typedef MechanicalObject<DataTypes> MechanicalObjectT;


public:



    HomogenizedHexahedronFEMForceFieldAndMass():HexahedronFEMForceFieldAndMassT()
    {
        _homogenizationMethod = initData(&this->_homogenizationMethod,0,"homogenizationMethod","0->static, 1->constrained static, 2->modal analysis");
        _finestToCoarse = initData(&this->_finestToCoarse,false,"finestToCoarse","Does the homogenization is done directly from the finest level to the coarse one?");
        _completeInterpolation = initData(&this->_completeInterpolation,false,"completeInterpolation","Is the non-linear, complete interpolation used?");
        _useRamification = initData(&this->_useRamification,true,"useRamification","If SparseGridRamification, are ramifications taken into account?");
        _drawType = initData(&this->_drawType,0,"drawType","");
        _drawColor = initData(&this->_drawColor,0,"drawColor","");
        _drawSize = initData(&this->_drawSize,(float)-1.0,"drawSize","");
    }



    virtual void init();
    virtual void reinit()
    {
        serr<<"WARNING : homogenized mechanical properties can't be updated, changes on mechanical properties (young, poisson, density) are not taken into account."<<sendl;
        if(_drawSize.getValue()==-1)
            _drawSize.setValue( (float)((this->_sparseGrid->getMax()[0]-this->_sparseGrid->getMin()[0]) * .004f) );
    }

    virtual void draw();


    Data<bool> _finestToCoarse;
    Data<int> _homogenizationMethod;
    Data<bool> _completeInterpolation;
    Data<bool> _useRamification;
    Data<int> _drawType;
    Data<int> _drawColor;
    Data<float> _drawSize;


//       protected:


    void findFinestChildren( helper::vector<int>& finestChildren, const int elementIndice,  int level=0);
    void computeMechanicalMatricesDirectlyFromTheFinestToCoarse( ElementStiffness &K, ElementMass &M, const int elementIndice);
    void computeMechanicalMatricesIterativly( ElementStiffness &K, ElementMass &M, const int elementIndice,  int level);
    void computeMechanicalMatricesIterativlyWithRamifications( ElementStiffness &K, ElementMass &M, const int elementIndice,  int level);

    /// multiply all weights for all levels and go to the finest level to obtain the final weights from the coarsest to the finest directly
    void computeFinalWeights( const Weight &W, const int coarseElementIndice, const int elementIndice,  int level);
    void computeFinalWeightsRamification( const Weight &W, const int coarseElementIndice, const int elementIndice,  int level);


    // surcharge NonUniformHexahedronFEMForceFieldAndMass::computeMechanicalMatricesByCondensation
//         virtual void computeMechanicalMatricesByCondensation( ElementStiffness &K, ElementMass &M, const int elementIndice,  int level);
    virtual void computeMechanicalMatricesByCondensation( );


// 		MechanicalObjectT* _finestDOF; // the nodes of the finest virtual level
// 		MappingT* _mapping; // the homogenized from the true DOFs to the finest DOFs


    helper::vector< helper::vector<Weight> > _weights;
    helper::vector< std::pair<int, Weight> > _finalWeights; // for each fine element -> the coarse element idx and corresponding Weight

protected:


    static const int FineHexa_FineNode_IndiceForAssembling[8][8]; // give an assembled idx for each node or each fine element
    static const int FineHexa_FineNode_IndiceForCutAssembling_27[27];// give an cutted assembled idx for each node or each fine element, if constrained -> idx in Kg, if not constrained -> idx in Kf

    static const int CoarseToFine[8]; // from a coarse node idx -> give the idx of the same node in the fine pb

    static const bool IS_CONSTRAINED_27[27]; // is the ith assembled vertices constrained?

    static const int WEIGHT_MASK[27*3][8*3];
    static const int WEIGHT_MASK_CROSSED[27*3][8*3];
    static const int WEIGHT_MASK_CROSSED_DIFF[27*3][8*3];
    static  const float MIDDLE_INTERPOLATION[27][8];
    static  const int MIDDLE_AXES[27];
    static const int FINE_ELEM_IN_COARSE_IN_ASS_FRAME[8][8];

    static const float RIGID_STIFFNESS[8*3][8*3];

};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
