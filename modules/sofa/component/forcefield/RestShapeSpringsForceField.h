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
#ifndef SOFA_COMPONENT_FORCEFIELD_RESTSHAPESPRINGFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_RESTSHAPESPRINGFORCEFIELD_H

#include <sofa/core/behavior/ForceField.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/objectmodel/Data.h>
#include <sofa/helper/vector.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

/** Apply constant forces to given degrees of freedom.  */
template<class DataTypes>
class RestShapeSpringsForceField : public core::behavior::ForceField<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(RestShapeSpringsForceField, DataTypes), SOFA_TEMPLATE(core::behavior::ForceField, DataTypes));

    typedef core::behavior::ForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    typedef helper::vector<unsigned int> VecIndex;
    typedef helper::vector<Real>	 VecReal;
public:

    Data< VecIndex > points;
    Data< VecReal > stiffness;
    Data< VecReal > angularStiffness;
    Data<std::string> external_rest_shape;
    Data< VecIndex > external_points;

    sofa::core::behavior::MechanicalState<DataTypes>* restMState;
    //sofa::core::behavior::MechanicalState<DataTypes>* restMState;
    bool useRestMState;
    VecDeriv Springs_dir;

    RestShapeSpringsForceField();
    ///
    void init();

    /// Add the forces
    virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    /// Constant force has null variation
    virtual void addDForce (VecDeriv& df, const VecDeriv& dx, double kFactor, double );


    virtual double getPotentialEnergy(const VecCoord& ) const
    {
        sout << "getPotentialEnergy not implemented" << sendl;
        return 0.0;
    };


    virtual void addKToMatrix(sofa::defaulttype::BaseMatrix * /*mat*/, double /*kFact*/, unsigned int &/*offset*/);


    virtual void draw();
    bool addBBox(double* minBBox, double* maxBBox);

};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
