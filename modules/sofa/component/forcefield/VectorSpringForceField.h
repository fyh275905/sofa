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
#ifndef SOFA_COMPONENT_FORCEFIELD_VECTORSPRINGFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_VECTORSPRINGFORCEFIELD_H

#include <sofa/component/forcefield/SpringForceField.h>
#include <sofa/core/behavior/ForceField.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/component/topology/EdgeData.h>
#include <sofa/component/topology/TopologyChangedEvent.h>
#include <sofa/core/objectmodel/DataFileName.h>

#include <sofa/component/topology/EdgeSetGeometryAlgorithms.h>
#include <sofa/component/topology/EdgeSetTopologyModifier.h>


namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;
using sofa::core::objectmodel::Event;

template<class DataTypes>
class VectorSpringForceField
    : public core::behavior::PairInteractionForceField<DataTypes>
//: public core::behavior::ForceField<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(VectorSpringForceField, DataTypes), SOFA_TEMPLATE(core::behavior::PairInteractionForceField, DataTypes));

    typedef typename core::behavior::PairInteractionForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    typedef core::behavior::MechanicalState<DataTypes> MechanicalState;
    enum { N=Coord::static_size };

    struct Spring
    {
        Real  ks;          ///< spring stiffness
        Real  kd;          ///< damping factor
        Deriv restVector;  ///< rest vector of the spring

        Spring(Real _ks, Real _kd, Deriv _rl) : ks(_ks), kd(_kd), restVector(_rl)
        {
        }
        Spring() : ks(1.0), kd(1.0)
        {
        }

        /// Output stream
        inline friend std::ostream& operator<< ( std::ostream& os, const Spring& /*s*/ )
        {
            return os;
        }

        /// Input stream
        inline friend std::istream& operator>> ( std::istream& in, Spring& /*s*/ )
        {
            return in;
        }
    };
protected:

    double m_potentialEnergy;
    /// true if the springs are initialized from the topology
    bool useTopology;

    /// indices in case we don't use the topology
    sofa::helper::vector<topology::Edge> edgeArray;
    /// where the springs information are stored
    sofa::component::topology::EdgeData<Spring> springArray;

    /// the filename where to load the spring information
    sofa::core::objectmodel::DataFileName m_filename;
    /// By default, assume that all edges have the same stiffness
    Data<double> m_stiffness;
    /// By default, assume that all edges have the same viscosity
    Data<double> m_viscosity;

    void resizeArray(unsigned int n);

    static void springCreationFunction(int /*index*/,
            void* param, Spring& t,
            const topology::Edge& e,
            const sofa::helper::vector< unsigned int > &ancestors,
            const sofa::helper::vector< double >& coefs);

    bool usingMask;
public:

    sofa::core::topology::BaseMeshTopology* _topology;
    sofa::component::topology::EdgeSetTopologyContainer* edgeCont;
    sofa::component::topology::EdgeSetGeometryAlgorithms<DataTypes>* edgeGeo;
    sofa::component::topology::EdgeSetTopologyModifier* edgeMod;

    VectorSpringForceField(MechanicalState* _object=NULL);

    VectorSpringForceField(MechanicalState* _object1, MechanicalState* _object2);

    bool load(const char *filename);

    core::behavior::MechanicalState<DataTypes>* getObject1() { return this->mstate1; }
    core::behavior::MechanicalState<DataTypes>* getObject2() { return this->mstate2; }

    virtual void init();
    virtual void bwdInit();

    void createDefaultSprings();

    virtual void handleEvent( Event* e );

    virtual void addForce(VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2);
    //virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    virtual void addDForce(VecDeriv& df1, VecDeriv& df2, const VecDeriv& dx1, const VecDeriv& dx2, double kFactor, double bFactor);
    //virtual void addDForce (VecDeriv& df, const VecDeriv& dx);

    //virtual double getPotentialEnergy(const VecCoord& ) const
    virtual double getPotentialEnergy(const VecCoord&, const VecCoord&) const
    { return m_potentialEnergy; }

    Real getStiffness() const
    {
        return (Real)(m_stiffness.getValue());
    }
    const Real getViscosity() const
    {
        return (Real)(m_viscosity.getValue());
    }
    const topology::EdgeData<Spring>& getSpringArray() const
    {
        return springArray;
    }

    void draw();

    // -- Modifiers

    void clear(int reserve=0)
    {
        helper::vector<Spring>& springArrayData = *(springArray.beginEdit());
        springArrayData.clear();
        if (reserve) springArrayData.reserve(reserve);
        springArray.endEdit();
    }

    bool useMask() {return true;}
    void addSpring(int m1, int m2, SReal ks, SReal kd, Coord restVector);

    /// forward declaration of the loader class used to read spring information from file
    class Loader;
    friend class Loader;

};

#if defined(WIN32) && !defined(SOFA_COMPONENT_FORCEFIELD_VECTORSPRINGFORCEFIELD_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_FORCEFIELD_API VectorSpringForceField<defaulttype::Vec3dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_FORCEFIELD_API VectorSpringForceField<defaulttype::Vec3fTypes>;
#endif
#endif

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
