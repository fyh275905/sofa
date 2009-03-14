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
#ifndef SOFA_COMPONENT_MAPPING_BARYCENTRICMAPPING_H
#define SOFA_COMPONENT_MAPPING_BARYCENTRICMAPPING_H

#include <sofa/helper/vector.h>
#include <sofa/component/topology/PointData.h>
#include <sofa/component/topology/HexahedronData.h>
#include <sofa/component/topology/RegularGridTopology.h>
#include <sofa/core/Mapping.h>
#include <sofa/core/componentmodel/behavior/MechanicalMapping.h>
#include <sofa/core/componentmodel/behavior/MappedModel.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>

// forward declarations
namespace sofa
{
namespace core
{
namespace componentmodel
{
namespace topology
{
class BaseMeshTopology;
}
}
}

namespace component
{
namespace topology
{
class MeshTopology;
class RegularGridTopology;
class SparseGridTopology;

class EdgeSetTopologyContainer;
template <class T>
class EdgeSetGeometryAlgorithms;

class TriangleSetTopologyContainer;
template <class T>
class TriangleSetGeometryAlgorithms;

class QuadSetTopologyContainer;
template <class T>
class QuadSetGeometryAlgorithms;

class TetrahedronSetTopologyContainer;
template <class T>
class TetrahedronSetGeometryAlgorithms;

class HexahedronSetTopologyContainer;
template <class T>
class HexahedronSetGeometryAlgorithms;
}
}
}

namespace sofa
{

namespace component
{

namespace mapping
{
/// Base class for barycentric mapping topology-specific mappers
template<class In, class Out>
class BarycentricMapper
{
public:
    typedef typename In::Real Real;
    typedef typename Out::Real OutReal;

    typedef typename In::VecDeriv InVecDeriv;
    typedef typename In::Deriv InDeriv;
    typedef typename std::map<unsigned int, InDeriv>::const_iterator InConstraintIterator;

    typedef typename Out::VecDeriv OutVecDeriv;
    typedef typename Out::Deriv OutDeriv;
    typedef typename std::map<unsigned int, OutDeriv>::const_iterator OutConstraintIterator;

protected:
    template< int NC,  int NP>
    class MappingData
    {
    public:
        int in_index;
        //unsigned int points[NP];
        Real baryCoords[NC];

        inline friend std::istream& operator >> ( std::istream& in, MappingData< NC, NP> &m )
        {
            in>>m.in_index;
            for (int i=0; i<NC; i++) in >> m.baryCoords[i];
            return in;
        }

        inline friend std::ostream& operator << ( std::ostream& out, const MappingData< NC , NP > & m )
        {
            out << m.in_index;
            for (int i=0; i<NC; i++)
                out << " " << m.baryCoords[i];
            out << "\n";
            return out;
        }

    };

public:
    typedef MappingData<1,2> LineData;
    typedef MappingData<2,3> TriangleData;
    typedef MappingData<2,4> QuadData;
    typedef MappingData<3,4> TetraData;
    typedef MappingData<3,8> CubeData;
    typedef MappingData<1,0> MappingData1D;
    typedef MappingData<2,0> MappingData2D;
    typedef MappingData<3,0> MappingData3D;

    virtual ~BarycentricMapper() {}
    virtual void init(const typename Out::VecCoord& out, const typename In::VecCoord& in) = 0;
    virtual void apply( typename Out::VecCoord& out, const typename In::VecCoord& in ) = 0;
    virtual void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in ) = 0;
    virtual void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in ) = 0;
    virtual void applyJT( typename In::VecConst& out, const typename Out::VecConst& in ) = 0;
    virtual void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in) = 0;

    //-- test mapping partiel
    virtual void applyOnePoint( const unsigned int& /*hexaId*/, typename Out::VecCoord& /*out*/, const typename In::VecCoord& /*in*/)
    {};
    //--

    virtual void clear( int reserve=0 ) =0;

    // TODO: make it pure virtual. Get the barycentric coefficients and the coordinates of the parents of a given mapped dof.
    virtual void getJ(unsigned int /*Idx*/, sofa::helper::vector< double > &/*factor*/, sofa::helper::vector< unsigned int > &/*indices*/)
    {std::cerr<< "getJ NOT implemented yet\n"; };

