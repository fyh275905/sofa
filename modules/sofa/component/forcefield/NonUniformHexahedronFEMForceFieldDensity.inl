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
#ifndef SOFA_COMPONENT_FORCEFIELD_NONUNIFORMHEXAHEDRONFEMFORCEFIELDDENSITY_INL
#define SOFA_COMPONENT_FORCEFIELD_NONUNIFORMHEXAHEDRONFEMFORCEFIELDDENSITY_INL

#include <sofa/component/forcefield/NonUniformHexahedronFEMForceFieldDensity.h>
#include <sofa/helper/system/FileRepository.h>

#include <fstream>
using std::cerr;
using std::endl;
using std::set;





namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;



template <class DataTypes>
void NonUniformHexahedronFEMForceFieldDensity<DataTypes>::init()
{
    cerr<<"NonUniformHexahedronFEMForceFieldDensity<DataTypes>::init()\n";

    if(this->_alreadyInit)return;
    else this->_alreadyInit=true;

// 	NonUniformHexahedronFEMForceFieldAndMass<DataTypes>::init();

    this->core::componentmodel::behavior::ForceField<DataTypes>::init();

    if( this->getContext()->getTopology()==NULL )
    {
        std::cerr << "ERROR(NonUniformHexahedronFEMForceFieldDensity): object must have a Topology.\n";
        return;
    }

    this->_mesh = dynamic_cast<sofa::component::topology::MeshTopology*>(this->getContext()->getTopology());
    if ( this->_mesh==NULL)
    {
        std::cerr << "ERROR(NonUniformHexahedronFEMForceFieldDensity): object must have a MeshTopology.\n";
        return;
    }
    else if( this->_mesh->getNbCubes()<=0 )
    {
        std::cerr << "ERROR(NonUniformHexahedronFEMForceFieldDensity): object must have a hexahedric MeshTopology.\n";
        std::cerr << this->_mesh->getName()<<std::endl;
        std::cerr << this->_mesh->getTypeName()<<std::endl;
        cerr<<this->_mesh->getNbPoints()<<endl;
        return;
    }

    this->_indexedElements = & (this->_mesh->getCubes());


    this->_sparseGrid = dynamic_cast<topology::SparseGridTopology*>(this->_mesh);



    if (this->_initialPoints.getValue().size() == 0)
    {
        VecCoord& p = *this->mstate->getX();
        this->_initialPoints.setValue(p);
    }

    this->_materialsStiffnesses.resize(this->_indexedElements->size() );
    this->_rotations.resize( this->_indexedElements->size() );
    this->_rotatedInitialElements.resize(this->_indexedElements->size());
// 	stiffnessFactor.resize(this->_indexedElements->size());


// 	NonUniformHexahedronFEMForceFieldAndMass<DataTypes>::init();

    // verify if it is wanted and possible to compute non-uniform stiffness
    if( !this->_nbVirtualFinerLevels.getValue() || !this->_sparseGrid || this->_sparseGrid->getNbVirtualFinerLevels() < this->_nbVirtualFinerLevels.getValue()  )
    {
// 		this->_nbVirtualFinerLevels.setValue(0);
        cerr<<"WARNING: NonUniformHexahedronFEMForceFieldDensity<DataTypes>::init    conflict in nb of virtual levels between ForceField "<<this->getName()<<" and SparseGrid "<<this->_sparseGrid->getName()<<" -> classical uniform properties are used"<<endl;
    }
    else
    {

    }



    this->_elementStiffnesses.beginEdit()->resize(this->_indexedElements->size());
    this->_elementMasses.beginEdit()->resize(this->_indexedElements->size());

    //Load Gray scale density from RAW file

    std::string path = densityFile.getValue();

    if (!densityFile.getValue().empty() && sofa::helper::system::DataRepository.findFile(path))
    {
        densityFile = sofa::helper::system::DataRepository.getFile ( densityFile.getValue() );
        FILE *file = fopen( densityFile.getValue().c_str(), "r" );
        voxels.resize(dimensionDensityFile.getValue()[2]);
        for (unsigned int z=0; z<dimensionDensityFile.getValue()[2]; ++z)
        {
            voxels[z].resize(dimensionDensityFile.getValue()[0]);
            for (unsigned int x=0; x<dimensionDensityFile.getValue()[0]; ++x)
            {
                voxels[z][x].resize(dimensionDensityFile.getValue()[1]);
                for (unsigned int y=0; y<dimensionDensityFile.getValue()[1]; ++y)
                {
                    voxels[z][x][y] = getc(file);
                }
            }
        }

        fclose(file);
    }
    //////////////////////


    if (this->f_method.getValue() == "large")
        this->setMethod(HexahedronFEMForceFieldT::LARGE);
    else if (this->f_method.getValue() == "polar")
        this->setMethod(HexahedronFEMForceFieldT::POLAR);


    for (unsigned int i=0; i<this->_indexedElements->size(); ++i)
    {


        Vec<8,Coord> nodes;
        for(int w=0; w<8; ++w)
            nodes[w] = this->_initialPoints.getValue()[(*this->_indexedElements)[i][this->_indices[w]]];


        typename HexahedronFEMForceFieldT::Transformation R_0_1;

        if( this->method == HexahedronFEMForceFieldT::LARGE )
        {
            Coord horizontal;
            horizontal = (nodes[1]-nodes[0] + nodes[2]-nodes[3] + nodes[5]-nodes[4] + nodes[6]-nodes[7])*.25;
            Coord vertical;
            vertical = (nodes[3]-nodes[0] + nodes[2]-nodes[1] + nodes[7]-nodes[4] + nodes[6]-nodes[5])*.25;
            computeRotationLarge( R_0_1, horizontal,vertical);
        }
        else
            computeRotationPolar( R_0_1, nodes);

        for(int w=0; w<8; ++w)
            this->_rotatedInitialElements[i][w] = R_0_1*this->_initialPoints.getValue()[(*this->_indexedElements)[i][this->_indices[w]]];

        computeCoarseElementStiffness( (*this->_elementStiffnesses.beginEdit())[i],
                (*this->_elementMasses.beginEdit())[i],i,0);
    }
    //////////////////////


// 	post-traitement of non-uniform stiffness
    if( this->_nbVirtualFinerLevels.getValue() )
    {
        this->_sparseGrid->setNbVirtualFinerLevels(0);
        //delete undesirable sparsegrids and hexa
        for(int i=0; i<this->_sparseGrid->getNbVirtualFinerLevels(); ++i)
            delete this->_sparseGrid->_virtualFinerLevels[i];
        this->_sparseGrid->_virtualFinerLevels.resize(0);
    }



    if(this->_useMass.getValue() )
    {

        MassT::init();
        this->_particleMasses.resize( this->_initialPoints.getValue().size() );


        int i=0;
        for(typename VecElement::const_iterator it = this->_indexedElements->begin() ; it != this->_indexedElements->end() ; ++it, ++i)
        {
            Vec<8,Coord> nodes;
            for(int w=0; w<8; ++w)
                nodes[w] = this->_initialPoints.getValue()[(*it)[this->_indices[w]]];

            // volume of a element
            Real volume = (nodes[1]-nodes[0]).norm()*(nodes[3]-nodes[0]).norm()*(nodes[4]-nodes[0]).norm();

            volume *= (Real) (this->_sparseGrid->getType(i)==topology::SparseGridTopology::BOUNDARY?.5:1.0);

            // mass of a particle...
            Real mass = Real (( volume * this->_density.getValue() ) / 8.0 );

            // ... is added to each particle of the element
            for(int w=0; w<8; ++w)
                this->_particleMasses[ (*it)[w] ] += mass;
        }
    }
    else
    {
        this->_particleMasses.resize( this->_initialPoints.getValue().size() );

        Real mass = this->_totalMass.getValue() / Real(this->_indexedElements->size());
        for(unsigned i=0; i<this->_particleMasses.size(); ++i)
            this->_particleMasses[ i ] = mass;
    }

}


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


