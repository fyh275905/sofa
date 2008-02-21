/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_COMPONENT_MAPPING_BARYCENTRICMAPPING_H
#define SOFA_COMPONENT_MAPPING_BARYCENTRICMAPPING_H

#include <sofa/core/componentmodel/behavior/MechanicalMapping.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/component/topology/MeshTopology.h>
#include <sofa/component/topology/RegularGridTopology.h>
#include <sofa/component/topology/SparseGridTopology.h>
#include <sofa/component/topology/TriangleSetTopology.h>
#include <vector>


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
    virtual void init() = 0;
    virtual void apply( typename Out::VecCoord& out, const typename In::VecCoord& in ) = 0;
    virtual void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in ) = 0;
    virtual void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in ) = 0;
    virtual void applyJT( typename In::VecConst& out, const typename Out::VecConst& in ) = 0;
    virtual void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in) = 0;

    //Nothing to do
    inline friend std::istream& operator >> ( std::istream& in, BarycentricMapper< In, Out > & ) {return in;}
    inline friend std::ostream& operator << ( std::ostream& out, const BarycentricMapper< In, Out > &  ) { return out; }
};

/// Template class for barycentric mapping topology-specific mappers
template<class Topology, class In, class Out>
class TopologyBarycentricMapper;

/// Class allowing barycentric mapping computation on a RegularGridTopology
template<class In, class Out>
class TopologyBarycentricMapper<topology::RegularGridTopology, In, Out> : public BarycentricMapper<In,Out>
{
public:
    typedef BarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::CubeData CubeData;
protected:
    sofa::helper::vector<CubeData> map;
    topology::RegularGridTopology* topology;
public:
    TopologyBarycentricMapper(topology::RegularGridTopology* topology) : topology(topology)
    {}

    bool empty() const {return map.size()==0;}

    void setTopology( topology::RegularGridTopology* t ) { topology = t; }

    void clear(int reserve=0);

    int addPointInCube(int cubeIndex, const Real* baryCoords);

    void init();

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    inline friend std::istream& operator >> ( std::istream& in, TopologyBarycentricMapper<topology::RegularGridTopology, In, Out> &b )
    {
        in >> b.map;
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const TopologyBarycentricMapper<topology::RegularGridTopology, In, Out> & b )
    {
        out << b.map;
        return out;
    }

};

/// Class allowing barycentric mapping computation on a SparseGridTopology
template<class In, class Out>
class TopologyBarycentricMapper<topology::SparseGridTopology, In, Out> : public BarycentricMapper<In,Out>
{
public:
    typedef BarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::CubeData CubeData;
protected:
    sofa::helper::vector<CubeData> map;
    topology::SparseGridTopology* topology;
public:
    TopologyBarycentricMapper(topology::SparseGridTopology* topology) : topology(topology)
    {}

    bool empty() const {return map.size()==0;}

    void setTopology( topology::SparseGridTopology* t ) { topology = t; }

    void clear(int reserve=0);

    int addPointInCube(int cubeIndex, const Real* baryCoords);

    void init();

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    inline friend std::istream& operator >> ( std::istream& in, TopologyBarycentricMapper<topology::SparseGridTopology, In, Out> &b )
    {
        in >> b.map;
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const TopologyBarycentricMapper<topology::SparseGridTopology, In, Out> & b )
    {
        out << b.map;
        return out;
    }

};


/// Class allowing barycentric mapping computation on a MeshTopology
template<class In, class Out>
class TopologyBarycentricMapper<topology::MeshTopology, In, Out> : public BarycentricMapper<In,Out>
{
public:
    typedef BarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::MappingData1D MappingData1D;
    typedef typename Inherit::MappingData2D MappingData2D;
    typedef typename Inherit::MappingData3D MappingData3D;
protected:
    sofa::helper::vector< MappingData1D >  map1d;
    sofa::helper::vector< MappingData2D >  map2d;
    sofa::helper::vector< MappingData3D >  map3d;
    topology::MeshTopology* topology;

public:
    TopologyBarycentricMapper(topology::MeshTopology* topology) : topology(topology)
    {}

    bool empty() const {return map1d.size()==0 && map2d.size()==0 && map3d.size()==0;}
    void setTopology( topology::MeshTopology* t ) { topology = t; }
    void clear(int reserve3d=0, int reserve2d=0, int reserve1d=0);

    int addPointInLine(int lineIndex, const Real* baryCoords);
    int createPointInLine(const typename Out::Coord& p, int lineIndex, const typename In::VecCoord* points);

    int addPointInTriangle(int triangleIndex, const Real* baryCoords);
    int createPointInTriangle(const typename Out::Coord& p, int triangleIndex, const typename In::VecCoord* points);