    //Nothing to do
    inline friend std::istream& operator >> ( std::istream& in, BarycentricMapper< In, Out > & ) {return in;}
    inline friend std::ostream& operator << ( std::ostream& out, const BarycentricMapper< In, Out > &  ) { return out; }
};

/// Template class for barycentric mapping topology-specific mappers.
template<class In, class Out>
class TopologyBarycentricMapper : public BarycentricMapper<In,Out>
{
public:
    typedef BarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;

    virtual ~TopologyBarycentricMapper() {}

    virtual int addPointInLine(const int /*lineIndex*/, const SReal* /*baryCoords*/) {return 0;}
    virtual int setPointInLine(const int /*pointIndex*/, const int /*lineIndex*/, const SReal* /*baryCoords*/) {return 0;}
    virtual int createPointInLine(const typename Out::Coord& /*p*/, int /*lineIndex*/, const typename In::VecCoord* /*points*/) {return 0;}

    virtual int addPointInTriangle(const int /*triangleIndex*/, const SReal* /*baryCoords*/) {return 0;}
    virtual int setPointInTriangle(const int /*pointIndex*/, const int /*triangleIndex*/, const SReal* /*baryCoords*/) {return 0;}
    virtual int createPointInTriangle(const typename Out::Coord& /*p*/, int /*triangleIndex*/, const typename In::VecCoord* /*points*/) {return 0;}

    virtual int addPointInQuad(const int /*quadIndex*/, const SReal* /*baryCoords*/) {return 0;}
    virtual int setPointInQuad(const int /*pointIndex*/, const int /*quadIndex*/, const SReal* /*baryCoords*/) {return 0;}
    virtual int createPointInQuad(const typename Out::Coord& /*p*/, int /*quadIndex*/, const typename In::VecCoord* /*points*/) {return 0;}

    virtual int addPointInTetra(const int /*tetraIndex*/, const SReal* /*baryCoords*/) {return 0;}
    virtual int setPointInTetra(const int /*pointIndex*/, const int /*tetraIndex*/, const SReal* /*baryCoords*/) {return 0;}
    virtual int createPointInTetra(const typename Out::Coord& /*p*/, int /*tetraIndex*/, const typename In::VecCoord* /*points*/) {return 0;}

    virtual int addPointInCube(const int /*cubeIndex*/, const SReal* /*baryCoords*/) {return 0;}
    virtual int setPointInCube(const int /*pointIndex*/, const int /*cubeIndex*/, const SReal* /*baryCoords*/) {return 0;}
    virtual int createPointInCube(const typename Out::Coord& /*p*/, int /*cubeIndex*/, const typename In::VecCoord* /*points*/) {return 0;}

protected:
    TopologyBarycentricMapper(core::componentmodel::topology::BaseMeshTopology* topology)
        : topology(topology)
    {}

protected:
    core::componentmodel::topology::BaseMeshTopology* topology;
};


/// Class allowing barycentric mapping computation on a MeshTopology
template<class In, class Out>
class BarycentricMapperMeshTopology : public TopologyBarycentricMapper<In,Out>
{
public:
    typedef TopologyBarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::OutConstraintIterator OutConstraintIterator;
    typedef typename Inherit::OutDeriv  OutDeriv;
    typedef typename Inherit::InConstraintIterator  InConstraintIterator;
    typedef typename Inherit::InDeriv  InDeriv;
    typedef typename Inherit::MappingData1D MappingData1D;
    typedef typename Inherit::MappingData2D MappingData2D;
    typedef typename Inherit::MappingData3D MappingData3D;
protected:
    sofa::helper::vector< MappingData1D >  map1d;
    sofa::helper::vector< MappingData2D >  map2d;
    sofa::helper::vector< MappingData3D >  map3d;

public:
    BarycentricMapperMeshTopology(core::componentmodel::topology::BaseMeshTopology* topology)
        : TopologyBarycentricMapper<In,Out>(topology)
    {}

    virtual ~BarycentricMapperMeshTopology() {}

    void clear(int reserve=0);

    int addPointInLine(const int lineIndex, const SReal* baryCoords);
    int createPointInLine(const typename Out::Coord& p, int lineIndex, const typename In::VecCoord* points);

