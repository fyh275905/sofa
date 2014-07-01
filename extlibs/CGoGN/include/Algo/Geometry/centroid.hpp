/*******************************************************************************
 * CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
 * version 0.1                                                                  *
 * Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
 * Web site: http://cgogn.unistra.fr/                                           *
 * Contact information: cgogn@unistra.fr                                        *
 *                                                                              *
 *******************************************************************************/

#include "Topology/generic/cellmarker.h"
#include "Topology/generic/traversor/traversorCell.h"
#include "Topology/generic/traversor/traversor2.h"
#include "Topology/generic/traversor/traversorCell.h"
#include "Topology/generic/traversor/traversor3.h"

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Geometry
{

template <typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE volumeCentroid(typename PFP::MAP& map, Vol d, const V_ATT& attributs, unsigned int thread)
{
    typename V_ATT::DATA_TYPE center(0.0);
    unsigned int count = 0 ;

    //	foreach_incident3<VERTEX>(map,d, [&] (Vertex v)
    //	{
    //		center += attributs[v];
    //		++count;
    //	}
    //	,false,thread);

    foreach_incident3<VERTEX>(map,d, (bl::var(center) += attributs[bl::_1], ++bl::var(count))
                              ,false,thread);

    center /= double(count) ;
    return center ;
}

template <typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE volumeCentroidELW(typename PFP::MAP& map, Vol d, const V_ATT& attributs, unsigned int thread)
{
    typedef typename V_ATT::DATA_TYPE EMB;
    typedef typename PFP::MAP Map;
    EMB center(0.0);

    double count=0.0;
    using bl::var;
    //    foreach_incident3<EDGE>(map,d, [&] (Edge it)
    //    {
    //        EMB e1 = attributs[it.dart];
    //        EMB e2 = attributs[map.phi1(it)];
    //        double l = (e2-e1).norm();
    //        center += (e1+e2)*l;
    //        count += 2.0*l ;
    //    },false,thread);
    EMB e1, e2;
    double l;
    foreach_incident3<EDGE>(map,d,
                            (var(e1) = attributs[bl::_1],
                             var(e2) = attributs[bl::bind(&Map::phi1, boost::ref(map), bl::_1)],
                             var(l) = bl::bind(&EMB::norm, var(e2) - var(e1)),
                             var(center) += (var(e1) +var(e2))*var(l),
                             var(count) += 2.0*var(l)), false, thread);

    center /= double(count);
    return center ;
}

template <typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE faceCentroid(typename PFP::MAP& map, Face f, const V_ATT& attributs)
{
    typename V_ATT::DATA_TYPE center(0.0);
    unsigned int count = 0 ;
    using bl::var;
    //    foreach_incident2<PFP::MAP, VERTEX>(map, f, [&](Vertex it)
    //    {
    //        center += attributs[it];
    //        ++count ;
    //    });
    foreach_incident2<PFP::MAP, VERTEX>(map, f, (var(center) += attributs[bl::_1],
                                                 var(count)++));

    center /= double(count);
    return center ;
}

template <typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE faceCentroidELW(typename PFP::MAP& map, Face f, const V_ATT& attributs)
{
    typedef typename V_ATT::DATA_TYPE EMB;
    typedef typename PFP::MAP Map;
    using bl::var;
    EMB center(0.0);
    double count=0.0;

    //    foreach_incident2<EDGE>(map, f, [&](Edge it)
    //    {
    //        EMB e1 = attributs[it.dart];
    //        EMB e2 = attributs[map.phi1(it)];
    //        double l = (e2-e1).norm();
    //        center += (e1+e2)*l;
    //        count += 2.0*l ;
    //    });
    EMB e1, e2;
    double l;
    foreach_incident2<EDGE>(map,f,
                            (var(e1) = attributs[bl::_1],
                             var(e2) = attributs[bl::bind(&Map::phi1, boost::ref(map), bl::_1)],
                             var(l) = bl::bind(&EMB::norm, var(e2) - var(e1)),
                             var(center) += (var(e1) +var(e2))*var(l),
                             var(count) += 2.0*var(l)));
    center /= double(count);
    return center ;
}

template <typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE vertexNeighborhoodCentroid(typename PFP::MAP& map, Vertex v, const V_ATT& attributs)
{
    typename V_ATT::DATA_TYPE center(0.0);
    using bl::var;
    unsigned int count = 0 ;
    //	Traversor2VVaE<typename PFP::MAP> t(map, d) ;
    //    foreach_adjacent2<EDGE>(map, v, [&](Vertex it)
    //    {
    //        center += attributs[it];
    //        ++count ;
    //    });
    foreach_adjacent2<EDGE>(map, v, (var(center) += attributs[bl::_1],
                                     var(count)++));
    center /= count ;
    return center ;
}

template <typename PFP, typename V_ATT, typename F_ATT>
void computeCentroidFaces(typename PFP::MAP& map, const V_ATT& position, F_ATT& face_centroid, unsigned int thread)
{
    using bl::var;
    if ((CGoGN::Parallel::NumberOfThreads > 1) && (thread==0))
    {
        Parallel::computeCentroidFaces<PFP,V_ATT,F_ATT>(map,position,face_centroid);
        return;
    }

    //    foreach_cell<FACE>(map, [&] (Face f)
    //    {
    //        face_centroid[f] = faceCentroid<PFP,V_ATT>(map, f, position) ;
    //    }
    //    ,AUTO,thread);
    foreach_cell<FACE>(map,
                       (
                           face_centroid[bl::_1] = bl::bind(&faceCentroid<PFP,V_ATT>, boost::ref(map), bl::_1, boost::ref(position))
            )
            ,AUTO,thread);
}

template <typename PFP, typename V_ATT, typename F_ATT>
void computeCentroidELWFaces(typename PFP::MAP& map, const V_ATT& position, F_ATT& face_centroid, unsigned int thread)
{
    if ((CGoGN::Parallel::NumberOfThreads > 1) && (thread==0))
    {
        Parallel::computeCentroidELWFaces<PFP,V_ATT,F_ATT>(map,position,face_centroid);
        return;
    }

    //    foreach_cell<FACE>(map, [&] (Face f)
    //    {
    //        face_centroid[f] = faceCentroidELW<PFP,V_ATT>(map, f, position) ;
    //    }
    //    ,AUTO,thread);
    foreach_cell<FACE>(map,
                       (
                           face_centroid[bl::_1] = bl::bind(&faceCentroidELW<PFP,V_ATT>, boost::ref(map), bl::_1, boost::ref(position))
            )
            ,AUTO,thread);
}

template <typename PFP, typename V_ATT>
void computeNeighborhoodCentroidVertices(typename PFP::MAP& map, const V_ATT& position, V_ATT& vertex_centroid, unsigned int thread)
{
    if ((CGoGN::Parallel::NumberOfThreads > 1) && (thread == 0))
    {
        Parallel::computeNeighborhoodCentroidVertices<PFP,V_ATT>(map,position,vertex_centroid);
        return;
    }

    //    foreach_cell<VERTEX>(map, [&] (Vertex v)
    //    {
    //        vertex_centroid[v] = vertexNeighborhoodCentroid<PFP,V_ATT>(map, v, position) ;
    //    }, AUTO, thread);
    foreach_cell<VERTEX>(map,
                         (
                             vertex_centroid[bl::_1] = bl::bind(&vertexNeighborhoodCentroid<PFP,V_ATT>, boost::ref(map), bl::_1, boost::ref(position))
            )
            ,AUTO,thread);
}


namespace Parallel
{

template <typename PFP, typename V_ATT, typename F_ATT>
void computeCentroidFaces(typename PFP::MAP& map, const V_ATT& position, F_ATT& face_centroid)
{
    //    CGoGN::Parallel::foreach_cell<FACE>(map,[&](Face f, unsigned int /*thr*/)
    //    {
    //        face_centroid[f] = faceCentroid<PFP>(map, f, position) ;
    //    });
    CGoGN::Parallel::foreach_cell<FACE>(map,
                                        (
                                            face_centroid[bl::_1 = bl::bind(&faceCentroid<PFP>, boost::ref(map), bl::_1, boost::ref(position))]
            ));

}

template <typename PFP, typename V_ATT, typename F_ATT>
void computeCentroidELWFaces(typename PFP::MAP& map, const V_ATT& position, F_ATT& face_centroid)
{
    //    CGoGN::Parallel::foreach_cell<FACE>(map,[&](Face f, unsigned int /*thr*/)
    //    {
    //        face_centroid[f] = faceCentroidELW<PFP>(map, f, position) ;
    //    });
    CGoGN::Parallel::foreach_cell<FACE>(map,
                                        (
                                            face_centroid[bl::_1 = bl::bind(&faceCentroidELW<PFP>, boost::ref(map), bl::_1, boost::ref(position))]
            ));
}

template <typename PFP, typename V_ATT>
void computeNeighborhoodCentroidVertices(typename PFP::MAP& map,
                                         const V_ATT& position, V_ATT& vertex_centroid)
{
    //    CGoGN::Parallel::foreach_cell<VERTEX>(map,[&](Vertex v, unsigned int /*thr*/)
    //    {
    //        vertex_centroid[v] = vertexNeighborhoodCentroid<PFP>(map, v, position) ;
    //    }, FORCE_CELL_MARKING);
    CGoGN::Parallel::foreach_cell<VERTEX>(map,
                                          (
                                              vertex_centroid[bl::_1] = bl::bind(&vertexNeighborhoodCentroid<PFP>, boost::ref(map), bl::_1, boost::ref(position))
            ), FORCE_CELL_MARKING);
}

} // namespace Parallel



} // namespace Geometry

} // namespace Surface