template<class DataTypes>
void NonUniformHexahedronFEMForceFieldDensity<DataTypes>::computeCoarseElementStiffness( ElementStiffness &coarseElement, ElementMass &coarseMassElement, const int elementIndice,  int level)
{
    /*
    //  //Get the 8 indices of the coarser Hexa
      const helper::fixed_array<unsigned int,8>& points = this->_sparseGrid->getHexas()[elementIndice];
    // //       Get the 8 points of the coarser Hexa
      helper::fixed_array<Coord,8> nodes;
      for (unsigned int k=0;k<8;++k) nodes[k] =  this->_sparseGrid->getPointPos(points[this->_indices[k]]);


      MaterialStiffness mat;
      computeMaterialStiffness(mat,  this->f_youngModulus.getValue(),this->f_poissonRatio.getValue());


      HexahedronFEMForceFieldT::computeElementStiffness(*coarseElement,mat,nodes,elementIndice); // classical stiffness
      HexahedronFEMForceFieldAndMassT::computeElementMass(*coarseMassElement,nodes,elementIndice);




      return;*/



    if (level == this->_nbVirtualFinerLevels.getValue())
    {

        //Get the 8 indices of the coarser Hexa
        const helper::fixed_array<unsigned int,8>& points = this->_sparseGrid->_virtualFinerLevels[0]->getHexas()[elementIndice];
        //Get the 8 points of the coarser Hexa
        helper::fixed_array<Coord,8> nodes;
        for (unsigned int k=0; k<8; ++k) nodes[k] =  this->_sparseGrid->_virtualFinerLevels[0]->getPointPos(points[this->_indices[k]]);


//       //given an elementIndice, find the 8 others from the sparse grid
//       //compute MaterialStiffness
        MaterialStiffness mat;

        double young = this->f_youngModulus.getValue();
        double grayScale;
        if (!densityFile.getValue().empty())
        {
            int indexInRegularGrid = this->_sparseGrid->_virtualFinerLevels[0]->_indicesOfCubeinRegularGrid[elementIndice];
            Vector3 coordinates = this->_sparseGrid->_virtualFinerLevels[0]->_regularGrid.getCubeCoordinate(indexInRegularGrid);

            Vector3 factor = Vector3(
                    dimensionDensityFile.getValue()[0]/((float)this->_sparseGrid->_virtualFinerLevels[0]->_regularGrid.getNx()),
                    dimensionDensityFile.getValue()[1]/((float)this->_sparseGrid->_virtualFinerLevels[0]->_regularGrid.getNy()),
                    dimensionDensityFile.getValue()[2]/((float)this->_sparseGrid->_virtualFinerLevels[0]->_regularGrid.getNz())
                    );

            grayScale = 1+(voxels[factor[2]*coordinates[2]][factor[0]*coordinates[0]][factor[1]*coordinates[1]])/256.0;
        }
//       grayScale = 1.0;
        computeMaterialStiffness(mat,  this->f_youngModulus.getValue(),this->f_poissonRatio.getValue());

        //Nodes are found using Sparse Grid
        HexahedronFEMForceFieldAndMassT::computeElementStiffness(coarseElement,mat,nodes,elementIndice, this->_sparseGrid->_virtualFinerLevels[0]->getType(elementIndice)==topology::SparseGridTopology::BOUNDARY?.5:1.0); // classical stiffness
        HexahedronFEMForceFieldAndMassT::computeElementMass(coarseMassElement,nodes,elementIndice,this->_sparseGrid->_virtualFinerLevels[0]->getType(elementIndice)==topology::SparseGridTopology::BOUNDARY?.5:1.0);

    }
    else
    {
        helper::fixed_array<int,8> finerChildren;
        if (level == 0)
        {
            finerChildren = this->_sparseGrid->_hierarchicalCubeMap[elementIndice];
        }
        else
        {
            finerChildren = this->_sparseGrid->_virtualFinerLevels[this->_nbVirtualFinerLevels.getValue()-level]->_hierarchicalCubeMap[elementIndice];
        }

//     std::cerr<<finerChildren<<"\n";
        //Get the 8 points of the coarser Hexa
        for ( int i=0; i<8; ++i)
        {
            if (finerChildren[i] != -1)
            {
                ElementStiffness childElement;
                ElementMass childMassElement;
                computeCoarseElementStiffness(childElement, childMassElement, finerChildren[i], level+1);
                this->addFineToCoarse(coarseElement, childElement, i);
                this->addFineToCoarse(coarseMassElement, childMassElement, i);
            }
        }
    }
}