    int addPointInTriangle(const int triangleIndex, const SReal* baryCoords);
    int createPointInTriangle(const typename Out::Coord& p, int triangleIndex, const typename In::VecCoord* points);

    int addPointInQuad(const int quadIndex, const SReal* baryCoords);
    int createPointInQuad(const typename Out::Coord& p, int quadIndex, const typename In::VecCoord* points);

    int addPointInTetra(const int tetraIndex, const SReal* baryCoords);

    int addPointInCube(const int cubeIndex, const SReal* baryCoords);

    void init(const typename Out::VecCoord& out, const typename In::VecCoord& in);

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    void getJ(unsigned int Idx, sofa::helper::vector< double > &factor, sofa::helper::vector< unsigned int > &indices);

    inline friend std::istream& operator >> ( std::istream& in, BarycentricMapperMeshTopology<In, Out> &b )
    {
        unsigned int size_vec;
        in >> size_vec;
        b.map1d.clear();
        MappingData1D value1d;
        for (unsigned int i=0; i<size_vec; i++)
        {
            in >> value1d;
            b.map1d.push_back(value1d);
        }

        in >> size_vec;
        b.map2d.clear();
        MappingData2D value2d;
        for (unsigned int i=0; i<size_vec; i++)
        {
            in >> value2d;
            b.map2d.push_back(value2d);
        }

        in >> size_vec;
        b.map3d.clear();
        MappingData3D value3d;
        for (unsigned int i=0; i<size_vec; i++)
        {
            in >> value3d;
            b.map3d.push_back(value3d);
        }
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const BarycentricMapperMeshTopology<In, Out> & b )
    {

        out << b.map1d.size();
        out << " " ;
        out << b.map1d;
        out << " " ;
        out << b.map2d.size();
        out << " " ;
        out << b.map2d;
        out << " " ;
        out << b.map3d.size();
        out << " " ;
        out << b.map3d;

        return out;
    }

private:
    void clear1d(int reserve=0);
    void clear2d(int reserve=0);
    void clear3d(int reserve=0);

};
/// Class allowing barycentric mapping computation on a RegularGridTopology
template<class In, class Out>
class BarycentricMapperRegularGridTopology : public TopologyBarycentricMapper<In,Out>
{
public:
    typedef TopologyBarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::OutConstraintIterator OutConstraintIterator;
    typedef typename Inherit::OutDeriv  OutDeriv;
    typedef typename Inherit::InConstraintIterator  InConstraintIterator;
    typedef typename Inherit::InDeriv  InDeriv;
    typedef typename Inherit::CubeData CubeData;
protected:
    sofa::helper::vector<CubeData> map;
    topology::RegularGridTopology* topology;

public:
    BarycentricMapperRegularGridTopology(topology::RegularGridTopology* topology)
        : Inherit(topology),topology(topology)
    {}

    virtual ~BarycentricMapperRegularGridTopology() {}

    void clear(int reserve=0);

    bool isEmpty() {return this->map.size() == 0;}
    void setTopology(topology::RegularGridTopology* _topology) {this->topology = _topology;}
    topology::RegularGridTopology *getTopology() {return dynamic_cast<topology::RegularGridTopology *>(this->topology);}

    int addPointInCube(const int cubeIndex, const SReal* baryCoords);

    void init(const typename Out::VecCoord& out, const typename In::VecCoord& in);
    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    inline friend std::istream& operator >> ( std::istream& in, BarycentricMapperRegularGridTopology<In, Out> &b )
    {
        in >> b.map;
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const BarycentricMapperRegularGridTopology<In, Out> & b )
    {
        out << b.map;
        return out;
    }

};


/// Class allowing barycentric mapping computation on a SparseGridTopology
template<class In, class Out>
class BarycentricMapperSparseGridTopology : public TopologyBarycentricMapper<In,Out>
{
public:
    typedef TopologyBarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::OutConstraintIterator OutConstraintIterator;
    typedef typename Inherit::OutDeriv  OutDeriv;
    typedef typename Inherit::InConstraintIterator  InConstraintIterator;
    typedef typename Inherit::InDeriv  InDeriv;

