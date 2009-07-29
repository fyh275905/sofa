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
//
// C++ Interface: TriangularBendingSprings
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SOFA_COMPONENT_FORCEFIELD_TRIANGULARBENDINGSPRINGS_H
#define SOFA_COMPONENT_FORCEFIELD_TRIANGULARBENDINGSPRINGS_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

//#include <sofa/component/forcefield/StiffSpringForceField.h>
#include <sofa/component/container/MechanicalObject.h>
#include <map>

#include <sofa/core/componentmodel/behavior/ForceField.h> // or "BaseForceField.h" ?
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/Mat.h>

#include <sofa/defaulttype/Mat.h>
#include <sofa/component/topology/EdgeData.h>

namespace sofa
{

namespace component
{

namespace forcefield
{
using namespace sofa::helper;
using namespace sofa::defaulttype;
using namespace sofa::component::topology;

/**
Bending springs added between vertices of triangles sharing a common edge.
The springs connect the vertices not belonging to the common edge. It compresses when the surface bends along the common edge.


	@author The SOFA team </www.sofa-framework.org>
*/
template<class DataTypes>
class TriangularBendingSprings : public core::componentmodel::behavior::ForceField<DataTypes>, public virtual core::objectmodel::BaseObject
{
public:
    typedef core::componentmodel::behavior::ForceField<DataTypes> Inherited;
    //typedef typename DataTypes::Real Real;
    typedef typename DataTypes::VecCoord VecCoord;


    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
    //typedef core::componentmodel::behavior::MechanicalState<DataTypes> MechanicalState;

    enum { N=Coord::static_size };
    typedef defaulttype::Mat<N,N,Real> Mat;

    class Mat3 : public fixed_array<Deriv,3>
    {
    public:
        Deriv operator*(const Deriv& v)
        {
            return Deriv((*this)[0]*v,(*this)[1]*v,(*this)[2]*v);
        }
        Deriv transposeMultiply(const Deriv& v)
        {
            return Deriv(v[0]*((*this)[0])[0]+v[1]*((*this)[1])[0]+v[2]*((*this)[2][0]),
                    v[0]*((*this)[0][1])+v[1]*((*this)[1][1])+v[2]*((*this)[2][1]),
                    v[0]*((*this)[0][2])+v[1]*((*this)[1][2])+v[2]*((*this)[2][2]));
        }
    };

protected:

    //Data<double> ks;
    //Data<double> kd;

    class EdgeInformation
    {
    public:
        Mat3 DfDx; /// the edge stiffness matrix

        int     m1, m2;  /// the two extremities of the spring: masses m1 and m2

        double  ks;      /// spring stiffness (initialized to the default value)
        double  kd;      /// damping factor (initialized to the default value)

        double  restlength; /// rest length of the spring

        bool is_activated;

        bool is_initialized;

        EdgeInformation(int m1=0, int m2=0, /* double ks=getKs(), double kd=getKd(), */ double restlength=0.0, bool is_activated=false, bool is_initialized=false)
            : m1(m1), m2(m2), /* ks(ks), kd(kd), */ restlength(restlength), is_activated(is_activated), is_initialized(is_initialized)
        {
        }
        /// Output stream
        inline friend std::ostream& operator<< ( std::ostream& os, const EdgeInformation& /*ei*/ )
        {
            return os;
        }

        /// Input stream
        inline friend std::istream& operator>> ( std::istream& in, EdgeInformation& /*ei*/ )
        {
            return in;
        }
    };

    EdgeData<EdgeInformation> edgeInfo;

    sofa::core::componentmodel::topology::BaseMeshTopology* _topology;

    bool updateMatrix;

    Data<double> f_ks;
    Data<double> f_kd;

public:

    TriangularBendingSprings(/*double _ks, double _kd*/); //component::container::MechanicalObject<DataTypes>* m_dof = NULL);
    //TriangularBendingSprings(); //MechanicalState<DataTypes> *mm1 = NULL, MechanicalState<DataTypes> *mm2 = NULL);

    virtual ~TriangularBendingSprings();

    virtual double getPotentialEnergy(const VecCoord& x);

    /// Searches triangle topology and creates the bending springs
    virtual void init();

    virtual void addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& v);
    virtual void addDForce(VecDeriv& df, const VecDeriv& dx);

    virtual double getKs() const { return f_ks.getValue();}
    virtual double getKd() const { return f_kd.getValue();}

    void setKs(const double ks)
    {
        f_ks.setValue((double)ks);
    }
    void setKd(const double kd)
    {
        f_kd.setValue((double)kd);
    }

    // handle topological changes
    virtual void handleTopologyChange();

    void draw();

protected:

    EdgeData<EdgeInformation> &getEdgeInfo() {return edgeInfo;}

    static void TriangularBSEdgeCreationFunction(int edgeIndex, void* param,
            EdgeInformation &ei,
            const Edge& ,  const sofa::helper::vector< unsigned int > &,
            const sofa::helper::vector< double >&);

    static void TriangularBSTriangleCreationFunction(const sofa::helper::vector<unsigned int> &triangleAdded,
            void* param, vector<EdgeInformation> &edgeData);

    static void TriangularBSTriangleDestructionFunction ( const sofa::helper::vector<unsigned int> &triangleRemoved,
            void* param, vector<EdgeInformation> &edgeData);

    double m_potentialEnergy;

    //public:
    //Data<double> ks;
    //Data<double> kd;

};

#if defined(WIN32) && !defined(SOFA_COMPONENT_FORCEFIELD_TRIANGULARBENDINGSPRINGS_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_FORCEFIELD_API TriangularBendingSprings<defaulttype::Vec3dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_FORCEFIELD_API TriangularBendingSprings<defaulttype::Vec3fTypes>;
#endif
#endif


} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