namespace Volume
{

namespace Geometry
{

template <typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE vertexNeighborhoodCentroid(typename PFP::MAP& map, Vertex v, const V_ATT& attributs, unsigned int thread)
{
    typename V_ATT::DATA_TYPE  center(0.0);
    using bl::var;
    unsigned int count = 0 ;
    //    foreach_adjacent3<EDGE>(map, v, [&] (Vertex it)
    //    {
    //        center += attributs[it];
    //        ++count ;
    //    }, false, thread);
    foreach_adjacent3<EDGE>(map, v,
                            (
                                var(center) += attributs[bl::_1],
                                var(count)++
                                ), false, thread);
    center /= count ;
    return center ;
}

template <typename PFP, typename V_ATT, typename W_ATT>
void computeCentroidVolumes(typename PFP::MAP& map, const V_ATT& position, W_ATT& vol_centroid, unsigned int thread)
{
    using bl::var;
    if ((CGoGN::Parallel::NumberOfThreads > 1) && (thread == 0))
    {
        Parallel::computeCentroidVolumes<PFP,V_ATT,W_ATT>(map,position,vol_centroid);
        return;
    }

    //    foreach_cell<VOLUME>(map, [&] (Vol v)
    //    {
    //        vol_centroid[v] = Surface::Geometry::volumeCentroid<PFP,V_ATT>(map, v, position,thread) ;
    //    }, AUTO, thread);
    foreach_cell<VOLUME>(map,
                         (
                             vol_centroid[bl::_1] = bl::bind(&Surface::Geometry::volumeCentroid<PFP,V_ATT>, boost::ref(map), bl::_1, boost::ref(position),thread)
            ), AUTO, thread);
}

template <typename PFP, typename V_ATT, typename W_ATT>
void computeCentroidELWVolumes(typename PFP::MAP& map, const V_ATT& position, W_ATT& vol_centroid, unsigned int thread)
{
    if ((CGoGN::Parallel::NumberOfThreads > 1) && (thread == 0))
    {
        Parallel::computeCentroidELWVolumes<PFP,V_ATT,W_ATT>(map,position,vol_centroid);
        return;
    }

    //    foreach_cell<VOLUME>(map, [&] (Vol v)
    //    {
    //        vol_centroid[v] = Surface::Geometry::volumeCentroidELW<PFP,V_ATT>(map, v, position,thread) ;
    //    }, AUTO, thread);
    foreach_cell<VOLUME>(map,
                         (
                             vol_centroid[bl::_1] = bl::bind(&Surface::Geometry::volumeCentroidELW<PFP,V_ATT>, boost::ref(map), bl::_1, boost::ref(position),thread)
            ), AUTO, thread);
}

template <typename PFP, typename V_ATT>
void computeNeighborhoodCentroidVertices(typename PFP::MAP& map, const V_ATT& position, V_ATT& vertex_centroid, unsigned int thread)
{
    if ((CGoGN::Parallel::NumberOfThreads > 1) && (thread == 0))
    {
        Parallel::computeNeighborhoodCentroidVertices<PFP,V_ATT>(map,position,vertex_centroid);
        return;
    }

    //    foreach_cell<VERTEX>(map, [&] (Vertex v)
    //    {
    //        vertex_centroid[v] = Volume::Geometry::vertexNeighborhoodCentroid<PFP,V_ATT>(map, v, position) ;
    //    }, AUTO, thread);
    foreach_cell<VOLUME>(map,
                         (
                             vertex_centroid[bl::_1] = bl::bind(&Surface::Geometry::vertexNeighborhoodCentroid<PFP,V_ATT>, boost::ref(map), bl::_1, boost::ref(position))
            ), AUTO, thread);
}


namespace Parallel
{

template <typename PFP, typename V_ATT, typename W_ATT>
void computeCentroidVolumes(typename PFP::MAP& map, const V_ATT& position, W_ATT& vol_centroid)
{
    //    CGoGN::Parallel::foreach_cell<VOLUME>(map, [&] (Vol v, unsigned int thr)
    //    {
    //        vol_centroid[v] = Surface::Geometry::volumeCentroid<PFP,V_ATT>(map, v, position, thr) ;
    //    });
    CGoGN::Parallel::foreach_cell<VOLUME>(map,
    (
        vol_centroid[bl::_1] = bl::bind(&Surface::Geometry::volumeCentroid<PFP,V_ATT>,boost::ref(map), bl::_1, boost::ref(position), bl::_2)
    )
            );
}

template <typename PFP, typename V_ATT, typename W_ATT>
void computeCentroidELWVolumes(typename PFP::MAP& map, const V_ATT& position, W_ATT& vol_centroid)
{
//    CGoGN::Parallel::foreach_cell<VOLUME>(map, [&] (Vol v, unsigned int thr)
//    {
//        vol_centroid[v] = Surface::Geometry::volumeCentroidELW<PFP,V_ATT>(map, v, position, thr) ;
//    });
    CGoGN::Parallel::foreach_cell<VOLUME>(map,
    (
        vol_centroid[bl::_1] = bl::bind(&Surface::Geometry::volumeCentroidELW<PFP,V_ATT>,boost::ref(map), bl::_1, boost::ref(position), bl::_2)
    )
            );
}

template <typename PFP, typename V_ATT>
void computeNeighborhoodCentroidVertices(typename PFP::MAP& map, const V_ATT& position, V_ATT& vertex_centroid)
{
//    CGoGN::Parallel::foreach_cell<VERTEX>(map, [&] (Vertex v, unsigned int thr)
//    {
//        vertex_centroid[v] = Volume::Geometry::vertexNeighborhoodCentroid<PFP,V_ATT>(map, v, position,thr) ;
//    }, FORCE_CELL_MARKING);
    CGoGN::Parallel::foreach_cell<VERTEX>(map,
    (
        vertex_centroid[bl::_1] = bl::bind(&Volume::Geometry::vertexNeighborhoodCentroid<PFP,V_ATT>,boost::ref(map), bl::_1, boost::ref(position), bl::_2)
    )
            );
}

} // namespace Parallel


} // namespace Geometry

} // namespace Volume

} // namespace Algo

} // namespace CGoGN
