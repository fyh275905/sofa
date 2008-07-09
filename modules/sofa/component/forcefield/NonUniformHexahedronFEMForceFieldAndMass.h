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
#ifndef SOFA_COMPONENT_FORCEFIELD_NONUNIFORMHEXAHEDRONFEMFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_NONUNIFORMHEXAHEDRONFEMFORCEFIELD_H


#include <sofa/component/forcefield/HexahedronFEMForceFieldAndMass.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;
using sofa::helper::vector;

/** Need a SparseGridTopology with _sparseGrid->_nbVirtualFinerLevels >= this->_nbVirtualFinerLevels

@InProceedings{NPF06,
author       = "Nesme, Matthieu and Payan, Yohan and Faure, Fran\c{c}ois",
title        = "Animating Shapes at Arbitrary Resolution with Non-Uniform Stiffness",
booktitle    = "Eurographics Workshop in Virtual Reality Interaction and Physical Simulation (VRIPHYS)",
month        = "nov",
year         = "2006",
organization = "Eurographics",
address      = "Madrid",
url          = "http://www-evasion.imag.fr/Publications/2006/NPF06"
}


*/


template<class DataTypes>
class NonUniformHexahedronFEMForceFieldAndMass : virtual public HexahedronFEMForceFieldAndMass<DataTypes>
{
public:
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef VecCoord Vector;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
#ifdef SOFA_NEW_HEXA
    typedef sofa::core::componentmodel::topology::BaseMeshTopology::SeqHexas VecElement;
#else
    typedef sofa::core::componentmodel::topology::BaseMeshTopology::SeqCubes VecElement;
#endif

    typedef HexahedronFEMForceFieldAndMass<DataTypes> HexahedronFEMForceFieldAndMassT;
    typedef HexahedronFEMForceField<DataTypes> HexahedronFEMForceFieldT;

    typedef typename HexahedronFEMForceFieldAndMassT::ElementStiffness ElementStiffness;
    typedef typename HexahedronFEMForceFieldAndMassT::MaterialStiffness MaterialStiffness;
    typedef typename HexahedronFEMForceFieldAndMassT::MassT MassT;
    typedef typename HexahedronFEMForceFieldAndMassT::ElementMass ElementMass;


public:


    Data<int> _nbVirtualFinerLevels; ///< use virtual finer levels, in order to compte non-uniform stiffness, only valid if the topology is a SparseGridTopology with enough VirtualFinerLevels.
    Data<bool> _useMass; ///< Do we want to use this ForceField like a Mass? (or do we prefer using a separate Mass)
    Data<Real> _totalMass;

    NonUniformHexahedronFEMForceFieldAndMass():HexahedronFEMForceFieldAndMassT()
    {
        _nbVirtualFinerLevels = initData(&this->_nbVirtualFinerLevels,0,"nbVirtualFinerLevels","use virtual finer levels, in order to compte non-uniform stiffness");
        _useMass = initData(&this->_useMass,true,"useMass","Using this ForceField like a Mass? (rather than using a separated Mass)");
        _totalMass = initData(&this->_totalMass,(Real)0.0,"totalMass","");
    }



    virtual void init();
    virtual void reinit()  { std::cerr<<"WARNING : non-uniform mechanical properties can't be updated, changes on mechanical properties (young, poisson, density) are not taken into account.\n"; }



    virtual void addMDx(VecDeriv& f, const VecDeriv& dx, double factor = 1.0);
    virtual void addGravityToV(double dt);
    virtual void addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& v);


protected:


    static const float FINE_TO_COARSE[8][8][8]; ///< interpolation matrices from finer level to a coarser (to build stiffness and mass matrices)
    /// add a matrix of a fine element to its englobing coarser matrix
    void addFineToCoarse( ElementStiffness& coarse, const ElementStiffness& fine, int indice );

    /// condensate matrice from the (virtual) finest level to the actual mechanical level
    /// recursive function
    /// if level is the finest level, matrices are built as usual
    /// else  finer matrices are built by condensation and added to the current matrices by addFineToCoarse
    void computeMechanicalMatricesByCondensation( ElementStiffness &K, ElementMass &M, const int elementIndice,  int level);



    /// compute the hookean material matrix
    void computeMaterialStiffness(MaterialStiffness &m, double youngModulus, double poissonRatio);




};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