    typedef typename Inherit::CubeData CubeData;
protected:
    sofa::helper::vector<CubeData> map;
    topology::SparseGridTopology* topology;
public:
    BarycentricMapperSparseGridTopology(topology::SparseGridTopology* topology)
        : TopologyBarycentricMapper<In,Out>(topology),
          topology(topology)
    {}

    virtual ~BarycentricMapperSparseGridTopology() {}

    void clear(int reserve=0);

    int addPointInCube(const int cubeIndex, const SReal* baryCoords);

    void init(const typename Out::VecCoord& out, const typename In::VecCoord& in);

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    inline friend std::istream& operator >> ( std::istream& in, BarycentricMapperSparseGridTopology<In, Out> &b )
    {
        in >> b.map;
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const BarycentricMapperSparseGridTopology<In, Out> & b )
    {
        out << b.map;
        return out;
    }

};


/// Base class for barycentric mapping topology-specific mappers. Enables topological changes.
class BarycentricMapperDynamicTopology
{
public:
    virtual ~BarycentricMapperDynamicTopology() {}

    // handle topology changes in the From topology
    virtual void handleTopologyChange()=0;

    // handle topology changes in the To topology
    virtual void handlePointEvents(std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator,
            std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator )=0;
protected:
    BarycentricMapperDynamicTopology() {}
};


/// Class allowing barycentric mapping computation on a EdgeSetTopology
template<class In, class Out>
class BarycentricMapperEdgeSetTopology : public TopologyBarycentricMapper<In,Out>,
    public BarycentricMapperDynamicTopology

{
public:
    typedef TopologyBarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::OutConstraintIterator OutConstraintIterator;
    typedef typename Inherit::OutDeriv  OutDeriv;
    typedef typename Inherit::InConstraintIterator  InConstraintIterator;
    typedef typename Inherit::InDeriv  InDeriv;
    typedef typename Inherit::MappingData1D MappingData;

protected:
    topology::PointData< MappingData >  map;
    topology::EdgeSetTopologyContainer*			_container;
    topology::EdgeSetGeometryAlgorithms<In>*	_geomAlgo;

public:
    BarycentricMapperEdgeSetTopology(topology::EdgeSetTopologyContainer* topology)
        : TopologyBarycentricMapper<In,Out>(topology),
          _container(topology),
          _geomAlgo(NULL)
    {}

    virtual ~BarycentricMapperEdgeSetTopology() {}

    void clear(int reserve=0);

    int addPointInLine(const int edgeIndex, const SReal* baryCoords);
    int createPointInLine(const typename Out::Coord& p, int edgeIndex, const typename In::VecCoord* points);

    void init(const typename Out::VecCoord& out, const typename In::VecCoord& in);

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    // handle topology changes in the From topology
    virtual void handleTopologyChange();
    // handle topology changes in the To topology
    virtual void handlePointEvents(std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator,
            std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator);

    inline friend std::istream& operator >> ( std::istream& in, BarycentricMapperEdgeSetTopology<In, Out> &b )
    {
        unsigned int size_vec;

        in >> size_vec;
        sofa::helper::vector<MappingData>& m = *(b.map.beginEdit());
        m.clear();

        MappingData value;
        for (unsigned int i=0; i<size_vec; i++)
        {
            in >> value;
            m.push_back(value);
        }
        b.map.endEdit();
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const BarycentricMapperEdgeSetTopology<In, Out> & b )
    {

        out << b.map.getValue().size();
        out << " " ;
        out << b.map;

        return out;
    }


};