    int addPointInQuad(int quadIndex, const Real* baryCoords);
    int createPointInQuad(const typename Out::Coord& p, int quadIndex, const typename In::VecCoord* points);

    int addPointInTetra(int tetraIndex, const Real* baryCoords);

    int addPointInCube(int cubeIndex, const Real* baryCoords);

    void init();

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    inline friend std::istream& operator >> ( std::istream& in, TopologyBarycentricMapper<topology::MeshTopology, In, Out> &b )
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

    inline friend std::ostream& operator << ( std::ostream& out, const TopologyBarycentricMapper<topology::MeshTopology, In, Out> & b )
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


};


/// Class allowing barycentric mapping computation on a EdgeSetTopology
template<class In, class Out>
class TopologyBarycentricMapper<topology::EdgeSetTopology<In>, In, Out> : public BarycentricMapper<In,Out>
    //class TriangleSetTopologyBarycentricMapper : public BarycentricMapper<In,Out>
{
public:
    typedef BarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    typedef typename Inherit::MappingData1D MappingData;
protected:
    sofa::helper::vector< MappingData >  map;
    topology::EdgeSetTopology<In>* topology;

public:
    TopologyBarycentricMapper(topology::EdgeSetTopology<In>* topology) : topology(topology)
    {}

    bool empty() const {return map.size()==0;}
    void setTopology( topology::EdgeSetTopology<In>* t ) { topology = t; }
    void clear(int reserve=0);

    int addPointInEdge(int edgeIndex, const Real* baryCoords);
    int createPointInEdge(const typename Out::Coord& p, int edgeIndex, const typename In::VecCoord* points);

    void init();

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    inline friend std::istream& operator >> ( std::istream& in, TopologyBarycentricMapper<topology::EdgeSetTopology<In>, In, Out> &b )
    {
        unsigned int size_vec;

        in >> size_vec;
        b.map.clear();
        MappingData value;
        for (unsigned int i=0; i<size_vec; i++)
        {
            in >> value;
            b.map.push_back(value);
        }
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const TopologyBarycentricMapper<topology::EdgeSetTopology<In>, In, Out> & b )
    {

        out << b.map.size();
        out << " " ;
        out << b.map;

        return out;
    }


};


