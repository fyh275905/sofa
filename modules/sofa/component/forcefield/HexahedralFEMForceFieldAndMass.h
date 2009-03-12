/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_COMPONENT_FORCEFIELD_HEXAHEDRALFEMFORCEFIELDANDMASS_H
#define SOFA_COMPONENT_FORCEFIELD_HEXAHEDRALFEMFORCEFIELDANDMASS_H


#include <sofa/component/forcefield/HexahedralFEMForceField.h>
#include <sofa/core/componentmodel/behavior/Mass.h>

#include <sofa/component/topology/PointData.h>
#include <sofa/component/topology/HexahedronData.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;
using sofa::helper::vector;
using sofa::core::componentmodel::behavior::Mass;

/** Compute Finite Element forces based on hexahedral elements including continuum mass matrices
*/
template<class DataTypes>
class HexahedralFEMForceFieldAndMass : virtual public Mass<DataTypes>,
    virtual public HexahedralFEMForceField<DataTypes>
{
public:
    typedef HexahedralFEMForceField<DataTypes> HexahedralFEMForceFieldT;
    typedef Mass<DataTypes> MassT;

    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef VecCoord Vector;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    typedef typename HexahedralFEMForceFieldT::Mat33 Mat33;
    typedef typename HexahedralFEMForceFieldT::Displacement Displacement;
    typedef typename HexahedralFEMForceFieldT::VecElement VecElement;
    typedef typename HexahedralFEMForceFieldT::VecElementStiffness VecElementMass;
    typedef typename HexahedralFEMForceFieldT::ElementStiffness ElementMass;


    using HexahedralFEMForceFieldT::sout;
    using HexahedralFEMForceFieldT::serr;
    using HexahedralFEMForceFieldT::sendl;

    HexahedralFEMForceFieldAndMass();

    virtual void init( );
    virtual void reinit( );

    // handle topological changes
    virtual void handleTopologyChange(core::componentmodel::topology::Topology*);

    // -- Mass interface
    virtual  void addMDx(VecDeriv& f, const VecDeriv& dx, double factor = 1.0);

    virtual void addMToMatrix(defaulttype::BaseMatrix * matrix, double mFact, unsigned int &offset);

    virtual  void accFromF(VecDeriv& a, const VecDeriv& f);

    virtual  void addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    virtual double getKineticEnergy(const VecDeriv& /*v*/)  ///< vMv/2 using dof->getV()
    {serr<<"HexahedralFEMForceFieldAndMass<DataTypes>::getKineticEnergy not yet implemented"<<sendl; return 0;}

    virtual double getPotentialEnergy(const VecCoord& /*x*/)   ///< Mgx potential in a uniform gravity field, null at origin
    {serr<<"HexahedralFEMForceFieldAndMass<DataTypes>::getPotentialEnergy not yet implemented"<<sendl; return 0;}

    virtual void addDForce(VecDeriv& df, const VecDeriv& dx);
    virtual void addDForce(VecDeriv& df, const VecDeriv& dx, double kFactor, double);

    virtual void addGravityToV(double dt);

    virtual void draw();

    virtual bool addBBox(double* minBBox, double* maxBBox);

protected:
    virtual void computeElementMasses( ); ///< compute the mass matrices
    Real integrateVolume( int signx, int signy, int signz, Real l0, Real l1, Real l2 );
    virtual void computeElementMass( ElementMass &Mass, Real& totalMass, const helper::fixed_array<Coord,8> &nodes); ///< compute the mass matrix of an element

    void computeParticleMasses();

    void computeLumpedMasses();

    double getElementMass(unsigned int index);

    void setDensity(Real d) {_density.setValue( d );}
    Real getDensity() {return _density.getValue();}

protected :

    Data<Real> _density;
    Data<bool> _useLumpedMass;

    HexahedronData<ElementMass> _elementMasses; ///< mass matrices per element
    HexahedronData<Real>		_elementTotalMass; ///< total mass per element

    PointData<Real> _particleMasses; ///< masses per particle in order to compute gravity
    PointData<Coord> _lumpedMasses; ///< masses per particle computed by lumping mass matrices
};

#if defined(WIN32) && !defined(SOFA_COMPONENT_FORCEFIELD_HEXAHEDRALFEMFORCEFIELDANDMASS_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_FORCEFIELD_API HexahedralFEMForceFieldAndMass<defaulttype::Vec3dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_FORCEFIELD_API HexahedralFEMForceFieldAndMass<defaulttype::Vec3fTypes>;
#endif
#endif

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
