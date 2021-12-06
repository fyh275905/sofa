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

#include <SofaMiscForceField/config.h>

#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/behavior/Mass.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/topology/TopologyData.h>
#include <sofa/type/vector.h>
#include <sofa/defaulttype/RigidTypes.h>

//VERY IMPORTANT FOR GRAPHS
#include <sofa/helper/map.h>
#include <sofa/core/topology/BaseMeshTopology.h>

namespace sofa::component::mass
{

template<class DataTypes, class TMassType>
class MeshMatrixMassInternalData
{
public:
    typedef typename DataTypes::Real Real;

    /// In case of non 3D template
    typedef type::Vec<3,Real> Vec3;
    /// assumes the geometry object type is 3D
    typedef defaulttype::StdVectorTypes< Vec3, Vec3, Real > GeometricalTypes;
};


template <class DataTypes, class TMassType>
class MeshMatrixMass : public core::behavior::Mass<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(MeshMatrixMass,DataTypes,TMassType), SOFA_TEMPLATE(core::behavior::Mass,DataTypes));

    typedef core::behavior::Mass<DataTypes> Inherited;
    typedef typename DataTypes::VecCoord                    VecCoord;
    typedef typename DataTypes::VecDeriv                    VecDeriv;
    typedef typename DataTypes::Coord                       Coord;
    typedef typename DataTypes::Deriv                       Deriv;
    typedef typename DataTypes::Real                        Real;
    typedef core::objectmodel::Data<VecCoord>               DataVecCoord;
    typedef core::objectmodel::Data<VecDeriv>               DataVecDeriv;
    typedef TMassType                                       MassType;
    typedef type::vector<MassType>                        MassVector;
    typedef type::vector<MassVector>                      MassVectorVector;

    using Index = sofa::Index;

    typedef typename MeshMatrixMassInternalData<DataTypes,TMassType>::GeometricalTypes GeometricalTypes;

    /// @name Data of mass information
    /// @{
    /// Mass density of the object
    Data< sofa::type::vector< Real > > d_massDensity;
    /// Total mass of the object
    Data< Real > d_totalMass;
    /// @}


    /// Values of the particles masses stored on vertices
    core::topology::PointData<type::vector<MassType> >  d_vertexMass;
    /// Values of the particles masses stored on edges
    core::topology::EdgeData<type::vector<MassType> >   d_edgeMass;

    /// if true, the mass of every element is computed based on the rest position rather than the position
    Data< bool > d_computeMassOnRest;
    /// to display the center of gravity of the system
    Data< bool >         d_showCenterOfGravity;
    /// scale to change the axis size
    Data< Real >         d_showAxisSize;  ///< factor length of the axis displayed (only used for rigids)
    /// if mass lumping should be performed (only compute mass on vertices)
    Data< bool >         d_lumping;
    /// if specific mass information should be outputed
    Data< bool >         d_printMass; ///< Boolean to print the mass
    Data< std::map < std::string, sofa::type::vector<double> > > f_graph; ///< Graph of the controlled potential

    /// Link to be set to the topology container in the component graph.
    SingleLink<MeshMatrixMass<DataTypes, TMassType>, sofa::core::topology::BaseMeshTopology, BaseLink::FLAG_STOREPATH | BaseLink::FLAG_STRONGLINK> l_topology;

protected:

    /// The type of topology to build the mass from the topology
    sofa::core::topology::TopologyElementType m_massTopologyType;
    Real m_massLumpingCoeff;

    MeshMatrixMass();
    ~MeshMatrixMass() override;

    sofa::core::topology::TopologyElementType checkTopology();
    void initTopologyHandlers(sofa::core::topology::TopologyElementType topologyType);
    void massInitialization();

    /// Internal data required for Cuda computation (copy of vertex mass for deviceRead)
    MeshMatrixMassInternalData<DataTypes, MassType> data;
    friend class MeshMatrixMassInternalData<DataTypes, MassType>;