/// Class allowing barycentric mapping computation on a TriangleSetTopology
template<class In, class Out>
class TopologyBarycentricMapper<topology::TriangleSetTopology<In>, In, Out> : public BarycentricMapper<In,Out>
    //class TriangleSetTopologyBarycentricMapper : public BarycentricMapper<In,Out>
{
public:
    typedef BarycentricMapper<In,Out> Inherit;
    typedef typename Inherit::Real Real;
    typedef typename Inherit::OutReal OutReal;
    //typedef typename Inherit::MappingData1D MappingData;
    typedef typename Inherit::MappingData2D MappingData;
    //typedef typename Inherit::MappingData3D MappingData;
protected:
    //sofa::helper::vector< MappingData1D >  map;
    sofa::helper::vector< MappingData >  map;
    //sofa::helper::vector< MappingData3D >  map;
    topology::TriangleSetTopology<In>* topology;

public:
    TopologyBarycentricMapper(topology::TriangleSetTopology<In>* topology) : topology(topology)
    {}

    bool empty() const {return map.size()==0;}
    void setTopology( topology::TriangleSetTopology<In>* t ) { topology = t; }
    void clear(int reserve=0);

    int addPointInTriangle(int triangleIndex, const Real* baryCoords);
    int createPointInTriangle(const typename Out::Coord& p, int triangleIndex, const typename In::VecCoord* points);

    void init();

    void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
    void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
    void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
    void applyJT( typename In::VecConst& out, const typename Out::VecConst& in );
    void draw( const typename Out::VecCoord& out, const typename In::VecCoord& in);

    inline friend std::istream& operator >> ( std::istream& in, TopologyBarycentricMapper<topology::TriangleSetTopology<In>, In, Out> &b )
    {
        unsigned int size_vec;

        in >> size_vec;
        b.map.clear();
        MappingData value;
        for (unsigned int i=0; i<size_vec; i++)
        {
            in >> value;
            b.map.push_back(value);
        }
        return in;
    }

    inline friend std::ostream& operator << ( std::ostream& out, const TopologyBarycentricMapper<topology::TriangleSetTopology<In>, In, Out> & b )
    {

        out << b.map.size();
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
    typedef typename InDataTypes::SparseDeriv InSparseDeriv;
    typedef typename InDataTypes::Real Real;
    typedef typename Out::DataTypes OutDataTypes;
    typedef typename OutDataTypes::VecCoord OutVecCoord;
    typedef typename OutDataTypes::VecDeriv OutVecDeriv;
    typedef typename OutDataTypes::Coord OutCoord;
    typedef typename OutDataTypes::Deriv OutDeriv;
    typedef typename OutDataTypes::SparseVecDeriv OutSparseVecDeriv;
    typedef typename OutDataTypes::SparseDeriv OutSparseDeriv;
    typedef typename OutDataTypes::Real OutReal;

protected:

    typedef BarycentricMapper<InDataTypes,OutDataTypes> Mapper;
    typedef TopologyBarycentricMapper<topology::MeshTopology, InDataTypes, OutDataTypes> MeshMapper;
    typedef TopologyBarycentricMapper<topology::TriangleSetTopology<InDataTypes>, InDataTypes, OutDataTypes> TriangleSetMapper;
    //typedef TriangleSetTopologyBarycentricMapper<InDataTypes, OutDataTypes> TriangleSetMapper;
    typedef TopologyBarycentricMapper<topology::RegularGridTopology, InDataTypes, OutDataTypes> RegularGridMapper;
    typedef TopologyBarycentricMapper<topology::SparseGridTopology, InDataTypes, OutDataTypes> SparseGridMapper;

    Mapper* mapper;
    DataPtr< RegularGridMapper >* f_grid;
    DataPtr< SparseGridMapper >* f_sparsegrid;
    DataPtr< MeshMapper >*        f_mesh;
    DataPtr< TriangleSetMapper >*        f_triangle;
    void calcMap(topology::RegularGridTopology* topo);
    void calcMap(topology::SparseGridTopology* topo);
    void calcMap(topology::MeshTopology* topo);
    void calcMap(topology::TriangleSetTopology<InDataTypes>* topo);

public:
    BarycentricMapping(In* from, Out* to)
        : Inherit(from, to), mapper(NULL)
        , f_grid (new DataPtr< RegularGridMapper >( new RegularGridMapper( NULL ),"Regular Grid Mapping"))
        , f_sparsegrid (new DataPtr< SparseGridMapper >( new SparseGridMapper( NULL ),"Sparse Grid Mapping"))
        , f_mesh (new DataPtr< MeshMapper >       ( new MeshMapper( NULL ),"Mesh Mapping"))
        , f_triangle (new DataPtr< TriangleSetMapper >       ( new TriangleSetMapper( NULL ),"TriangleSet Mapping"))
    {
        this->addField( f_grid, "gridmap");	f_grid->beginEdit();
        this->addField( f_sparsegrid, "sparsegridmap");	f_sparsegrid->beginEdit();
        this->addField( f_mesh, "meshmap");	f_mesh->beginEdit();
        this->addField( f_triangle, "trianglemap");	f_triangle->beginEdit();
    }

    BarycentricMapping(In* from, Out* to, Mapper* mapper)
        : Inherit(from, to), mapper(mapper)
    {
        //Regular Grid Case
        if (RegularGridMapper* m = dynamic_cast< RegularGridMapper* >(mapper))
            f_grid = new DataPtr< RegularGridMapper >( m,"Regular Grid Mapping");
        else
            f_grid = new DataPtr< RegularGridMapper >( new RegularGridMapper( NULL ),"Regular Grid Mapping");

        this->addField( f_grid, "gridmap");	f_grid->beginEdit();

        //Sparse Grid Case
        if (SparseGridMapper* m = dynamic_cast< SparseGridMapper* >(mapper))
            f_sparsegrid = new DataPtr< SparseGridMapper >( m,"Sparse Grid Mapping");
        else
            f_sparsegrid = new DataPtr< SparseGridMapper >( new SparseGridMapper( NULL ),"Sparse Grid Mapping");

        this->addField( f_sparsegrid, "sparsegridmap");	f_sparsegrid->beginEdit();

        //Mesh Case
        if (MeshMapper* m = dynamic_cast< MeshMapper* >(mapper))
            f_mesh = new DataPtr< MeshMapper >( m,"Mesh Mapping");
        else
            f_mesh = new DataPtr< MeshMapper >( new MeshMapper( NULL ),"Mesh Mapping");

        this->addField( f_mesh, "meshmap");	f_mesh->beginEdit();

        //TriangleSet Case
        if (TriangleSetMapper* m = dynamic_cast< TriangleSetMapper* >(mapper))
            f_triangle = new DataPtr< TriangleSetMapper >( m,"TriangleSet Mapping");
        else
            f_triangle = new DataPtr< TriangleSetMapper >( new TriangleSetMapper( NULL ),"TriangleSet Mapping");

        this->addField( f_triangle, "trianglemap");	f_triangle->beginEdit();

    }

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

};

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
