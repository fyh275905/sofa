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
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MASS_H
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MASS_H

#include <sofa/core/componentmodel/behavior/BaseMass.h>
#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/defaulttype/Vec.h>
namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

/**
 *  \brief Component responsible for mass-related computations (gravity, acceleration).
 *
 *  Mass can be defined either as a scalar, vector, or a full mass-matrix.
 *  It is responsible for converting forces to accelerations (for explicit integrators),
 *  or displacements to forces (for implicit integrators).
 *
 *  It is also a ForceField, computing gravity-related forces.
 */
template<class DataTypes>
class Mass : virtual public ForceField<DataTypes>, public BaseMass
{
public:
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    //Constraint typedef
    typedef typename DataTypes::VecConst VecConst;
    typedef typename DataTypes::SparseVecDeriv SparseVecDeriv;
    typedef typename DataTypes::SparseDeriv SparseDeriv;

    Mass(MechanicalState<DataTypes> *mm = NULL);

    virtual ~Mass();

    /// @name Vector operations
    /// @{

    /// f += factor M dx
    ///
    /// This method retrieves the force and dx vector and call the internal
    /// addMDx(VecDeriv&,const VecDeriv&) method implemented by the component.
    virtual void addMDx(double factor = 1.0);

    /// dx = M^-1 f
    ///
    /// This method retrieves the force and dx vector and call the internal
    /// accFromF(VecDeriv&,const VecDeriv&) method implemented by the component.
    virtual void accFromF();

    /// f += factor M dx
    ///
    /// This method must be implemented by the component.
    virtual void addMDx(VecDeriv& f, const VecDeriv& dx, double factor = 1.0) = 0;

    /// dx = M^-1 f
    ///
    /// This method must be implemented by the component.
    virtual void accFromF(VecDeriv& a, const VecDeriv& f) = 0;

    /// Mass forces (gravity) often have null derivative
    virtual void addDForce(VecDeriv& /*df*/, const VecDeriv& /*dx*/, double /*kFactor*/, double /*bFactor*/)
    {}

    /// vMv/2 using dof->getV()
    ///
    /// This method retrieves the velocity vector and call the internal
    /// getKineticEnergy(const VecDeriv&) method implemented by the component.
    virtual double getKineticEnergy();

    //virtual double getPotentialEnergy();

    /// vMv/2
    ///
    /// This method must be implemented by the component.
    virtual double getKineticEnergy( const VecDeriv& v )=0;

    /// Accumulate the contribution of M, B, and/or K matrices multiplied
    /// by the dx vector with the given coefficients.
    ///
    /// This method computes
    /// $ df += mFactor M dx + bFactor B dx + kFactor K dx $
    /// For masses, it calls both addMdx and addDForce (which is often empty).
    ///
    /// \param mFact coefficient for mass contributions (i.e. second-order derivatives term in the ODE)
    /// \param bFact coefficient for damping contributions (i.e. first derivatives term in the ODE)
    /// \param kFact coefficient for stiffness contributions (i.e. DOFs term in the ODE)
    virtual void addMBKdx(double mFactor, double bFactor, double kFactor);

    /// Accumulate the contribution of M, B, and/or K matrices multiplied
    /// by the v vector with the given coefficients.
    ///
    /// This method computes
    /// $ df += mFactor M v + bFactor B v + kFactor K v $
    /// For masses, it calls both addMdx and addDForce (which is often empty).
    ///
    /// \param mFact coefficient for mass contributions (i.e. second-order derivatives term in the ODE)
    /// \param bFact coefficient for damping contributions (i.e. first derivatives term in the ODE)
    /// \param kFact coefficient for stiffness contributions (i.e. DOFs term in the ODE)
    virtual void addMBKv(double mFactor, double bFactor, double kFactor);

    /// Compute the system matrix corresponding to m M + b B + k K
    ///
    /// \param matrix matrix to add the result to
    /// \param mFact coefficient for mass contributions (i.e. second-order derivatives term in the ODE)
    /// \param bFact coefficient for damping contributions (i.e. first derivatives term in the ODE)
    /// \param kFact coefficient for stiffness contributions (i.e. DOFs term in the ODE)
    /// \param offset current row/column offset
    virtual void addMBKToMatrix(sofa::defaulttype::BaseMatrix * matrix, double mFact, double bFact, double kFact, unsigned int &offset);

    /// initialization to export kinetic and potential energy to gnuplot files format
    virtual void initGnuplot(const std::string path);

    /// export kinetic and potential energy state at "time" to a gnuplot file
    virtual void exportGnuplot(double time);

    /// perform  v += dt*g operation. Used if mass wants to added G separately from the other forces to v.
    virtual void addGravityToV(double dt)=0;


    virtual void buildSystemMatrix(defaulttype::BaseMatrix &/* invM_Jtrans */, defaulttype::BaseMatrix &/* A */,
            const sofa::helper::vector< sofa::helper::vector<unsigned int>  >&/* constraintId */,
            const sofa::helper::vector< double > /* factor */,
            const sofa::helper::vector< unsigned int > /* offset */,
            const defaulttype::BaseVector& /* FixedPoints */);

    virtual void buildInvMassDenseMatrix(defaulttype::BaseMatrix &m);

protected:
    /// stream to export Kinematic, Potential and Mechanical Energy to gnuplot files
    std::ofstream* m_gnuplotFileEnergy;

    /// @}
};

/** Return the inertia force applied to a body referenced in a moving coordinate system.
\param sv spatial velocity (omega, vorigin) of the coordinate system
\param a acceleration of the origin of the coordinate system
\param m mass of the body
\param x position of the body in the moving coordinate system
\param v velocity of the body in the moving coordinate system
This default implementation returns no inertia.
*/
template<class Coord, class Deriv, class Vec, class M, class SV>
Deriv inertiaForce( const SV& /*sv*/, const Vec& /*a*/, const M& /*m*/, const Coord& /*x*/, const Deriv& /*v*/ )
{
    return Deriv();
    //const Deriv& omega=sv.getAngularVelocity();
    //return -( a + omega.cross( omega.cross(x) + v*2 ))*m;
}

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