/// Class allowing barycentric mapping computation on a TriangleSetTopology
template<class In, class Out>
class BarycentricMapperTriangleSetTopology : public TopologyBarycentricMapper<In,Out>,
    public BarycentricMapperDynamicTopology
{
public:
    typedef TopologyBarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::OutConstraintIterator OutConstraintIterator;
    typedef typename Inherit::OutDeriv  OutDeriv;
    typedef typename Inherit::InConstraintIterator  InConstraintIterator;
    typedef typename Inherit::InDeriv  InDeriv;
    typedef typename Inherit::MappingData2D MappingData;
protected:
    topology::PointData< MappingData >		map;
    topology::TriangleSetTopologyContainer*			_container;
    topology::TriangleSetGeometryAlgorithms<In>*	_geomAlgo;

public:
    BarycentricMapperTriangleSetTopology(topology::TriangleSetTopologyContainer* topology)
        : TopologyBarycentricMapper<In,Out>(topology),
          _container(topology),
          _geomAlgo(NULL)
    {}

    virtual ~BarycentricMapperTriangleSetTopology() {}

    void clear(int reserve=0);

    int addPointInTriangle(const int triangleIndex, const SReal* baryCoords);
    int createPointInTriangle(const typename Out::Coord& p, int triangleIndex, const typename In::VecCoord* points);

    void init(const typename Out::VecCoord& out, const typename In::VecCoord& in);

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    // handle topology changes in the From topology
    virtual void handleTopologyChange();
    // handle topology changes in the To topology
    virtual void handlePointEvents(std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator,
            std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator);

    inline friend std::istream& operator >> ( std::istream& in, BarycentricMapperTriangleSetTopology<In, Out> &b )
    {
        unsigned int size_vec;

        in >> size_vec;

        sofa::helper::vector<MappingData>& m = *(b.map.beginEdit());
        m.clear();
        MappingData value;
        for (unsigned int i=0; i<size_vec; i++)
        {
            in >> value;
            m.push_back(value);
        }
        b.map.endEdit();
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const BarycentricMapperTriangleSetTopology<In, Out> & b )
    {

        out << b.map.getValue().size();
        out << " " ;
        out << b.map;

        return out;
    }


};


/// Class allowing barycentric mapping computation on a QuadSetTopology
template<class In, class Out>
class BarycentricMapperQuadSetTopology : public TopologyBarycentricMapper<In,Out>,
    public BarycentricMapperDynamicTopology
{
public:
    typedef TopologyBarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::OutConstraintIterator OutConstraintIterator;
    typedef typename Inherit::OutDeriv  OutDeriv;
    typedef typename Inherit::InConstraintIterator  InConstraintIterator;
    typedef typename Inherit::InDeriv  InDeriv;
    typedef typename Inherit::MappingData2D MappingData;
protected:
    topology::PointData< MappingData >  map;
    topology::QuadSetTopologyContainer*			_container;
    topology::QuadSetGeometryAlgorithms<In>*	_geomAlgo;

public:
    BarycentricMapperQuadSetTopology(topology::QuadSetTopologyContainer* topology)
        : TopologyBarycentricMapper<In,Out>(topology),
          _container(topology),
          _geomAlgo(NULL)
    {}

    virtual ~BarycentricMapperQuadSetTopology() {}

    void clear(int reserve=0);

    int addPointInQuad(const int index, const SReal* baryCoords);
    int createPointInQuad(const typename Out::Coord& p, int index, const typename In::VecCoord* points);

    void init(const typename Out::VecCoord& out, const typename In::VecCoord& in);

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    // handle topology changes in the From topology
    virtual void handleTopologyChange();
    // handle topology changes in the To topology
    virtual void handlePointEvents(std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator,
            std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator);

    inline friend std::istream& operator >> ( std::istream& in, BarycentricMapperQuadSetTopology<In, Out> &b )
    {
        unsigned int size_vec;

        in >> size_vec;
        sofa::helper::vector<MappingData>& m = *(b.map.beginEdit());
        m.clear();
        MappingData value;
        for (unsigned int i=0; i<size_vec; i++)
        {
            in >> value;
            m.push_back(value);
        }
        b.map.endEdit();
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const BarycentricMapperQuadSetTopology<In, Out> & b )
    {

        out << b.map.getValue().size();
        out << " " ;
        out << b.map;

        return out;
    }

};

