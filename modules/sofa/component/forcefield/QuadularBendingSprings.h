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
#ifndef SOFA_COMPONENT_FORCEFIELD_QUADULARBENDINGSPRINGS_H
#define SOFA_COMPONENT_FORCEFIELD_QUADULARBENDINGSPRINGS_H

#include <sofa/component/MechanicalObject.h>
#include <map>
#include <set>

#include <sofa/core/componentmodel/behavior/ForceField.h> // or "BaseForceField.h" ?
#include <sofa/core/VisualModel.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/component/topology/QuadData.h>
#include <sofa/component/topology/QuadData.h> // to comment ?
#include <sofa/component/topology/EdgeData.h>

#include <sofa/defaulttype/Mat.h>
#include <sofa/helper/fixed_array.h>

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
Bending springs added between vertices of quads sharing a common edge.
The springs connect the vertices not belonging to the common edge. It compresses when the surface bends along the common edge.
*/
template<class DataTypes>
class QuadularBendingSprings : public core::componentmodel::behavior::ForceField<DataTypes>, public core::VisualModel
{
public:
    typedef core::componentmodel::behavior::ForceField<DataTypes> Inherited;
    //typedef typename DataTypes::Real Real;
    typedef typename DataTypes::VecCoord VecCoord;

    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;

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

    class EdgeInformation
    {
    public:
        Mat3 DfDx; /// the edge stiffness matrix

        int     m1, m2;  /// the two extremities of the first spring: masses m1 and m2
        int     m3, m4;  /// the two extremities of the second spring: masses m3 and m4

        double  ks;      /// spring stiffness (initialized to the default value)
        double  kd;      /// damping factor (initialized to the default value)

        double  restlength1; /// rest length of the first spring
        double  restlength2; /// rest length of the second spring

        bool is_activated;

        bool is_initialized;

        EdgeInformation(int m1=0, int m2=0, int m3=0, int m4=0, double restlength1=0.0, double restlength2=0.0, bool is_activated=false, bool is_initialized=false)
            : m1(m1), m2(m2), m3(m3), m4(m4), restlength1(restlength1), restlength2(restlength2), is_activated(is_activated), is_initialized(is_initialized)
        {
        }
    };

    EdgeData<EdgeInformation> edgeInfo;

    QuadSetTopology<DataTypes> * _mesh;

    bool updateMatrix;

    Data<double> f_ks;
    Data<double> f_kd;

public:

    QuadularBendingSprings();

    QuadSetTopology<DataTypes> *getQuadularTopology() const {return _mesh;}

    ~QuadularBendingSprings();

    virtual double getPotentialEnergy(const VecCoord& x);

    /// Searches quad topology and creates the bending springs
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

    // -- VisualModel interface
    void draw();
    void initTextures() { };
    void update() { };

protected:

    EdgeData<EdgeInformation> &getEdgeInfo() {return edgeInfo;}

    static void QuadularBSEdgeCreationFunction(int edgeIndex, void* param,
            EdgeInformation &ei,
            const Edge& ,  const sofa::helper::vector< unsigned int > &,
            const sofa::helper::vector< double >&);

    static void QuadularBSQuadCreationFunction(const sofa::helper::vector<unsigned int> &quadAdded,
            void* param, vector<EdgeInformation> &edgeData);

    static void QuadularBSQuadDestructionFunction ( const sofa::helper::vector<unsigned int> &quadAdded,
            void* param, vector<EdgeInformation> &edgeData);

    double m_potentialEnergy;

    //typedef std::pair<unsigned,unsigned> IndexPair;
    //void addSpring( unsigned, unsigned, std::set<IndexPair>& );

    //// void registerEdge( IndexPair, IndexPair, std::map<IndexPair, IndexPair>&, std::set<IndexPair>&);

    //component::MechanicalObject<DataTypes>* dof;

};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
