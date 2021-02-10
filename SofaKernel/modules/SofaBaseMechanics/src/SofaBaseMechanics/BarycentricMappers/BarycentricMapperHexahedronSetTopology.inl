/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <SofaBaseMechanics/BarycentricMappers/BarycentricMapperHexahedronSetTopology.h>

namespace sofa::component::mapping
{

template <class In, class Out>
BarycentricMapperHexahedronSetTopology<In,Out>::BarycentricMapperHexahedronSetTopology()
    : Inherit1(nullptr, nullptr),
      m_fromContainer(nullptr),
      m_fromGeomAlgo(nullptr)
{}

template <class In, class Out>
BarycentricMapperHexahedronSetTopology<In,Out>::BarycentricMapperHexahedronSetTopology(topology::HexahedronSetTopologyContainer* fromTopology,
                                                                                       topology::PointSetTopologyContainer* toTopology)
    : Inherit1(fromTopology, toTopology),
      m_fromContainer(fromTopology),
      m_fromGeomAlgo(nullptr)
{}

template <class In, class Out>
BarycentricMapperHexahedronSetTopology<In,Out>::~BarycentricMapperHexahedronSetTopology()
{}

template <class In, class Out>
void BarycentricMapperHexahedronSetTopology<In,Out>::setTopology(topology::HexahedronSetTopologyContainer* topology)
{
    m_fromTopology  = topology;
    m_fromContainer = topology;
}


template <class In, class Out>
typename BarycentricMapperHexahedronSetTopology<In, Out>::Index 
BarycentricMapperHexahedronSetTopology<In,Out>::addPointInCube ( const Index cubeIndex, const SReal* baryCoords )
{
    helper::vector<MappingData>& vectorData = *(d_map.beginEdit());
    vectorData.resize ( d_map.getValue().size() +1 );
    MappingData& data = *vectorData.rbegin();
    d_map.endEdit();
    data.in_index = cubeIndex;
    data.baryCoords[0] = ( Real ) baryCoords[0];
    data.baryCoords[1] = ( Real ) baryCoords[1];
    data.baryCoords[2] = ( Real ) baryCoords[2];
    return (int)d_map.getValue().size()-1;
}


template <class In, class Out>
typename BarycentricMapperHexahedronSetTopology<In, Out>::Index 
BarycentricMapperHexahedronSetTopology<In,Out>::setPointInCube ( const Index pointIndex, const Index cubeIndex, const SReal* baryCoords )
{
    if ( pointIndex >= d_map.getValue().size() )
        return sofa::InvalidID;

    helper::vector<MappingData>& vectorData = *(d_map.beginEdit());
    MappingData& data = vectorData[pointIndex];
    data.in_index = cubeIndex;
    data.baryCoords[0] = ( Real ) baryCoords[0];
    data.baryCoords[1] = ( Real ) baryCoords[1];
    data.baryCoords[2] = ( Real ) baryCoords[2];
    d_map.endEdit();

    if(cubeIndex == sofa::InvalidID)
        m_invalidIndex.insert(pointIndex);
    else
        m_invalidIndex.erase(pointIndex);

    return pointIndex;
}


template <class In, class Out>
helper::vector<Hexahedron> BarycentricMapperHexahedronSetTopology<In,Out>::getElements()
{
    return this->m_fromTopology->getHexahedra();
}


template <class In, class Out>
helper::vector<SReal> BarycentricMapperHexahedronSetTopology<In,Out>::getBaryCoef(const Real* f)
{
    return getBaryCoef(f[0],f[1],f[2]);
}


template <class In, class Out>
helper::vector<SReal> BarycentricMapperHexahedronSetTopology<In,Out>::getBaryCoef(const Real fx, const Real fy, const Real fz)
{
    helper::vector<SReal> hexahedronCoef{(1-fx)*(1-fy)*(1-fz),
                (fx)*(1-fy)*(1-fz),
                (fx)*(fy)*(1 - fz),
                (1 - fx)*(fy)*(1 - fz),
                (1-fx)*(1-fy)*(fz),
                (fx)*(1-fy)*(fz),
                (fx)*(fy)*(fz),
                (1 - fx)*(fy)*(fz)
    };
    return hexahedronCoef;
}


template <class In, class Out>
void BarycentricMapperHexahedronSetTopology<In,Out>::computeBase(Mat3x3d& base, const typename In::VecCoord& in, const Hexahedron& element)
{
    Mat3x3d matrixTranspose;
    base[0] = in[element[1]]-in[element[0]];
    base[1] = in[element[3]]-in[element[0]];
    base[2] = in[element[4]]-in[element[0]];
    matrixTranspose.transpose(base);
    base.invert(matrixTranspose);
}


template <class In, class Out>
void BarycentricMapperHexahedronSetTopology<In,Out>::computeCenter(Vector3& center, const typename In::VecCoord& in, const Hexahedron &element)
{
    center = ( in[element[0]]+in[element[1]]+in[element[2]]+in[element[3]]+in[element[4]]+in[element[5]]+in[element[6]]+in[element[7]] ) *0.125;
}


template <class In, class Out>
void BarycentricMapperHexahedronSetTopology<In,Out>::computeDistance(double& d, const Vector3& v)
{
    d = std::max ( std::max ( -v[0],-v[1] ),std::max ( std::max ( -v[2],v[0]-1 ),std::max ( v[1]-1,v[2]-1 ) ) );
}


template <class In, class Out>
void BarycentricMapperHexahedronSetTopology<In,Out>::addPointInElement(const Index elementIndex, const SReal* baryCoords)
{
    addPointInCube(elementIndex,baryCoords);
}



template <class In, class Out>
void BarycentricMapperHexahedronSetTopology<In,Out>::handleTopologyChange(core::topology::Topology* t)
{
    using sofa::core::behavior::MechanicalState;

    if (t != this->m_fromTopology) return;

    if ( this->m_fromTopology->beginChange() == this->m_fromTopology->endChange() )
        return;

    std::list<const core::topology::TopologyChange *>::const_iterator itBegin = this->m_fromTopology->beginChange();
    std::list<const core::topology::TopologyChange *>::const_iterator itEnd = this->m_fromTopology->endChange();

    for ( std::list<const core::topology::TopologyChange *>::const_iterator changeIt = itBegin;
            changeIt != itEnd; ++changeIt )
    {
        const core::topology::TopologyChangeType changeType = ( *changeIt )->getChangeType();
        switch ( changeType )
        {
            //TODO: implementation of BarycentricMapperHexahedronSetTopology<In,Out>::handleTopologyChange()
        case core::topology::ENDING_EVENT:       ///< To notify the end for the current sequence of topological change events
        {
            if(!m_invalidIndex.empty())
            {
                helper::vector<MappingData>& mapData = *(d_map.beginEdit());

                for ( auto iter = m_invalidIndex.cbegin();
                        iter != m_invalidIndex.cend(); ++iter )
                {
                    const auto j = *iter;
                    if ( mapData[j].in_index == sofa::InvalidID ) // compute new mapping
                    {
                        Vector3 coefs;
                        typename In::Coord pos;
                        pos[0] = mapData[j].baryCoords[0];
                        pos[1] = mapData[j].baryCoords[1];
                        pos[2] = mapData[j].baryCoords[2];

                        // find nearest cell and barycentric coords
                        Real distance = 1e10;

                        Index index = sofa::InvalidID;
                        // When smoothing a mesh, the element has to be found using the rest position of the point. Then, its position is set using this element.
                        if( this->m_toTopology)
                        {
                            typedef MechanicalState<Out> MechanicalStateT;
                            MechanicalStateT* mState;
                            this->m_toTopology->getContext()->get( mState);
                            if( !mState)
                            {
                                msg_error() << "Can not find mechanical state." ;
                            }
                            else
                            {
                                const typename MechanicalStateT::VecCoord& xto0 = (mState->read(core::ConstVecCoordId::restPosition())->getValue());
                                index = m_fromGeomAlgo->findNearestElementInRestPos ( Out::getCPos(xto0[j]), coefs, distance );
                                coefs = m_fromGeomAlgo->computeHexahedronRestBarycentricCoeficients(index, pos);
                            }
                        }
                        else
                        {
                            index = m_fromGeomAlgo->findNearestElementInRestPos ( pos, coefs, distance );
                        }

                        if ( index != sofa::InvalidID )
                        {
                            mapData[j].baryCoords[0] = ( Real ) coefs[0];
                            mapData[j].baryCoords[1] = ( Real ) coefs[1];
                            mapData[j].baryCoords[2] = ( Real ) coefs[2];
                            mapData[j].in_index = index;
                        }
                    }
                }

                d_map.endEdit();
                m_invalidIndex.clear();
            }
        }
        break;
        case core::topology::POINTSINDICESSWAP:  ///< For PointsIndicesSwap.
            break;
        case core::topology::POINTSADDED:        ///< For PointsAdded.
            break;
        case core::topology::POINTSREMOVED:      ///< For PointsRemoved.
            break;
        case core::topology::POINTSRENUMBERING:  ///< For PointsRenumbering.
            break;
        case core::topology::TRIANGLESADDED:  ///< For Triangles Added.
            break;
        case core::topology::TRIANGLESREMOVED:  ///< For Triangles Removed.
            break;
        case core::topology::HEXAHEDRAADDED:     ///< For HexahedraAdded.
        {
        }
        break;
        case core::topology::HEXAHEDRAREMOVED:   ///< For HexahedraRemoved.
        {
            const auto nbHexahedra = this->m_fromTopology->getNbHexahedra();

            const auto &hexahedra =
                    ( static_cast< const core::topology::HexahedraRemoved *> ( *changeIt ) )->getArray();
            //        helper::vector<unsigned int> hexahedra(tab);

            for ( std::size_t i=0; i<hexahedra.size(); ++i )
            {
                // remove all references to the removed cubes from the mapping data
                Index cubeId = hexahedra[i];
                for ( std::size_t j=0; j<d_map.getValue().size(); ++j )
                {
                    if ( d_map.getValue()[j].in_index == cubeId ) // invalidate mapping
                    {
                        Vector3 coefs;
                        coefs[0] = d_map.getValue()[j].baryCoords[0];
                        coefs[1] = d_map.getValue()[j].baryCoords[1];
                        coefs[2] = d_map.getValue()[j].baryCoords[2];

                        typename In::Coord restPos = m_fromGeomAlgo->getRestPointPositionInHexahedron ( cubeId, coefs );

                        helper::vector<MappingData>& vectorData = *(d_map.beginEdit());
                        vectorData[j].in_index = sofa::InvalidID;
                        vectorData[j].baryCoords[0] = restPos[0];
                        vectorData[j].baryCoords[1] = restPos[1];
                        vectorData[j].baryCoords[2] = restPos[2];
                        d_map.endEdit();

                        m_invalidIndex.insert(Size(j));
                    }
                }
            }

            // renumber
            Index lastCubeId = nbHexahedra-1;
            for ( std::size_t i=0; i<hexahedra.size(); ++i, --lastCubeId )
            {
                Index cubeId = hexahedra[i];
                for (Index j=0; j<d_map.getValue().size(); ++j )
                {
                    if ( d_map.getValue()[j].in_index == lastCubeId )
                    {
                        helper::vector<MappingData>& vectorData = *(d_map.beginEdit());
                        vectorData[j].in_index = cubeId;
                        d_map.endEdit();
                    }
                }
            }
        }
        break;
        case core::topology::HEXAHEDRARENUMBERING: ///< For HexahedraRenumbering.
            break;
        default:
            break;
        }
    }
}

template <class In, class Out>
void BarycentricMapperHexahedronSetTopology<In,Out>::applyOnePoint( const Index& hexaPointId,typename Out::VecCoord& out, const typename In::VecCoord& in )
{
    const helper::vector<Hexahedron>& cubes = this->m_fromTopology->getHexahedra();
    const Real fx = d_map.getValue()[hexaPointId].baryCoords[0];
    const Real fy = d_map.getValue()[hexaPointId].baryCoords[1];
    const Real fz = d_map.getValue()[hexaPointId].baryCoords[2];
    Index index = d_map.getValue()[hexaPointId].in_index;
    const Hexahedron& cube = cubes[index];
    Out::setCPos(out[hexaPointId] , in[cube[0]] * ( ( 1-fx ) * ( 1-fy ) * ( 1-fz ) )
            + in[cube[1]] * ( ( fx ) * ( 1-fy ) * ( 1-fz ) )
            + in[cube[3]] * ( ( 1-fx ) * ( fy ) * ( 1-fz ) )
            + in[cube[2]] * ( ( fx ) * ( fy ) * ( 1-fz ) )
            + in[cube[4]] * ( ( 1-fx ) * ( 1-fy ) * ( fz ) )
            + in[cube[5]] * ( ( fx ) * ( 1-fy ) * ( fz ) )
            + in[cube[7]] * ( ( 1-fx ) * ( fy ) * ( fz ) )
            + in[cube[6]] * ( ( fx ) * ( fy ) * ( fz ) ) );
}


} // namespace sofa::component::mapping