/// Class allowing barycentric mapping computation on a TetrehedronSetTopology
template<class In, class Out>
class BarycentricMapperTetrahedronSetTopology : public TopologyBarycentricMapper<In,Out>,
    public BarycentricMapperDynamicTopology
{
public:
    typedef TopologyBarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::OutConstraintIterator OutConstraintIterator;
    typedef typename Inherit::OutDeriv  OutDeriv;
    typedef typename Inherit::InConstraintIterator  InConstraintIterator;
    typedef typename Inherit::InDeriv  InDeriv;
    typedef typename Inherit::MappingData3D MappingData;
protected:
    topology::PointData< MappingData >  map;
    topology::TetrahedronSetTopologyContainer*			_container;
    topology::TetrahedronSetGeometryAlgorithms<In>*	_geomAlgo;

public:
    BarycentricMapperTetrahedronSetTopology(topology::TetrahedronSetTopologyContainer* topology)
        : TopologyBarycentricMapper<In,Out>(topology),
          _container(topology),
          _geomAlgo(NULL)
    {}

    virtual ~BarycentricMapperTetrahedronSetTopology() {}

    void clear(int reserve=0);

    int addPointInTetra(const int index, const SReal* baryCoords);

    void init(const typename Out::VecCoord& out, const typename In::VecCoord& in);

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    // handle topology changes in the From topology
    virtual void handleTopologyChange();
    // handle topology changes in the To topology
    virtual void handlePointEvents(std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator,
            std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator);

    inline friend std::istream& operator >> ( std::istream& in, BarycentricMapperTetrahedronSetTopology<In, Out> &b )
    {
        unsigned int size_vec;

        in >> size_vec;
        sofa::helper::vector<MappingData>& m = *(b.map.beginEdit());
        m.clear();
        MappingData value;
        for (unsigned int i=0; i<size_vec; i++)
        {
            in >> value;
            m.push_back(value);
        }
        b.map.endEdit();
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const BarycentricMapperTetrahedronSetTopology<In, Out> & b )
    {

        out << b.map.getValue().size();
        out << " " ;
        out << b.map;

        return out;
    }
};


/// Class allowing barycentric mapping computation on a HexahedronSetTopology
template<class In, class Out>
class BarycentricMapperHexahedronSetTopology : public TopologyBarycentricMapper<In,Out>,
    public BarycentricMapperDynamicTopology
{
public:
    typedef TopologyBarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::OutConstraintIterator OutConstraintIterator;
    typedef typename Inherit::OutDeriv  OutDeriv;
    typedef typename Inherit::InConstraintIterator  InConstraintIterator;
    typedef typename Inherit::InDeriv  InDeriv;
    typedef typename Inherit::MappingData3D MappingData;


protected:
    topology::PointData< MappingData >  map;
    topology::HexahedronSetTopologyContainer*		_container;
    topology::HexahedronSetGeometryAlgorithms<In>*	_geomAlgo;

    std::set<int>	_invalidIndex;

public:
    BarycentricMapperHexahedronSetTopology()
        : TopologyBarycentricMapper<In,Out>(NULL),_container(NULL),_geomAlgo(NULL)
    {}

    BarycentricMapperHexahedronSetTopology(topology::HexahedronSetTopologyContainer* topology)
        : TopologyBarycentricMapper<In,Out>(topology),
          _container(topology),
          _geomAlgo(NULL)
    {}

    virtual ~BarycentricMapperHexahedronSetTopology() {}

    void clear(int reserve=0);

    int addPointInCube(const int index, const SReal* baryCoords);

    int setPointInCube(const int pointIndex, const int cubeIndex, const SReal* baryCoords);

    void init(const typename Out::VecCoord& out, const typename In::VecCoord& in);

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    //-- test mapping partiel
    void applyOnePoint( const unsigned int& hexaId, typename Out::VecCoord& out, const typename In::VecCoord& in);
    //--

    // handle topology changes in the From topology
    virtual void handleTopologyChange();
    // handle topology changes in the To topology
    virtual void handlePointEvents(std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator,
            std::list< const core::componentmodel::topology::TopologyChange *>::const_iterator);
    bool isEmpty() {return this->map.getValue().empty();}
    void setTopology(topology::HexahedronSetTopologyContainer* _topology) {this->topology = _topology; _container=_topology;}

    inline friend std::istream& operator >> ( std::istream& in, BarycentricMapperHexahedronSetTopology<In, Out> &b )
    {
        unsigned int size_vec;

        in >> size_vec;
        sofa::helper::vector<MappingData>& m = *(b.map.beginEdit());
        m.clear();
        MappingData value;
        for (unsigned int i=0; i<size_vec; i++)
        {
            in >> value;
            m.push_back(value);
        }
        b.map.endEdit();
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const BarycentricMapperHexahedronSetTopology<In, Out> & b )
    {

        out << b.map.getValue().size();
        out << " " ;
        out << b.map;

        return out;
    }
};