template<class DataTypes>
void NonUniformHexahedronFEMForceFieldDensity<DataTypes>::computeMaterialStiffness(MaterialStiffness &m, double youngModulus, double poissonRatio)
{
    m[0][0] = m[1][1] = m[2][2] = 1;
    m[0][1] = m[0][2] = m[1][0]= m[1][2] = m[2][0] =  m[2][1] = poissonRatio/(1-poissonRatio);
    m[0][3] = m[0][4] =	m[0][5] = 0;
    m[1][3] = m[1][4] =	m[1][5] = 0;
    m[2][3] = m[2][4] =	m[2][5] = 0;
    m[3][0] = m[3][1] = m[3][2] = m[3][4] =	m[3][5] = 0;
    m[4][0] = m[4][1] = m[4][2] = m[4][3] =	m[4][5] = 0;
    m[5][0] = m[5][1] = m[5][2] = m[5][3] =	m[5][4] = 0;
    m[3][3] = m[4][4] = m[5][5] = (1-2*poissonRatio)/(2*(1-poissonRatio));
    m *= (youngModulus*(1-poissonRatio))/((1+poissonRatio)*(1-2*poissonRatio));
    // S = [ U V V 0 0 0 ]
    //     [ V U V 0 0 0 ]
    //     [ V V U 0 0 0 ]
    //     [ 0 0 0 W 0 0 ]
    //     [ 0 0 0 0 W 0 ]
    //     [ 0 0 0 0 0 W ]
    // with U = y * (1-p)/( (1+p)(1-2p))
    //      V = y *    p /( (1+p)(1-2p))
    //      W = y *  1   /(2(1+p)) = (U-V)/2
}

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