public:
    virtual void clear();

    void reinit() override;
    void init() override;
    void handleEvent(sofa::core::objectmodel::Event *event) override;
    void doUpdateInternal() override;

    sofa::core::topology::TopologyElementType getMassTopologyType() const
    {
        return m_massTopologyType;
    }

    void setMassTopologyType(sofa::core::topology::TopologyElementType t)
    {
        m_massTopologyType = t;
    }

    int getMassCount() {
        return d_vertexMass.getValue().size();
    }

    /// Print key mass informations (totalMass, vertexMass and massDensity)
    void printMass();

    /// Compute the mass from input values
    void computeMass();


    /// @name Read and write access functions in mass information
    /// @{
    virtual const sofa::type::vector< Real > &getVertexMass();
    virtual const sofa::type::vector< Real > &getMassDensity();
    virtual const Real &getTotalMass();

    virtual void setVertexMass(sofa::type::vector< Real > vertexMass);
    virtual void setMassDensity(sofa::type::vector< Real > massDensity);
    virtual void setMassDensity(Real massDensityValue);
    virtual void setTotalMass(Real totalMass);

    virtual void addMassDensity(const sofa::type::vector< Index >& indices,        
        const sofa::type::vector< sofa::type::vector< Index > >& ancestors,
        const sofa::type::vector< sofa::type::vector< double > >& coefs);
    /// @}


    /// @name Check and standard initialization functions from mass information
    /// @{
    virtual bool checkVertexMass();
    virtual void initFromVertexMass();

    virtual bool checkMassDensity();
    virtual void initFromMassDensity();

    virtual bool checkTotalMass();
    virtual void checkTotalMassInit();
    virtual void initFromTotalMass();

    bool checkEdgeMass();
    void initFromVertexAndEdgeMass();
    /// @}


    /// Copy the vertex mass scalar (in case of CudaTypes)
    void copyVertexMass();


    // -- Mass interface
    void addMDx(const core::MechanicalParams*, DataVecDeriv& f, const DataVecDeriv& dx, SReal factor) override;

    void accFromF(const core::MechanicalParams*, DataVecDeriv& a, const DataVecDeriv& f) override; // This function can't be used as it use M^-1

    void addForce(const core::MechanicalParams*, DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v) override;

    SReal getKineticEnergy(const core::MechanicalParams*, const DataVecDeriv& v) const override;  ///< vMv/2 using dof->getV() override

    SReal getPotentialEnergy(const core::MechanicalParams*, const DataVecCoord& x) const override;   ///< Mgx potential in a uniform gravity field, null at origin

    type::Vector6 getMomentum(const core::MechanicalParams* mparams, const DataVecCoord& x, const DataVecDeriv& v) const override;  ///< (Mv,cross(x,Mv)) override

    void addGravityToV(const core::MechanicalParams* mparams, DataVecDeriv& d_v) override;

    bool isDiagonal() const override { return isLumped(); }



    /// Add Mass contribution to global Matrix assembling
    void addMToMatrix(const core::MechanicalParams *mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix) override;

    SReal getElementMass(Index index) const override;
    void getElementMass(Index index, linearalgebra::BaseMatrix *m) const override;

    void draw(const core::visual::VisualParams* vparams) override;

    /// Answer wether mass matrix is lumped or not
    bool isLumped() const { return d_lumping.getValue(); }