template <class BasicMapping>
class BarycentricMapping : public BasicMapping
{
public:
    typedef BasicMapping Inherit;
    typedef typename Inherit::In In;
    typedef typename Inherit::Out Out;
    typedef typename In::DataTypes InDataTypes;
    typedef typename InDataTypes::VecCoord InVecCoord;
    typedef typename InDataTypes::VecDeriv InVecDeriv;
    typedef typename InDataTypes::Coord InCoord;
    typedef typename InDataTypes::Deriv InDeriv;
    typedef typename InDataTypes::SparseVecDeriv InSparseVecDeriv;
    typedef typename InDataTypes::Real Real;
    typedef typename Out::DataTypes OutDataTypes;
    typedef typename OutDataTypes::VecCoord OutVecCoord;
    typedef typename OutDataTypes::VecDeriv OutVecDeriv;
    typedef typename OutDataTypes::Coord OutCoord;
    typedef typename OutDataTypes::Deriv OutDeriv;
    typedef typename OutDataTypes::SparseVecDeriv OutSparseVecDeriv;
    typedef typename OutDataTypes::Real OutReal;

    typedef core::componentmodel::topology::BaseMeshTopology BaseMeshTopology;

    using Inherit::sout;
    using Inherit::serr;
    using Inherit::sendl;


protected:

    typedef TopologyBarycentricMapper<InDataTypes,OutDataTypes> Mapper;
    typedef BarycentricMapperRegularGridTopology<InDataTypes, OutDataTypes> RegularGridMapper;
    typedef BarycentricMapperHexahedronSetTopology<InDataTypes, OutDataTypes> HexaMapper;

    Mapper* mapper;
    DataPtr< RegularGridMapper >* f_grid;
    DataPtr< HexaMapper >* f_hexaMapper;

    BarycentricMapperDynamicTopology* dynamicMapper;

public:

#ifdef SOFA_DEV
    //--- partial mapping test
    Data<bool> sleeping;
#endif

    BarycentricMapping(In* from, Out* to)
        : Inherit(from, to), mapper(NULL)
        , f_grid (new DataPtr< RegularGridMapper >( new RegularGridMapper( NULL ),"Regular Grid Mapping"))
        , f_hexaMapper (new DataPtr< HexaMapper >( new HexaMapper(  ),"Hexahedron Mapper"))
#ifdef SOFA_DEV
        , sleeping(core::objectmodel::Base::initData(&sleeping, false, "sleeping", "is the mapping sleeping (not computed)"))
#endif
    {
        this->addField( f_grid, "gridmap");	f_grid->beginEdit();
        this->addField( f_hexaMapper, "hexamap");	f_hexaMapper->beginEdit();
    }

    BarycentricMapping(In* from, Out* to, Mapper* mapper)
        : Inherit(from, to), mapper(mapper)
#ifdef SOFA_DEV
        , sleeping(core::objectmodel::Base::initData(&sleeping, false, "sleeping", "is the mapping sleeping (not computed)"))
#endif
    {
        if (RegularGridMapper* m = dynamic_cast< RegularGridMapper* >(mapper))
        {
            f_grid = new DataPtr< RegularGridMapper >( m,"Regular Grid Mapping");
            this->addField( f_grid, "gridmap");	f_grid->beginEdit();
        }
        else if (HexaMapper* m = dynamic_cast< HexaMapper* >(mapper))
        {
            f_hexaMapper = new DataPtr< HexaMapper >( m,"Hexahedron Mapper");
            this->addField( f_hexaMapper, "hexamap");	f_hexaMapper->beginEdit();
        }
    }

    BarycentricMapping(In* from, Out* to, BaseMeshTopology * topology );

    virtual ~BarycentricMapping()
    {
        if (mapper!=NULL)
            delete mapper;
    }

    void init();

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );

    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );

    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );

    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );

    void draw();

    void getJ(unsigned int Idx, sofa::helper::vector< double > &factor, sofa::helper::vector< unsigned int > &indices) {mapper->getJ(Idx,factor,indices);}

    // handle topology changes depending on the topology
    virtual void handleTopologyChange(core::componentmodel::topology::Topology* t);

    TopologyBarycentricMapper<InDataTypes,OutDataTypes>*	getMapper() {return mapper;}