protected:
    /** Method to initialize @sa MassType when a new Point is created to compute mass coefficient matrix.
    * Will be set as creation callback in the PointData @sa d_vertexMass
    */
    void applyVertexMassCreation(Index pointIndex, MassType& VertexMass,
        const core::topology::BaseMeshTopology::Point& point,
        const sofa::type::vector< Index >&,
        const sofa::type::vector< double >&);

    /** Method to update @sa d_vertexMass when a Point is removed.
    * Will be set as destruction callback in the PointData @sa d_vertexMass
    */
    void applyVertexMassDestruction(Index, MassType&);


    /** Method to update @sa d_vertexMass using mass matrix coefficient when a new Triangle is created.
    * Will be set as callback in the PointData @sa d_vertexMass to update the mass vector when TRIANGLESADDED event is fired.
    */
    void applyVertexMassTriangleCreation(const sofa::type::vector< Index >& triangleAdded,
        const sofa::type::vector< core::topology::BaseMeshTopology::Triangle >& elems,
        const sofa::type::vector< sofa::type::vector< Index > >& ancestors,
        const sofa::type::vector< sofa::type::vector< double > >& coefs);

    /** Method to update @sa d_vertexMass using mass matrix coefficient when a Triangle is removed.
    * Will be set as callback in the PointData @sa d_vertexMass to update the mass vector when TRIANGLESREMOVED event is fired.
    */
    void applyVertexMassTriangleDestruction(const sofa::type::vector<Index>& triangleRemoved);


    /** Method to update @sa d_vertexMass using mass matrix coefficient when a new Quad is created.
    * Will be set as callback in the PointData @sa d_vertexMass to update the mass vector when QUADSADDED event is fired.
    */
    void applyVertexMassQuadCreation(const sofa::type::vector< Index >& quadAdded,
        const sofa::type::vector< core::topology::BaseMeshTopology::Quad >& elems,
        const sofa::type::vector< sofa::type::vector< Index > >& ancestors,
        const sofa::type::vector< sofa::type::vector< double > >& coefs);

    /** Method to update @sa d_vertexMass using mass matrix coefficient when a Quad is removed.
    * Will be set as callback in the PointData @sa d_vertexMass to update the mass vector when QUADSREMOVED event is fired.
    */
    void applyVertexMassQuadDestruction(const sofa::type::vector<Index>& quadRemoved);


    /** Method to update @sa d_vertexMass using mass matrix coefficient when a new Tetrahedron is created.
    * Will be set as callback in the PointData @sa d_vertexMass to update the mass vector when TETRAHEDRAADDED event is fired.
    */
    void applyVertexMassTetrahedronCreation(const sofa::type::vector< Index >& tetrahedronAdded,
        const sofa::type::vector< core::topology::BaseMeshTopology::Tetrahedron >& elems,
        const sofa::type::vector< sofa::type::vector< Index > >& ancestors,
        const sofa::type::vector< sofa::type::vector< double > >& coefs);

    /** Method to update @sa d_vertexMass using mass matrix coefficient when a Tetrahedron is removed.
    * Will be set as callback in the PointData @sa d_vertexMass to update the mass vector when TETRAHEDRAREMOVED event is fired.
    */
    void applyVertexMassTetrahedronDestruction(const sofa::type::vector<Index>& tetrahedronRemoved);

    
    /** Method to update @sa d_vertexMass using mass matrix coefficient when a new Hexahedron is created.
    * Will be set as callback in the PointData @sa d_vertexMass to update the mass vector when HEXAHEDRAADDED event is fired.
    */
    void applyVertexMassHexahedronCreation(const sofa::type::vector< Index >& hexahedronAdded,
        const sofa::type::vector< core::topology::BaseMeshTopology::Hexahedron >& elems,
        const sofa::type::vector< sofa::type::vector< Index > >& ancestors,
        const sofa::type::vector< sofa::type::vector< double > >& coefs);

    /** Method to update @sa d_vertexMass using mass matrix coefficient when a Hexahedron is removed.
    * Will be set as callback in the PointData @sa d_vertexMass to update the mass vector when HEXAHEDRAREMOVED event is fired.
    */
    void applyVertexMassHexahedronDestruction(const sofa::type::vector<Index>& hexahedronRemoved);
   


    /** Method to initialize @sa MassType when a new Edge is created to compute mass coefficient matrix.
    * Will be set as creation callback in the EdgeData @sa d_edgeMass
    */
    void applyEdgeMassCreation(Index edgeIndex, MassType& EdgeMass,
        const core::topology::BaseMeshTopology::Edge&,
        const sofa::type::vector< Index >&,
        const sofa::type::vector< double >&);

    /** Method to update @sa d_edgeMass when a Edge is removed.
    * Will be set as destruction callback in the EdgeData @sa d_edgeMass
    */
    void applyEdgeMassDestruction(Index, MassType&);

    
    /** Method to update @sa d_edgeMass using mass matrix coefficient when a new Triangle is created.
    * Will be set as callback in the EdgeData @sa d_edgeMass to update the mass vector when TRIANGLESADDED event is fired.
    */
    void applyEdgeMassTriangleCreation(const sofa::type::vector< Index >& triangleAdded,
        const sofa::type::vector< core::topology::BaseMeshTopology::Triangle >& elems,
        const sofa::type::vector< sofa::type::vector< Index > >& ancestors,
        const sofa::type::vector< sofa::type::vector< double > >& coefs);

    /** Method to update @sa d_edgeMass using mass matrix coefficient when a Triangle is removed.
    * Will be set as callback in the EdgeData @sa d_edgeMass to update the mass vector when TRIANGLESREMOVED event is fired.
    */
    void applyEdgeMassTriangleDestruction(const sofa::type::vector<Index>& triangleRemoved);


    /** Method to update @sa d_edgeMass using mass matrix coefficient when a new Quad is created.
    * Will be set as callback in the EdgeData @sa d_edgeMass to update the mass vector when QUADSADDED event is fired.
    */
    void applyEdgeMassQuadCreation(const sofa::type::vector< Index >& quadAdded,
        const sofa::type::vector< core::topology::BaseMeshTopology::Quad >& elems,
        const sofa::type::vector< sofa::type::vector< Index > >& ancestors,
        const sofa::type::vector< sofa::type::vector< double > >& coefs);

    /** Method to update @sa d_edgeMass using mass matrix coefficient when a Quad is removed.
    * Will be set as callback in the EdgeData @sa d_edgeMass to update the mass vector when QUADSREMOVED event is fired.
    */
    void applyEdgeMassQuadDestruction(const sofa::type::vector<Index>& quadRemoved);


    /** Method to update @sa d_edgeMass using mass matrix coefficient when a new Tetrahedron is created.
    * Will be set as callback in the EdgeData @sa d_edgeMass to update the mass vector when TETRAHEDRAADDED event is fired.
    */
    void applyEdgeMassTetrahedronCreation(const sofa::type::vector< Index >& tetrahedronAdded,
        const sofa::type::vector< core::topology::BaseMeshTopology::Tetrahedron >& elems,
        const sofa::type::vector< sofa::type::vector< Index > >& ancestors,
        const sofa::type::vector< sofa::type::vector< double > >& coefs);

    /** Method to update @sa d_edgeMass using mass matrix coefficient when a Tetrahedron is removed.
    * Will be set as callback in the EdgeData @sa d_edgeMass to update the mass vector when TETRAHEDRAREMOVED event is fired.
    */
    void applyEdgeMassTetrahedronDestruction(const sofa::type::vector<Index>& tetrahedronRemoved);


    /** Method to update @sa d_edgeMass using mass matrix coefficient when a new Hexahedron is created.
    * Will be set as callback in the EdgeData @sa d_edgeMass to update the mass vector when HEXAHEDRAADDED event is fired.
    */
    void applyEdgeMassHexahedronCreation(const sofa::type::vector< Index >& hexahedronAdded,
        const sofa::type::vector< core::topology::BaseMeshTopology::Hexahedron >& elems,
        const sofa::type::vector< sofa::type::vector< Index > >& ancestors,
        const sofa::type::vector< sofa::type::vector< double > >& coefs);

    /** Method to update @sa d_vertexMass using mass matrix coefficient when a Hexahedron is removed.
    * Will be set as callback in the EdgeData @sa d_edgeMass to update the mass vector when HEXAHEDRAREMOVED event is fired.
    */
    void applyEdgeMassHexahedronDestruction(const sofa::type::vector<Index>& /*indices*/);


    sofa::core::topology::BaseMeshTopology* m_topology;
};

#if  !defined(SOFA_COMPONENT_MASS_MESHMATRIXMASS_CPP)
extern template class SOFA_SOFAMISCFORCEFIELD_API MeshMatrixMass<defaulttype::Vec3Types,defaulttype::Vec3Types::Real>;
extern template class SOFA_SOFAMISCFORCEFIELD_API MeshMatrixMass<defaulttype::Vec2Types,defaulttype::Vec2Types::Real>;
extern template class SOFA_SOFAMISCFORCEFIELD_API MeshMatrixMass<defaulttype::Vec1Types,defaulttype::Vec1Types::Real>;

#endif

} // namespace sofa::component::mass