protected:
    sofa::core::componentmodel::topology::BaseMeshTopology* topology_from;
    sofa::core::componentmodel::topology::BaseMeshTopology* topology_to;

private:
    void createMapperFromTopology(BaseMeshTopology * topology);
};

using sofa::defaulttype::Vec1dTypes;
using sofa::defaulttype::Vec2dTypes;
using sofa::defaulttype::Vec3dTypes;
using sofa::defaulttype::Vec1fTypes;
using sofa::defaulttype::Vec2fTypes;
using sofa::defaulttype::Vec3fTypes;
using sofa::defaulttype::ExtVec1fTypes;
using sofa::defaulttype::ExtVec2fTypes;
using sofa::defaulttype::ExtVec3fTypes;
using core::Mapping;
using core::componentmodel::behavior::MechanicalMapping;
using core::componentmodel::behavior::MappedModel;
using core::componentmodel::behavior::State;
using core::componentmodel::behavior::MechanicalState;

#if defined(WIN32) && !defined(SOFA_COMPONENT_MAPPING_BARYCENTRICMAPPING_CPP)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<Vec3dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3dTypes>, MappedModel<Vec3dTypes> > >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3dTypes>, MappedModel<ExtVec3dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3dTypes>, MappedModel<ExtVec3fTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3dTypes, Vec3dTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3dTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3dTypes, ExtVec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3dTypes, Vec3dTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3dTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3dTypes, ExtVec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3dTypes, Vec3dTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3dTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3dTypes, ExtVec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3dTypes, Vec3dTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3dTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3dTypes, ExtVec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3dTypes, Vec3dTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3dTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3dTypes, ExtVec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3dTypes, Vec3dTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3dTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3dTypes, ExtVec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3dTypes, Vec3dTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3dTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3dTypes, ExtVec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3dTypes, Vec3dTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3dTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3dTypes, ExtVec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3dTypes, Vec3dTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3dTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3dTypes, ExtVec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3dTypes, Vec3dTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3dTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3dTypes, ExtVec3fTypes >;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<Vec3fTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3fTypes>, MappedModel<Vec3fTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3fTypes>, MappedModel<ExtVec3fTypes> > >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3fTypes>, MappedModel<ExtVec3dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3fTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3fTypes, ExtVec3fTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3fTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3fTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3fTypes, ExtVec3fTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3fTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3fTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3fTypes, ExtVec3fTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3fTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3fTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3fTypes, ExtVec3fTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3fTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3fTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3fTypes, ExtVec3fTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3fTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3fTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3fTypes, ExtVec3fTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3fTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3fTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3fTypes, ExtVec3fTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3fTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3fTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3fTypes, ExtVec3fTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3fTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3fTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3fTypes, ExtVec3fTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3fTypes, ExtVec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3fTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3fTypes, ExtVec3fTypes >;
// extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3fTypes, ExtVec3dTypes >;
#endif
#ifndef SOFA_FLOAT
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< MechanicalMapping< MechanicalState<Vec3dTypes>, MechanicalState<Vec3fTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< MechanicalMapping< MechanicalState<Vec3fTypes>, MechanicalState<Vec3dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3dTypes>, MappedModel<Vec3fTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapping< Mapping< State<Vec3fTypes>, MappedModel<Vec3dTypes> > >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3dTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapper<Vec3fTypes, Vec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3dTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API TopologyBarycentricMapper<Vec3fTypes, Vec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3dTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperRegularGridTopology<Vec3fTypes, Vec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3dTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperSparseGridTopology<Vec3fTypes, Vec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3dTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperMeshTopology<Vec3fTypes, Vec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3dTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperEdgeSetTopology<Vec3fTypes, Vec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3dTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTriangleSetTopology<Vec3fTypes, Vec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3dTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperQuadSetTopology<Vec3fTypes, Vec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3dTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperTetrahedronSetTopology<Vec3fTypes, Vec3dTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3dTypes, Vec3fTypes >;
extern template class SOFA_COMPONENT_MAPPING_API BarycentricMapperHexahedronSetTopology<Vec3fTypes, Vec3dTypes >;
#endif
#endif
#endif

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
