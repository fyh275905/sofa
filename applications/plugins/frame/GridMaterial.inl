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
#ifndef SOFA_COMPONENT_MATERIAL_GRIDMATERIAL_INL
#define SOFA_COMPONENT_MATERIAL_GRIDMATERIAL_INL

#include "GridMaterial.h"
#include <sofa/helper/gl/Color.h>
#include <sofa/helper/gl/glText.inl>
#include <queue>
#include <string>
//#include <omp.h>

namespace sofa
{

namespace component
{

namespace material
{
using helper::WriteAccessor;

template<class MaterialTypes>
GridMaterial< MaterialTypes>::GridMaterial()
    : Inherited()
    , voxelSize ( initData ( &voxelSize, SCoord ( 0,0,0 ), "voxelSize", "Voxel size." ) )
    , origin ( initData ( &origin, SCoord ( 0,0,0 ), "origin", "Grid origin." ) )
    , dimension ( initData ( &dimension, GCoord ( 0,0,0 ), "dimension", "Grid dimensions." ) )
    , labelToStiffnessPairs ( initData ( &labelToStiffnessPairs, "labelToStiffnessPairs","Correspondances between grid value and material stiffness." ) )
    , labelToDensityPairs ( initData ( &labelToDensityPairs, "labelToDensityPairs","Correspondances between grid value and material density." ) )
    , labelToBulkModulusPairs ( initData ( &labelToBulkModulusPairs, "labelToBulkModulusPairs","Correspondances between grid value and material bulk modulus." ) )
    , labelToPoissonRatioPairs ( initData ( &labelToPoissonRatioPairs, "labelToPoissonRatioPairs","Correspondances between grid value and material Poisson Ratio." ) )
    , bulkModulus ( initData ( &bulkModulus, "bulkModulus","Sample bulk Modulus." ) )
    , stiffness ( initData ( &stiffness, "stiffness","Sample stiffness." ) )
    , density ( initData ( &density, "density","Sample density." ) )
    , poissonRatio ( initData ( &poissonRatio, "poissonRatio","Sample poisson Ratio." ) )
    , imageFile( initData(&imageFile,"imageFile","Image file."))
    , weightFile( initData(&weightFile,"weightFile","Voxel weight file."))
    , distanceType ( initData ( &distanceType,"distanceType","Distance measure." ) )
    , biasDistances ( initData ( &biasDistances,true, "biasDistances","Bias distances according to stiffness." ) )
    , showVoxels ( initData ( &showVoxels, "showVoxelData","Show voxel data." ) )
    , showWeightIndex ( initData ( &showWeightIndex, ( unsigned int ) 0, "showWeightIndex","Weight index." ) )
    , showPlane ( initData ( &showPlane, GCoord ( -1,-1,-1 ), "showPlane","Indices of slices to be shown." ) )
    , show3DValues ( initData ( &show3DValues, false, "show3DValues","When show plane is activated, values are displayed in 3D." ) )
    , vboSupported(false)
    , vboValuesId1(0)
    , vboValuesId2(0)
    , valuesVertices(NULL)
    , valuesNormals(NULL)
    , valuesIndices(NULL)
{
    helper::OptionsGroup distanceTypeOptions(3,"Geodesic", "HeatDiffusion", "AnisotropicHeatDiffusion");
    distanceTypeOptions.setSelectedItem(DISTANCE_GEODESIC);
    distanceType.setValue(distanceTypeOptions);

    helper::OptionsGroup showVoxelsOptions(8,"None", "Data", "Stiffness", "Density", "Bulk modulus", "Poisson ratio", "Voronoi", "Distances", "Weights");
    showVoxelsOptions.setSelectedItem(SHOWVOXELS_NONE);
    showVoxels.setValue(showVoxelsOptions);
}


template<class MaterialTypes>
GridMaterial< MaterialTypes >::~GridMaterial()
{
    deleteVBO(vboValuesId2);
}


template<class MaterialTypes>
void GridMaterial< MaterialTypes>::init()
{
    if (imageFile.isSet())
    {
        infoFile=imageFile.getFullPath(); infoFile.replace(infoFile.find_last_of('.')+1,infoFile.size(),"nfo");
        bool writeinfos=false;	if(!loadInfos()) writeinfos=true;
        loadImage();
        if (writeinfos) saveInfos();
    }
    if (weightFile.isSet()) loadWeightRepartion();
    showedrepartition=-1;
    showWireframe=false;

    //TEST
    /*
    if(v_weights.size()!=nbVoxels)
    {

    VecSCoord points;	  points.push_back(SCoord(0.8,0,0.3));	  points.push_back(SCoord(-0.534989,-0.661314,-0.58));	  points.push_back(SCoord(-0.534955,0.661343,-0.58));	  points.push_back(SCoord(0.257823,-0.46005,-0.63));	  points.push_back(SCoord(0.257847,0.460036,-0.63));	  points.push_back(SCoord(-0.15,0,0.2 ));
    computeUniformSampling(points,6);
    computeWeights(points);

    VecSCoord samples;
    computeUniformSampling(samples,50);

    //	  Vec<nbRef,unsigned int> reps; vector<Real> w; VecSGradient dw; VecSHessian ddw;
    // for(unsigned int j=0;j<samples.size();j++) lumpWeightsRepartition(samples[j],reps,w,&dw,&ddw);
    }*/
    ////

    initVBO();
    genListCube();

    Inherited::init();
}


template<class MaterialTypes>
void GridMaterial< MaterialTypes>::reinit()
{
    updateSampleMaterialProperties();
}

// WARNING : The strain is defined as exx, eyy, ezz, exy, eyz, ezx
template<class MaterialTypes>
void GridMaterial< MaterialTypes>::computeStress  ( VecStrain1& stresses, VecStrStr* stressStrainMatrices, const VecStrain1& strains, const VecStrain1& /*strainRates*/, const VecMaterialCoord& /*point*/  )
{
    Real stressDiagonal, stressOffDiagonal, shear, poissRatio,youngModulus;

    for( unsigned int i=0; i<stresses.size(); i++ )
    {
        if(poissonRatio.getValue().size()>i) poissRatio= poissonRatio.getValue()[i]; else poissRatio=0;
        if(stiffness.getValue().size()>i) youngModulus= stiffness.getValue()[i]; else youngModulus=1;

        Real f = youngModulus/((1 + poissRatio)*(1 - 2 * poissRatio));
        stressDiagonal = f * (1 - poissRatio);
        stressOffDiagonal = poissRatio * f;
        shear = f * (1 - 2 * poissRatio);

        Str& stress = stresses[i][0];
        const Str& strain = strains[i][0];

        stress = hookeStress( strain, stressDiagonal, stressOffDiagonal, shear );

        if( stressStrainMatrices != NULL )
        {
            fillHookeMatrix( (*stressStrainMatrices)[i], stressDiagonal, stressOffDiagonal,  shear );
        }
    }
}


// WARNING : The strain is defined as exx, eyy, ezz, exy, eyz, ezx
template<class MaterialTypes>
void GridMaterial< MaterialTypes>::computeStress  ( VecStrain4& stresses, VecStrStr* stressStrainMatrices, const VecStrain4& strains, const VecStrain4& /*strainRates*/, const VecMaterialCoord& /*point*/  )
{
    Real stressDiagonal, stressOffDiagonal, shear, poissRatio,youngModulus;

    for( unsigned int i=0; i<stresses.size(); i++ )
    {
        if(poissonRatio.getValue().size()>i) poissRatio= poissonRatio.getValue()[i]; else poissRatio=0;
        if(stiffness.getValue().size()>i) youngModulus= stiffness.getValue()[i]; else youngModulus=1;

        Real f = youngModulus/((1 + poissRatio)*(1 - 2 * poissRatio));
        stressDiagonal = f * (1 - poissRatio);
        stressOffDiagonal = poissRatio * f;
        shear = f * (1 - 2 * poissRatio);

        for(unsigned int j=0; j<4; j++ )
        {
            Str& stress = stresses[i][j];
            const Str& strain = strains[i][j];

            stress = hookeStress( strain, stressDiagonal, stressOffDiagonal, shear );

            if( stressStrainMatrices != NULL )
            {
                fillHookeMatrix( (*stressStrainMatrices)[i], stressDiagonal, stressOffDiagonal,  shear );
            }
        }
    }
}

// WARNING : The strain is defined as exx, eyy, ezz, exy, eyz, ezx
template<class MaterialTypes>
void GridMaterial< MaterialTypes>::computeStress  ( VecStrain10& stresses, VecStrStr* stressStrainMatrices, const VecStrain10& strains, const VecStrain10& /*strainRates*/, const VecMaterialCoord& /*point*/  )
{
    Real stressDiagonal, stressOffDiagonal, shear, poissRatio, youngModulus;

    for( unsigned int i=0; i<stresses.size(); i++ )
    {
        if(poissonRatio.getValue().size()>i) poissRatio= poissonRatio.getValue()[i]; else poissRatio=0;
        if(stiffness.getValue().size()>i) youngModulus= stiffness.getValue()[i]; else youngModulus=1;

        Real f = youngModulus/((1 + poissRatio)*(1 - 2 * poissRatio));
        stressDiagonal = f * (1 - poissRatio);
        stressOffDiagonal = poissRatio * f;
        shear = f * (1 - 2 * poissRatio);

        for(unsigned int j=0; j<10; j++ )
        {
            Str& stress = stresses[i][j];
            const Str& strain = strains[i][j];

            stress = hookeStress( strain, stressDiagonal, stressOffDiagonal, shear );

            if( stressStrainMatrices != NULL )
            {
                fillHookeMatrix( (*stressStrainMatrices)[i], stressDiagonal, stressOffDiagonal,  shear );
            }
        }
    }
}

// WARNING : The strain is defined as exx, eyy, ezz, exy, eyz, ezx
template<class MaterialTypes>
void GridMaterial< MaterialTypes>::computeStressChange  ( VecStrain1& stresses, const VecStrain1& strains, const VecMaterialCoord& /*point*/  )
{
    Real stressDiagonal, stressOffDiagonal, shear, poissRatio,youngModulus;

    for( unsigned int i=0; i<stresses.size(); i++ )
    {
        if(poissonRatio.getValue().size()>i) poissRatio= poissonRatio.getValue()[i]; else poissRatio=0;
        if(stiffness.getValue().size()>i) youngModulus= stiffness.getValue()[i]; else youngModulus=1;

        Real f = youngModulus/((1 + poissRatio)*(1 - 2 * poissRatio));
        stressDiagonal = f * (1 - poissRatio);
        stressOffDiagonal = poissRatio * f;
        shear = f * (1 - 2 * poissRatio);

        for(unsigned int j=0; j<1; j++ )
        {

            stresses[i][j] = hookeStress( strains[i][j], stressDiagonal, stressOffDiagonal, shear );
        }
    }
}

// WARNING : The strain is defined as exx, eyy, ezz, exy, eyz, ezx
template<class MaterialTypes>
void GridMaterial< MaterialTypes>::computeStressChange  ( VecStrain4& stresses, const VecStrain4& strains, const VecMaterialCoord& /*point*/  )
{
    Real stressDiagonal, stressOffDiagonal, shear, poissRatio,youngModulus;

    for( unsigned int i=0; i<stresses.size(); i++ )
    {
        if(poissonRatio.getValue().size()>i) poissRatio= poissonRatio.getValue()[i]; else poissRatio=0;
        if(stiffness.getValue().size()>i) youngModulus= stiffness.getValue()[i]; else youngModulus=1;

        Real f = youngModulus/((1 + poissRatio)*(1 - 2 * poissRatio));
        stressDiagonal = f * (1 - poissRatio);
        stressOffDiagonal = poissRatio * f;
        shear = f * (1 - 2 * poissRatio);

        for(unsigned int j=0; j<4; j++ )
        {

            stresses[i][j] = hookeStress( strains[i][j], stressDiagonal, stressOffDiagonal, shear );
        }
    }
}

// WARNING : The strain is defined as exx, eyy, ezz, exy, eyz, ezx
template<class MaterialTypes>
void GridMaterial< MaterialTypes>::computeStressChange  ( VecStrain10& stresses, const VecStrain10& strains, const VecMaterialCoord& /*point*/  )
{
    Real stressDiagonal, stressOffDiagonal, shear, poissRatio,youngModulus;

    for( unsigned int i=0; i<stresses.size(); i++ )
    {
        if(poissonRatio.getValue().size()>i) poissRatio= poissonRatio.getValue()[i]; else poissRatio=0;
        if(stiffness.getValue().size()>i) youngModulus= stiffness.getValue()[i]; else youngModulus=1;

        Real f = youngModulus/((1 + poissRatio)*(1 - 2 * poissRatio));
        stressDiagonal = f * (1 - poissRatio);
        stressOffDiagonal = poissRatio * f;
        shear = f * (1 - 2 * poissRatio);

        for(unsigned int j=0; j<10; j++ )
        {

            stresses[i][j] = hookeStress( strains[i][j], stressDiagonal, stressOffDiagonal, shear );
        }
    }
}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::updateSampleMaterialProperties()
{
    if (!nbVoxels) return ;
    if (voronoi.size()!=nbVoxels) return ;

    unsigned int nbsamples=0;
    for(unsigned int i=0; i<nbVoxels; i++)
        if((unsigned int)voronoi[i]!=-1) if((unsigned int)voronoi[i]>nbsamples) nbsamples=(unsigned int)voronoi[i];
    nbsamples++; // indices in voronoi start from 0

    WriteAccessor<Data<vector<Real> > >  m_bulkModulus  ( bulkModulus );	m_bulkModulus.resize(nbsamples);
    WriteAccessor<Data<vector<Real> > >  m_stiffness  ( stiffness );		m_stiffness.resize(nbsamples);
    WriteAccessor<Data<vector<Real> > >  m_density  ( density );			m_density.resize(nbsamples);
    WriteAccessor<Data<vector<Real> > >  m_poissonRatio  ( poissonRatio );	m_poissonRatio.resize(nbsamples);

    for(unsigned int sampleindex=0; sampleindex<nbsamples; sampleindex++)
    {
        unsigned int count=0;
        m_bulkModulus[sampleindex]=0;
        m_stiffness[sampleindex]=0;
        m_density[sampleindex]=0;
        m_poissonRatio[sampleindex]=0;

        for(unsigned int i=0; i<nbVoxels; i++)
            if((unsigned int)voronoi[i]==sampleindex)
            {
                m_bulkModulus[sampleindex]+=getBulkModulus(grid.data()[i]);
                m_stiffness[sampleindex]+=getStiffness(grid.data()[i]);;
                m_density[sampleindex]+=getDensity(grid.data()[i]);;
                m_poissonRatio[sampleindex]+=getPoissonRatio(grid.data()[i]);
                count++;
            }
        if(count!=0)
        {
            m_bulkModulus[sampleindex]/=(Real)count;
            m_stiffness[sampleindex]/=(Real)count;
            m_density[sampleindex]/=(Real)count;
            m_poissonRatio[sampleindex]/=(Real)count;
        }
    }

}


template < class MaterialTypes>
typename GridMaterial< MaterialTypes>::Real GridMaterial<MaterialTypes>::getBulkModulus(const unsigned int sampleindex)
{
    if (bulkModulus.getValue().size()>sampleindex) return bulkModulus.getValue()[sampleindex]; else return 0;
}



template < class MaterialTypes>
typename GridMaterial< MaterialTypes>::Real GridMaterial<MaterialTypes>::getStiffness(const voxelType label)
{
    if(label==0) return (Real)1;

    const mapLabelType& pairs = labelToStiffnessPairs.getValue();
    if (pairs.size()==0) return (Real)1; // no map defined -> return 1

    typename mapLabelType::const_iterator mit;
    for (typename mapLabelType::const_iterator pit=pairs.begin(); pit!=pairs.end(); pit++)
    {
        if ((Real)pit->first>(Real)label)
        {
            if (pit==pairs.begin()) return pit->second;
            else
            {
                Real vlow=mit->second,vup=pit->second;
                Real alpha=(((Real)pit->first-(Real)label)/((Real)pit->first-(Real)mit->first));
                return alpha*vlow+(1.-alpha)*vup;
            }
        }
        mit=pit;
    }
    return mit->second;
}

template < class MaterialTypes>
typename GridMaterial<MaterialTypes>::Real GridMaterial<MaterialTypes>::getDensity(const voxelType label)
{
    if(label==0) return (Real)1;

    const mapLabelType& pairs = labelToDensityPairs.getValue();
    if (pairs.size()==0) return (Real)1; // no map defined -> return 1

    typename mapLabelType::const_iterator mit;
    for (typename mapLabelType::const_iterator pit=pairs.begin(); pit!=pairs.end(); pit++)
    {
        if ((Real)pit->first>(Real)label)
        {
            if (pit==pairs.begin()) return pit->second;
            else
            {
                Real vlow=mit->second,vup=pit->second;
                Real alpha=(((Real)pit->first-(Real)label)/((Real)pit->first-(Real)mit->first));
                return alpha*vlow+(1.-alpha)*vup;
            }
        }
        mit=pit;
    }
    return mit->second;
}

template < class MaterialTypes>
typename GridMaterial<MaterialTypes>::Real GridMaterial<MaterialTypes>::getBulkModulus(const voxelType label)
{
    if(label==0) return (Real)0;

    const mapLabelType& pairs = labelToBulkModulusPairs.getValue();
    if (pairs.size()==0) return (Real)0; // no map defined -> return 0

    typename mapLabelType::const_iterator mit;
    for (typename mapLabelType::const_iterator pit=pairs.begin(); pit!=pairs.end(); pit++)
    {
        if ((Real)pit->first>(Real)label)
        {
            if (pit==pairs.begin()) return pit->second;
            else
            {
                Real vlow=mit->second,vup=pit->second;
                Real alpha=(((Real)pit->first-(Real)label)/((Real)pit->first-(Real)mit->first));
                return alpha*vlow+(1.-alpha)*vup;
            }
        }
        mit=pit;
    }
    return mit->second;
}

template < class MaterialTypes>
typename GridMaterial<MaterialTypes>::Real GridMaterial<MaterialTypes>::getPoissonRatio(const voxelType label)
{
    if(label==0) return (Real)0;

    const mapLabelType& pairs = labelToPoissonRatioPairs.getValue();
    if (pairs.size()==0) return (Real)0; // no map defined -> return 0

    typename mapLabelType::const_iterator mit;
    for (typename mapLabelType::const_iterator pit=pairs.begin(); pit!=pairs.end(); pit++)
    {
        if ((Real)pit->first>(Real)label)
        {
            if (pit==pairs.begin()) return pit->second;
            else
            {
                Real vlow=mit->second,vup=pit->second;
                Real alpha=(((Real)pit->first-(Real)label)/((Real)pit->first-(Real)mit->first));
                return alpha*vlow+(1.-alpha)*vup;
            }
        }
        mit=pit;
    }
    return mit->second;
}

/*************************/
/*   IO               */
/*************************/

template < class MaterialTypes>
bool GridMaterial<MaterialTypes>::loadInfos()
{
    if (!infoFile.size()) return false;
    if (sofa::helper::system::DataRepository.findFile(infoFile)) // If the file is existing
    {
        infoFile=sofa::helper::system::DataRepository.getFile(infoFile);
        std::ifstream fileStream (infoFile.c_str(), std::ifstream::in);
        if (!fileStream.is_open())
        {
            serr << "Can not open " << infoFile << sendl;
            return false;
        }
        std::string str;
        fileStream >> str;	char vtype[32]; fileStream.getline(vtype,32); // voxeltype not used yet
        fileStream >> str; GCoord& dim = *this->dimension.beginEdit();       fileStream >> dim;      this->dimension.endEdit();
        fileStream >> str; SCoord& origin = *this->origin.beginEdit();       fileStream >> origin;   this->origin.endEdit();
        fileStream >> str; SCoord& voxelsize = *this->voxelSize.beginEdit(); fileStream >> voxelsize; this->voxelSize.endEdit();
        fileStream.close();
        std::cout << "Loaded info file "<< infoFile << std::endl;
    }
    return true;
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::saveInfos()
{
    if (!infoFile.size()) return false;
    std::ofstream fileStream (infoFile.c_str(), std::ofstream::out);
    if (!fileStream.is_open())
    {
        serr << "Can not open " << infoFile << sendl;
        return false;
    }
    std::cout << "Writing info file " << infoFile << std::endl;
    fileStream << "voxelType: " << CImg<voxelType>::pixel_type() << std::endl;
    fileStream << "dimensions: " << dimension.getValue() << std::endl;
    fileStream << "origin: " << origin.getValue() << std::endl;
    fileStream << "voxelSize: " << voxelSize.getValue() << std::endl;
    fileStream.close();
    return true;
}


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::loadImage()
{
    if (!imageFile.isSet()) return false;

    std::string fName (imageFile.getValue());
    if( strcmp( fName.substr(fName.find_last_of('.')+1).c_str(), "raw") == 0 ||
        strcmp( fName.substr(fName.find_last_of('.')+1).c_str(), "RAW") == 0)
    {
        grid.load_raw(imageFile.getFullPath().c_str(),dimension.getValue()[0],dimension.getValue()[1],dimension.getValue()[2]);
    }
    else
    {
        grid.load(imageFile.getFullPath().c_str());

        // Convert image to black and white to avoid access problems
        cimg_forXY(grid,x,y)
        {
            grid(x,y)=(grid(x,y,0)+grid(x,y,1)+grid(x,y,2))/3.0;
        }

        // Extrud Z dimension of the image to the wanted size.
        if (dimension.getValue()[2] > 1)
            grid.resize(grid.width(), grid.height(), dimension.getValue()[2], grid.spectrum(), 0, 1); // no interpolation, extrude values
    }

    // offset by one voxel to prevent from interpolation outside the grid
    int offset=5;
    GCoord off; off.fill(2*offset);
    dimension.setValue(dimension.getValue()+off);
    origin.setValue(origin.getValue()-voxelSize.getValue()*(Real)offset);

    grid.resize(dimension.getValue()[0],dimension.getValue()[1],dimension.getValue()[2],1,0,0,0.5,0.5,0.5,0.5);

    if (grid.size()==0)
    {
        serr << "Can not open " << imageFile << sendl;
        return false;
    }
    std::cout << "Loaded image "<< imageFile <<" of voxel type " << grid.pixel_type() << std::endl;
    this->nbVoxels = dimension.getValue()[0]*dimension.getValue()[1]*dimension.getValue()[2];
    return true;
}


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::saveImage()
{
    if (!imageFile.isSet()) return false;
    if (nbVoxels==0) return false;
    grid.save_raw(imageFile.getFullPath().c_str());
    return true;
}


template < class MaterialTypes>
bool GridMaterial<MaterialTypes>::loadWeightRepartion()
{
    if (!weightFile.isSet()) return false;
    if (nbVoxels==0) return false;

    std::ifstream fileStream (weightFile.getFullPath().c_str(), std::ifstream::in);
    if (!fileStream.is_open())
    {
        serr << "Can not open " << weightFile << sendl;
        return false;
    }
    unsigned int nbrefs,nbvox;
    fileStream >> nbvox;
    fileStream >> nbrefs;
    if (nbVoxels!=nbvox)
    {
        serr << "Invalid grid size in " << weightFile << sendl;
        return false;
    }
    if (nbRef!=nbrefs)
    {
        serr << "Invalid nbRef in " << weightFile << sendl;
        return false;
    }

    this->v_index.resize(nbVoxels);
    this->v_weights.resize(nbVoxels);

    for (unsigned int i=0; i<nbVoxels; i++)
        for (unsigned int j=0; j<nbRef; j++)
        {
            fileStream >> v_index[i][j] ;
            fileStream >> v_weights[i][j];
        }
    fileStream.close();
    std::cout << "Loaded weight file "<< weightFile << std::endl;
    showedrepartition=-1;
    return true;
}



template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::saveWeightRepartion()
{
    if (!weightFile.isSet()) return false;
    if (nbVoxels==0) return false;
    if (v_weights.size()!=nbVoxels) return false;
    if (v_index.size()!=nbVoxels) return false;

    std::ofstream fileStream (weightFile.getFullPath().c_str(), std::ofstream::out);
    if (!fileStream.is_open())
    {
        serr << "Can not open " << weightFile << sendl;
        return false;
    }
    std::cout << "Writing grid weights repartion file " << weightFile << std::endl;

    fileStream << nbVoxels << " " << nbRef << std::endl;
    for (unsigned int i=0; i<nbVoxels; i++)
    {
        for (unsigned int j=0; j<nbRef; j++)
        {
            fileStream << v_index[i][j] << " " << v_weights[i][j] << " ";
        }
        fileStream << std::endl;
    }
    fileStream.close();
    return true;
}




/*************************/
/*   Lumping        */
/*************************/


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::getWeightedMasses(const unsigned int sampleindex, vector<VRef>& reps, vector<VRefReal>& w, VecSCoord& p,vector<Real>& masses)
{
    p.clear();
    masses.clear();
    reps.clear();
    w.clear();

    if (!nbVoxels) return false;
    if (voronoi.size()!=nbVoxels || v_weights.size()!=nbVoxels || v_index.size()!=nbVoxels) return false; // weights not computed

    Real voxelvolume=voxelSize.getValue()[0]*voxelSize.getValue()[1]*voxelSize.getValue()[2];

    unsigned int i;
    for(i=0; i<nbVoxels; i++) if(voronoi[i]==(int)sampleindex)
        {
            p.push_back(SCoord());  getCoord(i,p.back());
            w.push_back(v_weights[i]);
            reps.push_back(v_index[i]);
            masses.push_back(voxelvolume*getDensity(grid.data()[i]));
        }
    return true;
}


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::lumpMass(const unsigned int sampleindex,Real& mass)
{
    mass=0;
    if (!nbVoxels) return false;
    if (voronoi.size()!=nbVoxels) return false;
    Real voxelvolume=voxelSize.getValue()[0]*voxelSize.getValue()[1]*voxelSize.getValue()[2];
    for (unsigned int i=0; i<nbVoxels; i++) if (voronoi[i]==(int)sampleindex) mass+=voxelvolume*getDensity(grid.data()[i]);
    return true;
}


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::lumpVolume(const unsigned int sampleindex,Real& vol)
{
    vol=0;
    if (!nbVoxels) return false;
    if (voronoi.size()!=nbVoxels) return false;
    Real voxelvolume=voxelSize.getValue()[0]*voxelSize.getValue()[1]*voxelSize.getValue()[2];
    for (unsigned int i=0; i<nbVoxels; i++) if (voronoi[i]==(int)sampleindex) vol+=voxelvolume;
    return true;
}


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeVolumeIntegrationFactors(const unsigned int sampleindex,const SCoord& point,const unsigned int order,vector<Real>& moments)
{
    unsigned int i,j,dim=(order+1)*(order+2)*(order+3)/6; // complete basis of order 'order'
    moments.resize(dim);	for (i=0; i<dim; i++) moments[i]=0;

    if (!nbVoxels) return false;
    if (voronoi.size()!=nbVoxels) return false;
    Real voxelvolume=voxelSize.getValue()[0]*voxelSize.getValue()[1]*voxelSize.getValue()[2];

    SCoord G;
    vector<Real> momentPG;
    for (i=0; i<nbVoxels; i++)
        if (voronoi[i]==(int)sampleindex)
        {
            getCoord(i,G);
            getCompleteBasis(G-point,order,momentPG);
            for (j=0; j<dim; j++) moments[j]+=momentPG[j]*voxelvolume;
            //for (j=0;j<dim;j++) moments[j]+=momentPG[j]*voxelvolume*getStiffness(grid.data()[i]); // for linear materials stiffness can be integrated into the precomputed factors -> need to update compute stress in this case // not use here to be able to modify stiffness through the gui
        }
    return true;
}




template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::lumpWeightsRepartition(const unsigned int sampleindex,const SCoord& point,VRef& reps,VRefReal& w,VRefGradient* dw,VRefHessian* ddw)
{
    if (!nbVoxels) return false;
    if (voronoi.size()!=nbVoxels || v_weights.size()!=nbVoxels || v_index.size()!=nbVoxels) return false; // weights not computed

    unsigned int i,j,k;
    for (i=0; i<nbRef; i++) {reps[i]=0; w[i]=0;}

    // get the nbrefs most relevant weights in the voronoi region
    unsigned int maxlabel=0;
    for (i=0; i<nbVoxels; i++) if(voronoi[i]==(int)sampleindex) for (j=0; j<nbRef; j++) { if(v_weights[i][j]!=0) if(v_index[i][j]>maxlabel) maxlabel=v_index[i][j]; }
    vector<Real> W((int)(maxlabel+1),0);
    for (i=0; i<nbVoxels; i++) if(voronoi[i]==(int)sampleindex) for (j=0; j<nbRef; j++) if(v_weights[i][j]!=0) W[v_index[i][j]]+=v_weights[i][j];

    for (i=0; i<maxlabel+1; i++)
    {
        j=0; while (j!=nbRef && w[j]>W[i]) j++;
        if(j!=nbRef)
        {
            for (k=nbRef-1; k>j; k--) {w[k]=w[k-1]; reps[k]=reps[k-1];}
            w[j]=W[i];
            reps[j]=i;
        }
    }

    // get point indices in voronoi
    VUI neighbors;   for (i=0; i<nbVoxels; i++) if (voronoi[i]==(int)sampleindex) neighbors.push_back((unsigned int)i);
    bool dilatevoronoi=false;
    if (dilatevoronoi)
        for (i=0; i<nbVoxels; i++)
        {
            VUI tmp;
            get26Neighbors(i, tmp);
            bool insert=false;
            for (j=0; j<tmp.size(); j++) if (voronoi[tmp[j]]==(int)sampleindex) insert=true;
            if (insert) neighbors.push_back((unsigned int)i);
        }

    // lump the weights
    for (i=0; i<nbRef; i++)
        if(w[i]!=0)
        {
            pasteRepartioninWeight(reps[i]);
            if(!dw) lumpWeights(neighbors,point,w[i]);
            else /*if(!ddw)*/ lumpWeights(neighbors,point,w[i],&(*dw)[i]);
            //else lumpWeights(neighbors,point,w[i],&(*dw)[i],&(*ddw)[i]);  // desctivated for speed... (weights are supposed to be linear and not quadratic)
        }

    return true;
}




template<class real>
inline real determ(vector<vector<real> >& m, int S)
{
    if (S < 2) {return 0;}
    else if (S == 2) { return m[0][0] * m[1][1] - m[1][0] * m[0][1]; }
    else
    {
        int i,j,j1,j2;
        real det = 0;
        vector<vector<real> > m1(S-1,vector<real>(S-1));
        for (j1=0; j1<S; j1++)
        {
            for (i=1; i<S; i++)
            {
                j2 = 0;
                for (j=0; j<S; j++)
                {
                    if (j == j1) continue;
                    m1[i-1][j2] = m[i][j];
                    j2++;
                }
            }
            det += pow(-1.0,1.0+j1+1.0) * m[0][j1] * determ(m1,S-1);
        }
        return(det);
    }
}




template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::lumpWeights(const VUI& indices,const SCoord& point,Real& w,SGradient* dw,SHessian* ddw,Real* err)
{
    if (!nbVoxels) return false;
    if (weights.size()!=nbVoxels) return false; // point not in grid or no weight computed

    //sout<<"fit on "<<indices.size()<<" voxels"<<sendl;

    unsigned int i,j,k;
    Real MIN_DET=1E-2;

    // least squares fit
    unsigned int order=0;
    if (ddw && dw) order=2;
    else if (dw) order=1;
    unsigned int dim=(order+1)*(order+2)*(order+3)/6;
    vector<vector<Real> > Cov((int)dim,vector<Real>((int)dim,(Real)0)); // accumulate dp^(order)dp^(order)T
    SCoord pi;
    for (j=0; j<indices.size(); j++)
    {
        getCoord(indices[j],pi);
        //sout<<"dp["<<j<<"]="<<pi-point<<",w="<<weights[indices[j]]<<sendl;
        accumulateCovariance(pi-point,order,Cov);
    }

    //sout<<"Cov="<<Cov<<sendl;

    // invert covariance matrix
    vector<vector<Real> > invCov((int)dim,vector<Real>((int)dim,(Real)0));
    if (order==0) invCov[0][0]=1./Cov[0][0];    // simple average
    else if (order==1)
    {
        Mat<4,4,Real> tmp,invtmp;
        for (i=0; i<4; i++) for (j=0; j<4; j++) tmp[i][j]=Cov[i][j];

        if (determ(Cov,dim)<MIN_DET) invCov[0][0]=1./Cov[0][0];    // coplanar points->not invertible->go back to simple average
        else	{ invtmp.invert(tmp); for (i=0; i<4; i++) for (j=0; j<4; j++) invCov[i][j]=invtmp[i][j];}
    }
    else if (order==2)
    {
        Mat<10,10,Real> tmp,invtmp;
        for (i=0; i<10; i++) for (j=0; j<10; j++) tmp[i][j]=Cov[i][j];
        if (determ(Cov,dim)<MIN_DET) // try order 1
        {
            ddw->fill(0);
            return lumpWeights(indices,point,w,dw,NULL,err);
        }
        else { invtmp.invert(tmp); for (i=0; i<10; i++) for (j=0; j<10; j++) invCov[i][j]=invtmp[i][j];}
    }

    // compute weights and its derivatives
    vector<Real> basis,wdp((int)dim,(Real)0);
    for (j=0; j<indices.size(); j++)
    {
        getCoord(indices[j],pi);
        getCompleteBasis(pi-point,order,basis);
        for (i=0; i<dim; i++) wdp[i]+=basis[i]*weights[indices[j]];
    }

    vector<Real> W((int)dim,(Real)0);
    for (i=0; i<dim; i++) for (j=0; j<dim; j++) W[i]+=invCov[i][j]*wdp[j];


//
//sout<<"wdp="; for (i=0;i<dim;i++) sout<<wdp[i]<<","; sout<<sendl;
//vector<Real> sdp((int)dim,(Real)0);
//for (j=0;j<indices.size();j++) { getCoord(indices[j],pi); getCompleteBasis(pi-point,order,basis); for (i=0;i<dim;i++) sdp[i]+=basis[i]; }
//sout<<"sdp="; for (i=0;i<dim;i++) sout<<sdp[i]<<","; sout<<sendl;
//sout<<"W="; for (i=0;i<dim;i++) sout<<W[i]<<","; sout<<sendl;
//sout<<"invCov="; for (i=0;i<dim;i++) for (j=0;j<dim;j++) sout<<invCov[i][j]<<","; sout<<sendl;
    if(err)
    {
        for (j=0; j<indices.size(); j++)
        {
            getCoord(indices[j],pi);
            getCompleteBasis(pi-point,order,basis);
            Real er=0; for (k=0; k<dim; k++) er+=basis[k]*W[k];
            er=weights[indices[j]]-er; if(er<0) er*=-1;
            *err+=er;
        }
    }

    w=W[0];
    if (order==0) return true;

    vector<SGradient> basisderiv;
    getCompleteBasisDeriv(SCoord(0,0,0),order,basisderiv);
    for (i=0; i<3; i++)
    {
        (*dw)[i]=0;
        for (j=0; j<dim; j++) (*dw)[i]+=W[j]*basisderiv[j][i];
    }
    if (order==1) return true;

    vector<SHessian> basisderiv2;
    getCompleteBasisDeriv2(SCoord(0,0,0),order,basisderiv2);
    for (i=0; i<3; i++) for (k=0; k<3; k++)
        {
            (*ddw)[i][k]=0;
            for (j=0; j<dim; j++) (*ddw)[i][k]+=W[j]*basisderiv2[j][i][k];
        }
    return true;
}

template < class MaterialTypes>
void GridMaterial< MaterialTypes>::accumulateCovariance(const SCoord& p,const unsigned int order,vector<vector<Real> >& Cov)
{
    vector<Real> basis;
    getCompleteBasis(p,order,basis);
    unsigned int dim=(order+1)*(order+2)*(order+3)/6;
    for (unsigned int i=0; i<dim; i++) for (unsigned int j=0; j<dim; j++) Cov[i][j]+=basis[i]*basis[j];
}




template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::interpolateWeightsRepartition(const SCoord& point,VRef& reps,VRefReal& w)
{
    if (!nbVoxels) return false;

    int index=getIndex(point);
    if (index==-1) std::cout<<"problem with point:"<<point<<" (out of voxels)"<<std::endl;

    if (index==-1) return false; // point not in grid
    if (v_weights.size()!=nbVoxels || v_index.size()!=nbVoxels) return false; // weights not computed

    for (unsigned int i=0; i<nbRef; i++)
        if(v_weights[index][i]!=0)
        {
            reps[i]=v_index[index][i];
            pasteRepartioninWeight(v_index[index][i]);
            interpolateWeights(point,w[i]);
        }
        else reps[i]=w[i]=0;

    if (w[0]==0) std::cout<<"problem with point:"<<point<<" data:"<<(int)grid.data()[index]<<" rep:"<<v_index[index]<<" wghts:"<<v_weights[index]<<std::endl;

    return true;
}



template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::interpolateWeights(const SCoord& point,Real& w)
{
    if (!nbVoxels) return false;
    if(weights.size()!=nbVoxels) {w=0; return false; }  //no weight computed

    // get weights of the underlying voxel
    int index=getIndex(point);
    if (index==-1 || weights.size()!=nbVoxels) {w=0; return false; } // point not in grid

    //w=weights[index]; return true; // temporary: no interpolation

    VUI ptlist; get26Neighbors(index,ptlist);
    SGradient dwvox;   SHessian ddwvox;
    lumpWeights(ptlist,point,w,&dwvox,NULL);
    if(w<0) w=0; else if(w>1) w=1;
    return true;
}



/*********************************/
/*   Compute weights/distances   */
/*********************************/


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeWeights(const VecSCoord& points)
{
    // TO DO: add params as data? : GEODESIC factor, DIFFUSION fixdatavalue, DIFFUSION max_iterations, DIFFUSION precision
    if (!nbVoxels) return false;
    unsigned int i,dtype=this->distanceType.getValue().getSelectedId(),nbp=points.size();

    // init
    this->v_index.resize(nbVoxels);
    this->v_weights.resize(nbVoxels);
    for (i=0; i<nbVoxels; i++)
    {
        this->v_weights[i].fill(0);
    }

    if (dtype==DISTANCE_GEODESIC)
    {
        computeGeodesicalDistances (points); // voronoi
        for (i=0; i<nbp; i++)
        {
            computeAnisotropicLinearWeightsInVoronoi(points[i]);
            offsetWeightsOutsideObject();
            addWeightinRepartion(i);
        }
    }
    else if (dtype==DISTANCE_DIFFUSION || dtype==DISTANCE_ANISOTROPICDIFFUSION)
    {
        for (i=0; i<nbp; i++)
        {
            HeatDiffusion(points,i);
            offsetWeightsOutsideObject();
            addWeightinRepartion(i);
        }
    }


    normalizeWeightRepartion();

    std::cout<<"Grid weight computation completed"<<std::endl;
    if (weightFile.isSet()) saveWeightRepartion();
    showedrepartition=-1;
    return true;
}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::addWeightinRepartion(const unsigned int index)
{
    if (!nbVoxels) return;
    unsigned int j,k;

    for (unsigned int i=0; i<nbVoxels; i++)
        //if (grid.data()[i])
        if(weights[i])
        {
            j=0;
            while (j!=nbRef && v_weights[i][j]>weights[i]) j++;
            if (j!=nbRef) // insert weight and index in the ordered list
            {
                for (k=nbRef-1; k>j; k--)
                {
                    v_weights[i][k]=v_weights[i][k-1];
                    v_index[i][k]=v_index[i][k-1];
                }
                v_weights[i][j]=weights[i];
                v_index[i][j]=index;
            }
        }
    showedrepartition=-1;
}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::pasteRepartioninWeight(const unsigned int index)
{
    if (!nbVoxels) return;
    if (v_index.size()!=nbVoxels) return;
    if (v_weights.size()!=nbVoxels) return;
    weights.resize(this->nbVoxels);
    unsigned int i;
    for (i=0; i<nbVoxels; i++) weights[i]=findWeightInRepartition(i,index);
    showedrepartition=-1;
}

template < class MaterialTypes>
void GridMaterial< MaterialTypes>::normalizeWeightRepartion()
{
    if (!nbVoxels) return;
    unsigned int j;
    for (unsigned int i=0; i<nbVoxels; i++)
        //if (grid.data()[i])
        if(v_weights[i][0])
        {
            Real W=0;
            for (j=0; j<nbRef && v_weights[i][j]!=0; j++) W+=v_weights[i][j];
            if (W!=0) for (j=0; j<nbRef  && v_weights[i][j]!=0; j++) v_weights[i][j]/=W;
        }
    showedrepartition=-1;
}


template < class MaterialTypes>
typename GridMaterial< MaterialTypes>::Real GridMaterial< MaterialTypes>::getDistance(const unsigned int& index1,const unsigned int& index2)
{
    if (!nbVoxels) return -1;
    SCoord coord1;
    if (!getCoord(index1,coord1)) return -1; // point1 not in grid
    SCoord coord2;
    if (!getCoord(index2,coord2)) return -1; // point2 not in grid

    if (this->biasDistances.getValue()) // bias distances according to stiffness
    {
        Real meanstiff=(getStiffness(grid.data()[index1])+getStiffness(grid.data()[index2]))/2.;
        return ((Real)(coord2-coord1).norm()/meanstiff);
    }
    else return (Real)(coord2-coord1).norm();
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeGeodesicalDistances ( const SCoord& point, const Real distMax )
{
    if (!nbVoxels) return false;
    int index=getIndex(point);
    return computeGeodesicalDistances (index, distMax );
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeGeodesicalDistances ( const int& index, const Real distMax )
{
    if (!nbVoxels) return false;
    unsigned int i,index1,index2;
    distances.resize(this->nbVoxels);
    for (i=0; i<this->nbVoxels; i++) distances[i]=distMax;

    if (index<0 || index>=(int)nbVoxels) return false; // voxel out of grid
    if (!grid.data()[index]) return false;  // voxel out of object

    VUI neighbors;
    Real d;
    std::queue<int> fifo;
    distances[index]=0;
    fifo.push(index);
    while (!fifo.empty())
    {
        index1=fifo.front();
        get26Neighbors(index1, neighbors);
        for (i=0; i<neighbors.size(); i++)
        {
            index2=neighbors[i];
            if (grid.data()[index2]) // test if voxel is not void
            {
                d=distances[index1]+getDistance(index1,index2);
                if (distances[index2]>d)
                {
                    distances[index2]=d;
                    fifo.push(index2);
                }
            }
        }
        fifo.pop();
    }
    return true;
}


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeGeodesicalDistances ( const VecSCoord& points, const Real distMax )
{
    if (!nbVoxels) return false;
    vector<int> indices;
    for (unsigned int i=0; i<points.size(); i++) indices.push_back(getIndex(points[i]));
    return computeGeodesicalDistances ( indices, distMax );
}


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeGeodesicalDistances ( const vector<int>& indices, const Real distMax )
{
    if (!nbVoxels) return false;
    unsigned int i,nbi=indices.size(),index1,index2;
    distances.resize(this->nbVoxels);
    voronoi.resize(this->nbVoxels);
    for (i=0; i<this->nbVoxels; i++)
    {
        distances[i]=distMax;
        voronoi[i]=-1;
    }

    VUI neighbors;
    Real d;

    std::queue<unsigned int> fifo;
    for (i=0; i<nbi; i++) if (indices[i]>=0 && indices[i]<(int)nbVoxels) if (grid.data()[indices[i]]!=0)
            {
                distances[indices[i]]=0;
                voronoi[indices[i]]=i;
                fifo.push(indices[i]);
            }
    if (fifo.empty()) return false; // all input voxels out of grid
    while (!fifo.empty())
    {
        index1=fifo.front();
        get26Neighbors(index1, neighbors);
        for (i=0; i<neighbors.size(); i++)
        {
            index2=neighbors[i];
            if (grid.data()[index2]) // test if voxel is not void
            {
                d=distances[index1]+getDistance(index1,index2);
                if (distances[index2]>d)
                {
                    distances[index2]=d;
                    voronoi[index2]=voronoi[index1];
                    fifo.push(index2);
                }
            }
        }
        fifo.pop();
    }
    return true;
}



template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeGeodesicalDistancesToVoronoi ( const SCoord& point, const Real distMax )
{
    if (!nbVoxels) return false;
    if (voronoi.size()!=nbVoxels) return false;
    int index=getIndex(point);
    if (voronoi[index]==-1) return false;

    return computeGeodesicalDistancesToVoronoi (index, distMax );
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeGeodesicalDistancesToVoronoi ( const int& index, const Real distMax )
{
    if (!nbVoxels) return false;
    if (voronoi.size()!=nbVoxels) return false;

    unsigned int i,j,index1,index2;
    distances.resize(this->nbVoxels);
    for (i=0; i<this->nbVoxels; i++) distances[i]=distMax;
    if (index<0 || index>=(int)nbVoxels) return false; // voxel out of grid
    if (!grid.data()[index]) return false;  // voxel out of object
    if (voronoi[index]==-1) return false;  // no voronoi defined

    VUI neighbors;
    Real d;
    std::queue<int> fifo;

    for (i=0; i<nbVoxels; i++)
        if(voronoi[i]==voronoi[index])
        {
            get6Neighbors((int)i, neighbors);
            for (j=0; j<neighbors.size(); j++)
                if(voronoi[neighbors[j]]!=voronoi[index] || grid.data()[neighbors[j]]) // voronoi frontier
                {
                    distances[i]=0; fifo.push((int)i);
                    j=neighbors.size();
                }
        }

    while (!fifo.empty())
    {
        index1=fifo.front();
        get26Neighbors(index1, neighbors);
        for (i=0; i<neighbors.size(); i++)
        {
            index2=neighbors[i];
            if (grid.data()[index2]) // test if voxel is not void
            {
                d=distances[index1]+getDistance(index1,index2);
                if (distances[index2]>d)
                {
                    distances[index2]=d;
                    fifo.push(index2);
                }
            }
        }
        fifo.pop();
    }

    return true;
}


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeLinearRegionsSampling ( VecSCoord& points, const unsigned int num_points)
{
    if (!nbVoxels) return false;

    unsigned int i,j,initial_num_points=points.size(),nbnonemptyvoxels=0;
    int k;

    // identify regions with similar repartitions and similar stiffness
    voronoi.resize(this->nbVoxels);
    for (i=0; i<this->nbVoxels; i++) voronoi[i]=-1;

    vector<unsigned int> indices;
    vector<Real> stiffnesses;

    // insert initial points
    for (i=0; i<initial_num_points; i++)
    {
        j=getIndex(points[i]);
        if(grid.data()[j])
        {
            voronoi[j]=indices.size();
            indices.push_back(j);
            stiffnesses.push_back(getStiffness(grid.data()[j]));
        }
    }


    // visit all voxels and insert new reps/stiffness if necessary
    for (i=0; i<this->nbVoxels; i++)
        if(grid.data()[i])
        {
            nbnonemptyvoxels++;
            Real stiff=getStiffness(grid.data()[i]);
            k=-1;
            for (j=0; j<(unsigned int)indices.size() && k==-1; j++) // detect similar already inserted repartitions and stiffness
                if(stiffnesses[j]==stiff) if(areRepsSimilar(i,indices[j])) k=j;

            if(k==-1)   // insert
            {
                voronoi[i]=indices.size();
                indices.push_back(i);
                stiffnesses.push_back(stiff);
            }
            else voronoi[i]=k;
        }


    // check linearity in each region and subdivide region of highest error until num_points is reached
    vector<Real> errors(indices.size(),(Real)0.);
    Real w; SGradient dw; SHessian ddw; VUI ptlist;  SCoord point;
    for (j=0; j<indices.size(); j++)
    {
        getCoord(indices[j],point);
        ptlist.clear();  for (i=0; i<nbVoxels; i++) if (voronoi[i]==(int)j) ptlist.push_back(i);
        for (i=0; i<nbRef; i++)
            if(v_weights[indices[j]][i]!=0)
            {
                pasteRepartioninWeight(v_index[indices[j]][i]);
                lumpWeights(ptlist,point,w,&dw,NULL,&errors[j]);
            }
    }

    while(indices.size()<num_points)
    {
        Real maxerr=0;
        for (j=0; j<indices.size(); j++) if(errors[j]>maxerr) {maxerr=errors[j]; i=j;}
        SubdivideVoronoiRegion(i,indices.size());
        j=0; while((unsigned int)voronoi[j]!=indices.size() && j<(unsigned int)nbVoxels) j++;
        if(j==nbVoxels) {errors[j]=0; continue;} //unable to add region
        else
        {
            indices.push_back(j); errors.push_back(0);
            // update errors
            errors[i]=0;
            getCoord(indices[i],point);
            ptlist.clear();  for (j=0; j<(unsigned int)nbVoxels; j++) if ((unsigned int)voronoi[j]==i) ptlist.push_back(j);
            for (j=0; j<nbRef; j++)
                if(v_weights[indices[i]][j]!=0)
                {
                    pasteRepartioninWeight(v_index[indices[i]][j]);
                    lumpWeights(ptlist,point,w,&dw,NULL,&errors[i]);
                }
            i=indices.size()-1;
            errors[i]=0;
            getCoord(indices[i],point);
            ptlist.clear();  for (j=0; j<(unsigned int)nbVoxels; j++) if ((unsigned int)voronoi[j]==i) ptlist.push_back(j);
            for (j=0; j<nbRef; j++)
                if(v_weights[indices[i]][j]!=0)
                {
                    pasteRepartioninWeight(v_index[indices[i]][j]);
                    lumpWeights(ptlist,point,w,&dw,NULL,&errors[i]);
                }
        }
    }
    Real err=0; for (j=0; j<errors.size(); j++) err+=errors[j];
    err/=(Real)nbnonemptyvoxels;
    std::cout<<"Average error in weights per voxel="<<err<<std::endl;

    // insert gauss points in the center of voronoi regions
    points.resize(indices.size());
    for (j=initial_num_points; j<indices.size(); j++)
    {
        points[j].fill(0);
        SCoord p; unsigned int count=0;
        for (i=0; i<this->nbVoxels; i++) if(voronoi[i]==(int)j) {getCoord(i,p); points[j]+=p; count++; }
        if(count!=0)
        {
            points[j]/=(Real)count;
        }
    }


    //std::cout<<"Added " << indices.size()-initial_num_points << " samples"<<std::endl;
    return true;
}




template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::SubdivideVoronoiRegion( const unsigned int voronoiindex, const unsigned int newvoronoiindex, const unsigned int max_iterations )
{
    if (!nbVoxels) return false;
    if (voronoi.size()!=nbVoxels) return false;

    unsigned int i,i1,i2=0;
    SCoord p1,p2;

    // initialization: take the first point and its farthest point inside the voronoi region
    i1=0; while((unsigned int)voronoi[i1]!=voronoiindex && i1<nbVoxels) i1++;
    if(i1==nbVoxels) return false;
    getCoord(i1,p1);

    Real d,dmax=0;
    for (i=0; i<nbVoxels; i++)
        if((unsigned int)voronoi[i]==voronoiindex)
        {
            getCoord(i,p2); d=(p2-p1).norm2();
            if(d>dmax) {i2=i; dmax=d;}
        }
    if(dmax==0) return false;
    getCoord(i2,p2);

    // Lloyd relaxation
    Real d1,d2;
    SCoord p,cp1,cp2;
    unsigned int nb1,nb2,nbiterations=0;
    while (nbiterations<max_iterations)
    {
        // mode points to the centroid of their voronoi regions
        nb1=nb2=0;
        cp1.fill(0); cp2.fill(0);
        for (i=0; i<nbVoxels; i++)
            if((unsigned int)voronoi[i]==voronoiindex)
            {
                getCoord(i,p);
                d1=(p1-p).norm2(); d2=(p2-p).norm2();
                if(d1<d2) { cp1+=p; nb1++; }
                else { cp2+=p; nb2++; }
            }
        if(nb1!=0) cp1/=(Real)nb1;	nb1=getIndex(cp1);
        if(nb2!=0) cp2/=(Real)nb2;	nb2=getIndex(cp2);
        if(nb1==i1 && nb2==i2) nbiterations=max_iterations;
        else {i1=nb1; getCoord(i1,p1); i2=nb2; getCoord(i2,p2); nbiterations++; }
    }

    // fill one region with new index
    for (i=0; i<nbVoxels; i++)
        if((unsigned int)voronoi[i]==voronoiindex)
        {
            getCoord(i,p);
            d1=(p1-p).norm2(); d2=(p2-p).norm2();
            if(d1<d2) voronoi[i]=newvoronoiindex;
        }

    return true;
}




template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeRegularSampling ( VecSCoord& points, const unsigned int step)
{
    if (!nbVoxels) return false;
    if(step==0)  return false;

    unsigned int i,initial_num_points=points.size();
    vector<int> indices;
    for (i=0; i<initial_num_points; i++) indices.push_back(getIndex(points[i]));

    for(unsigned int z=0; z<(unsigned int)dimension.getValue()[2]; z+=step)
        for(unsigned int y=0; y<(unsigned int)dimension.getValue()[1]; y+=step)
            for(unsigned int x=0; x<(unsigned int)dimension.getValue()[0]; x+=step)
                if (grid(x,y,z)!=0)
                    indices.push_back(getIndex(GCoord(x,y,z)));

    computeGeodesicalDistances(indices); // voronoi

    // get points from indices
    points.resize(indices.size());
    for (i=initial_num_points; i<indices.size(); i++)     getCoord(indices[i],points[i]) ;

    //std::cout<<"Added " << indices.size()-initial_num_points << " regularly sampled points"<<std::endl;
    return true;
}


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeUniformSampling ( VecSCoord& points, const unsigned int num_points, const unsigned int max_iterations )
{
    if (!nbVoxels) return false;
    unsigned int i,k,initial_num_points=points.size(),nb_points=num_points;
    if(initial_num_points>num_points) nb_points=initial_num_points;

    vector<int> indices((int)nb_points,-1);
    for (i=0; i<initial_num_points; i++) indices[i]=getIndex(points[i]);
    points.resize(nb_points);

    // initialization: farthest point sampling (see [adams08])
    Real dmax;
    int indexmax;
    if (initial_num_points==0)
    {
        indices[0]=0;    // take the first not empty voxel as a random point
        while (grid.data()[indices[0]]==0)
        {
            indices[0]++;
            if (indices[0]==(int)nbVoxels) return false;
        }
    }
    else if(initial_num_points==nb_points) computeGeodesicalDistances(indices);

    for (i=initial_num_points; i<nb_points; i++)
    {
        if (i==0) i=1; // a random point has been inserted
        // get farthest point from all inserted points
        computeGeodesicalDistances(indices);
        dmax=-1;
        indexmax=-1;
        for (k=0; k<nbVoxels; k++)  if (grid.data()[k])
            {
                if (distances[k]>dmax && voronoi[k]!=-1)
                {
                    dmax=distances[k];
                    indexmax=k;
                }
            }
        if (indexmax==-1)
        {
            return false;    // unable to add point
        }
        indices[i]=indexmax;
    }
    // Lloyd relaxation
    SCoord pos,u,pos_point,pos_voxel;
    unsigned int count,nbiterations=0;
    bool ok=false,ok2;
    Real d,dmin;
    int indexmin;

    while (!ok && nbiterations<max_iterations)
    {
        ok2=true;
        computeGeodesicalDistances(indices); // Voronoi
        vector<bool> flag((int)nbVoxels,false);
        for (i=initial_num_points; i<nb_points; i++) // move to centroid of Voronoi cells
        {
            // estimate centroid given the measured distances = p + 1/N sum d(p,pi)*(pi-p)/|pi-p|
            getCoord(indices[i],pos_point);
            pos.fill(0);
            count=0;
            for (k=0; k<nbVoxels; k++)
                if (voronoi[k]==(int)i)
                {
                    getCoord(k,pos_voxel);
                    u=pos_voxel-pos_point;
                    u.normalize();
                    pos+=u*(Real)distances[k];
                    count++;
                }
            pos/=(Real)count;
            pos+=pos_point;
            // get closest unoccupied point in object
            dmin=std::numeric_limits<Real>::max();
            indexmin=-1;
            for (k=0; k<nbVoxels; k++) if (!flag[k]) if (grid.data()[k]!=0)
                    {
                        getCoord(k,pos_voxel);
                        d=(pos-pos_voxel).norm2();
                        if (d<dmin)
                        {
                            dmin=d;
                            indexmin=k;
                        }
                    }
            flag[indexmin]=true;
            if (indices[i]!=indexmin)
            {
                ok2=false;
                indices[i]=indexmin;
            }
        }
        ok=ok2;
        nbiterations++;
    }

    // get points from indices
    for (i=initial_num_points; i<nb_points; i++)
    {
        getCoord(indices[i],points[i]) ;
    }

    if (nbiterations==max_iterations)
    {
        serr<<"Lloyd relaxation has not converged in "<<nbiterations<<" iterations"<<sendl;
        return false;
    }
    else std::cout<<"Lloyd relaxation completed in "<<nbiterations<<" iterations"<<std::endl;
    return true;
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeAnisotropicLinearWeightsInVoronoi ( const SCoord& point, const Real factor)
/// linearly decreasing weight with support=factor*dist(point,closestVoronoiBorder) -> weight= 1-d/(factor*(d+-disttovoronoi))
{
    unsigned int i;
    weights.resize(this->nbVoxels);
    for (i=0; i<this->nbVoxels; i++)  weights[i]=0;
    if (!this->nbVoxels) return false;
    int index=getIndex(point);
    if (voronoi.size()!=nbVoxels) return false;
    if (voronoi[index]==-1) return false;
    Real dmax=0;
    for (i=0; i<nbVoxels; i++) if (grid.data()[i])  if (voronoi[i]==voronoi[index]) if (distances[i]>dmax) dmax=distances[i];
    if (dmax==0) return false;
    dmax*=factor;

    vector<Real> backupdistance;
    backupdistance.swap(distances);
    computeGeodesicalDistances(point,dmax);
    vector<Real> d(distances);
    computeGeodesicalDistancesToVoronoi(point,dmax);
    for (i=0; i<nbVoxels; i++) if (grid.data()[i]) if (d[i]<dmax)
            {
                if(d[i]==0) weights[i]=1;
                //else if(voronoi[i]==voronoi[index]) weights[i]=1.-d[i]/(factor*(d[i]+distances[i])); // inside voronoi: dist(frame,closestVoronoiBorder)=d+disttovoronoi
                //else weights[i]=1.-d[i]/(factor*(d[i]-distances[i]));	// outside voronoi: dist(frame,closestVoronoiBorder)=d-disttovoronoi
                else if(voronoi[i]==voronoi[index]) weights[i]=(factor-1.)/factor + distances[i]/(factor*(d[i]+distances[i])); // inside voronoi: dist(frame,closestVoronoiBorder)=d+disttovoronoi
                else weights[i]=(factor-1.)/factor - distances[i]/(factor*(d[i]-distances[i]));	// outside voronoi: dist(frame,closestVoronoiBorder)=d-disttovoronoi
                if(weights[i]<0) weights[i]=0;
                else if(weights[i]>1) weights[i]=1;
            }
    backupdistance.swap(distances);
    showedrepartition=-1;
    return true;  // 1 point = voxels its his voronoi region
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::computeLinearWeightsInVoronoi ( const SCoord& point, const Real factor)
/// linearly decreasing weight with support=factor*distmax_in_voronoi -> weight= factor*(1-d/distmax)
{
    unsigned int i;
    weights.resize(this->nbVoxels);
    for (i=0; i<this->nbVoxels; i++)  weights[i]=0;
    if (!this->nbVoxels) return false;
    int index=getIndex(point);
    if (voronoi.size()!=nbVoxels) return false;
    if (voronoi[index]==-1) return false;
    Real dmax=0;
    for (i=0; i<nbVoxels; i++) if (grid.data()[i])  if (voronoi[i]==voronoi[index]) if (distances[i]>dmax) dmax=distances[i];
    if (dmax==0) return false;
    dmax*=factor;
    vector<Real> backupdistance;
    backupdistance.swap(distances);
    computeGeodesicalDistances(point,dmax);
    for (i=0; i<nbVoxels; i++) if (grid.data()[i]) if (distances[i]<dmax) weights[i]=1.-distances[i]/dmax;
    backupdistance.swap(distances);
    showedrepartition=-1;
    return true;  // 1 point = voxels its his voronoi region
}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::offsetWeightsOutsideObject(unsigned int offestdist)
{
    if (!this->nbVoxels) return;
    unsigned int i,j;


    // get voxels within offsetdist
    VUI neighbors;
    vector<unsigned int> update((int)nbVoxels,0);
    for (i=0; i<nbVoxels; i++)
        if(grid.data()[i])
        {
            get26Neighbors(i, neighbors);
            for (j=0; j<neighbors.size(); j++) if (!grid.data()[neighbors[j]]) update[neighbors[j]]=1;
        }
    unsigned int nbiterations=0;
    while (nbiterations<offestdist)
    {
        for (i=0; i<nbVoxels; i++)
            if(update[i]==1)
            {
                get26Neighbors(i, neighbors);
                for (j=0; j<neighbors.size(); j++) if (!grid.data()[neighbors[j]]) update[neighbors[j]]=2;
                update[i]=3;
            }
        for (i=0; i<nbVoxels; i++) if(update[i]==2) update[i]=1;
        nbiterations++;
    }

    // diffuse 2*offsetdist times for the selected voxels
    nbiterations=0;
    while (nbiterations<2*offestdist)
    {
        for (i=0; i<this->nbVoxels; i++)
            if (update[i])
            {
                Real val=0,W=0;
                get26Neighbors(i, neighbors);
                for (j=0; j<neighbors.size(); j++)
                {
                    val+=weights[neighbors[j]];
                    W+=1;
                }
                if (W!=0) val=val/W; // normalize value
                weights[i]=val;
            }
        nbiterations++;
    }
}


template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::HeatDiffusion( const VecSCoord& points, const unsigned int hotpointindex,const bool fixdatavalue,const unsigned int max_iterations,const Real precision  )
{
    if (!this->nbVoxels) return false;
    unsigned int i,j,k,num_points=points.size();
    int index;
    weights.resize(this->nbVoxels);
    for (i=0; i<this->nbVoxels; i++)  weights[i]=0;

    vector<bool> isfixed((int)nbVoxels,false);
    vector<bool> update((int)nbVoxels,false);
    VUI neighbors;
    Real diffw,meanstiff;
    Real alphabias=5; // d^2/sigma^2 between a voxel and one its 6 neighbor.

    // intialisation: fix weight of points or regions
    for (i=0; i<num_points; i++)
    {
        index=getIndex(points[i]);
        if (index!=-1)
        {
            isfixed[index]=true;
            if (i==hotpointindex) weights[index]=1;
            else weights[index]=0;
            get6Neighbors(index, neighbors);
            for (j=0; j<neighbors.size(); j++) update[neighbors[j]]=true;
            if (fixdatavalue) // fix regions
                for (k=0; k<this->nbVoxels; k++)
                    if (grid.data()[k]==grid.data()[index])
                    {
                        isfixed[k]=true;
                        weights[k]=weights[index];
                        get6Neighbors(k, neighbors);
                        for (j=0; j<neighbors.size(); j++) update[neighbors[j]]=true;
                    }
        }
    }

    // diffuse
    unsigned int nbiterations=0;
    bool ok=false,ok2;
    Real maxchange=0.;
    while (!ok && nbiterations<max_iterations)
    {
        ok2=true;
        maxchange=0;
        //  #pragma omp parallel for private(j,neighbors,diffw,meanstiff)
        for (i=0; i<this->nbVoxels; i++)
            if (grid.data()[i])
                if (update[i])
                {
                    if (isfixed[i]) update[i]=false;
                    else
                    {
                        Real val=0,W=0;

                        if (this->distanceType.getValue().getSelectedId()==DISTANCE_ANISOTROPICDIFFUSION)
                        {
                            Real dv2;
                            int ip,im;
                            GCoord icoord;
                            getiCoord(i,icoord);
                            neighbors.clear();
                            for (j=0; j<3 ; j++)
                            {
                                icoord[j]+=1; ip=getIndex(icoord); icoord[j]-=2; im=getIndex(icoord); icoord[j]+=1;
                                    if (ip!=-1) if (grid.data()[ip]) neighbors.push_back(ip); else ip=i; else ip=i;
                                    if (im!=-1) if (grid.data()[im]) neighbors.push_back(im); else im=i; else im=i;

                                if(biasDistances.getValue())
                                {
                                    meanstiff=(getStiffness(grid.data()[ip])+getStiffness(grid.data()[im]))/2.;
                                    diffw=(double)exp(-alphabias/(meanstiff*meanstiff));
                                }
                                else diffw=1;

                                dv2=diffw*(weights[ip]-weights[im])*(weights[ip]-weights[im])/4.;
                                val+=(weights[ip]+weights[im])*dv2;
                                W+=2*dv2;
                            }
                        }
                        else
                        {
                            get6Neighbors(i, neighbors);
                            for (j=0; j<neighbors.size(); j++)
                                if (grid.data()[neighbors[j]])
                                {
                                    if(biasDistances.getValue())
                                    {
                                        meanstiff=(getStiffness(grid.data()[i])+getStiffness(grid.data()[neighbors[j]]))/2.;
                                        diffw=(double)exp(-alphabias/(meanstiff*meanstiff));
                                    }
                                    else diffw=1.;
                                    val+=diffw*weights[neighbors[j]];
                                    W+=diffw;
                                }
                                else
                                {
                                    val+=weights[i];    // dissipative border
                                    W+=1.;
                                }
                        }
                        if (W!=0) val=val/W; // normalize value

                        if (fabs(val-weights[i])<precision) update[i]=false;
                        else
                        {
                            if (fabs(val-weights[i])>maxchange) maxchange=fabs(val-weights[i]);
                            weights[i]=val;
                            ok2=false;
                            for (j=0; j<neighbors.size(); j++) update[neighbors[j]]=true;
                        }
                    }
                }
        ok=ok2;
        nbiterations++;
    }

    if (nbiterations==max_iterations)
    {
        serr<<"Heat diffusion has not converged in "<<nbiterations<<" iterations (precision="<<maxchange<<")"<<sendl;
        return false;
    }
    else std::cout<<"Heat diffusion completed in "<<nbiterations<<" iterations"<<std::endl;
    showedrepartition=-1;
    return true;
}





/*************************/
/*         Draw          */
/*************************/

template<class MaterialTypes>
void GridMaterial< MaterialTypes>::draw()
{
    if (!nbVoxels) return;

    unsigned int showvox=this->showVoxels.getValue().getSelectedId();

    if ( showvox!=SHOWVOXELS_NONE)
    {
        //glDisable ( GL_LIGHTING );
        //glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
        glLineWidth(1);

        unsigned int i;
        //        Real s=(voxelSize.getValue()[0]+voxelSize.getValue()[1]+voxelSize.getValue()[2])/3.;
        float defaultcolor[4]= {0.8,0.8,0.8,0.3},color[4];
        showWireframe=this->getContext()->getShowWireFrame();

        float label=-1,labelmax=-1;

        if (showvox==SHOWVOXELS_DATAVALUE)
        {
            for (i=0; i<nbVoxels; i++) if (grid.data()[i]>labelmax) labelmax=(float)grid.data()[i];
        }
        else if (showvox==SHOWVOXELS_STIFFNESS)
        {
            labelmax=20000;
        }
        else if (showvox==SHOWVOXELS_DENSITY)
        {
            labelmax=10000;
        }
        else if (showvox==SHOWVOXELS_BULKMODULUS)
        {
            labelmax=0.5;
        }
        else if (voronoi.size()==nbVoxels && showvox==SHOWVOXELS_VORONOI)
        {
            for (i=0; i<nbVoxels; i++) if (grid.data()[i]) if (voronoi[i]+1>labelmax) labelmax=(float)voronoi[i]+1;
        }
        else if (distances.size()==nbVoxels && showvox==SHOWVOXELS_DISTANCES)
        {
            for (i=0; i<nbVoxels; i++) if (grid.data()[i]) if (distances[i]>labelmax) labelmax=(float)distances[i];
        }
        else if (showvox==SHOWVOXELS_WEIGHTS)
        {
            labelmax=1;
            if (v_weights.size()==nbVoxels && v_index.size()==nbVoxels) // paste v_weights into weights
            {
                if (showedrepartition!=(int)showWeightIndex.getValue()) pasteRepartioninWeight(showWeightIndex.getValue());
                showedrepartition=(int)showWeightIndex.getValue();
            }
        }

        bool slicedisplay=false;
        for (i=0; i<3; i++) if(showPlane.getValue()[i]>=0 && showPlane.getValue()[i]<dimension.getValue()[i]) slicedisplay=true;

        cimg_forXYZ(grid,x,y,z)
        {
//                        if (grid(x,y,z)==0) continue;
            if (slicedisplay)
            {
                if(x!=showPlane.getValue()[0] && y!=showPlane.getValue()[1] && z!=showPlane.getValue()[2])
                    continue;
            }
            else
            {
                //VUI neighbors;
                //get6Neighbors(getIndex(GCoord(x,y,z)), neighbors);
                //if (!wireframe && neighbors.size()==6) // disable internal voxels -> not working anymore (return neighbors outside objects)
                //    continue;
            }

            if (labelmax!=-1)
                label = getLabel(x,y,z);

            if (label<=0) continue;

            if (label>labelmax) label=labelmax;
            helper::gl::Color::setHSVA(240.*label/labelmax,1.,.8,defaultcolor[3]);
            glGetFloatv(GL_CURRENT_COLOR, color);
            glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);

            SCoord coord;
            getCoord(GCoord(x,y,z),coord);
            drawCube((double)coord[0],(double)coord[1],(double)coord[2]);
        }

        if (show3DValues.getValue() && slicedisplay && vboSupported)
        {
            updateValuesVBO(showvox,labelmax);
            //displayValuesVBO();
        }

    }
}


template < class MaterialTypes>
float GridMaterial< MaterialTypes>::getLabel( const int&x, const int& y, const int& z)
{
    float label = -1;
    const unsigned int& showvox=this->showVoxels.getValue().getSelectedId();

    if (showvox==SHOWVOXELS_DATAVALUE) label=(float)grid(x,y,z);
    else if (showvox==SHOWVOXELS_STIFFNESS) label=(float)getStiffness(grid(x,y,z));
    else if (showvox==SHOWVOXELS_DENSITY) label=(float)getDensity(grid(x,y,z));
    else if (showvox==SHOWVOXELS_BULKMODULUS) label=(float)getBulkModulus(grid(x,y,z));
    else if (showvox==SHOWVOXELS_POISSONRATIO) label=(float)getPoissonRatio(grid(x,y,z));
    else if (voronoi.size()==nbVoxels && showvox==SHOWVOXELS_VORONOI)  label=(float)voronoi[getIndex(GCoord(x,y,z))]+1.;
    else if (distances.size()==nbVoxels && showvox==SHOWVOXELS_DISTANCES)  {if (grid(x,y,z)) label=(float)distances[getIndex(GCoord(x,y,z))]; else label=0; }
    else if (weights.size()==nbVoxels && showvox==SHOWVOXELS_WEIGHTS)  { if (grid(x,y,z)) label=(float)weights[getIndex(GCoord(x,y,z))]; else label=0; }
    return label;
}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::drawCube(const double& x, const double& y, const double& z) const
{
    const SCoord& size = voxelSize.getValue();
    glPushMatrix();
    glTranslated (x,y,z);
    glScaled(size[0]*0.5, size[1]*0.5, size[2]*0.5);
    if(showWireframe) glCallList(wcubeList); else glCallList(cubeList);
    glPopMatrix();
}


template < class MaterialTypes>
GLuint GridMaterial< MaterialTypes>::createVBO(const void* data, int dataSize, GLenum target, GLenum usage)
{
    GLuint id = 0;  // 0 is reserved, glGenBuffersARB() will return non-zero id if success

    glGenBuffersARB(1, &id);                        // create a vbo
    glBindBufferARB(target, id);                    // activate vbo id to use
    glBufferDataARB(target, dataSize, data, usage); // upload data to video card

    // check data size in VBO is same as input array, if not return 0 and delete VBO
    int bufferSize = 0;
    glGetBufferParameterivARB(target, GL_BUFFER_SIZE_ARB, &bufferSize);
    if(dataSize != bufferSize)
    {
        glDeleteBuffersARB(1, &id);
        id = 0;
        std::cout << "[createVBO()] Data size is mismatch with input array\n";
    }

    return id;      // return VBO id
}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::deleteVBO(const GLuint vboId)
{
    glDeleteBuffersARB(1, &vboId);
}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::initVBO()
{
    vboSupported = false; // TODO Check it later

    if(vboSupported)
    {
        int bufferSize; //TODO: allocate and initialize three views in the same VBO

        // Allocate
        unsigned int vertexSize = 3*((grid.width()+1) * (grid.height()+1) * (grid.depth()+1));
        unsigned int normalSize = vertexSize;
        unsigned int indicesSize = 0;
        valuesVertices = new GLfloat[vertexSize];
        valuesNormals = new GLfloat[normalSize];
        valuesIndices = new GLfloat[indicesSize];

        // Initialize
        /*
        unsigned int dim0 = (grid.width()+1) * (grid.height()+1);
        unsigned int dim1 = grid.width()+1;
        float vSX = voxelSize.getValue()[0];
        float vSY = voxelSize.getValue()[1];
        float vSZ = voxelSize.getValue()[2];
        float vSX2 = voxelSize.getValue()[0] * .5;
        float vSY2 = voxelSize.getValue()[1] * .5;
        float vSZ2 = voxelSize.getValue()[2] * .5;
        const SCoord& ori = origin.getValue();
        for (int x = 0; x < grid.width()+1; ++x)
        {
            for (int y = 0; y < grid.height()+1; ++y)
            {
                for (int z = 0; z < grid.depth()+1; ++z)
                {
                    for (unsigned int w = 0; w < 3; ++w)
                    {
                        //const unsigned int index = z+y*dim1+x*dim0;
                        //valuesVertices[w+3*(index)] = (ori + SCoord(x*vSX-vSX2,y*vSY-vSY2,z*vSZ-vSZ2))[w]; // TODO
                        //valuesNormals[w+3*(z+y*dim1+x*dim0)] = SCoord(-vSX, -vSY, -vSZ)[w]; // TODO
                    }
                    //valuesIndices[6*(z+y*dim1+x*dim0)+0] = index; // TODO
                }
            }
        }
        */

        // Allocate on GPU
        glGenBuffersARB(1, &vboValuesId1);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboValuesId1);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(valuesVertices)+sizeof(valuesNormals), 0, GL_STREAM_DRAW_ARB);
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(valuesVertices), valuesVertices);
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(valuesVertices), sizeof(valuesNormals), valuesNormals);
        glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
        std::cout << "Vertex and Normal Array in VBO: " << bufferSize << " bytes\n";

        vboValuesId2 = createVBO(valuesIndices, sizeof(valuesIndices), GL_ELEMENT_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);
        glGetBufferParameterivARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
        std::cout << "Index Array in VBO: " << bufferSize << " bytes\n";
    }
}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::updateValuesVBO( const bool& /*showvox*/, const float& /*labelmax*/)
{
    /*
    unsigned int dimX, dimY;
    Vec3i dimension;
    if( axis == 0) // Z axis
    {
        dimX = grid.width();
        dimY = grid.height();
    }
    else if( axis == 1) // X axis
    {
        dimX = grid.depth();
        dimY = grid.width();
    }
    else if( axis == 2) // Y axis
    {
        dimX = grid.height();
        dimY = grid.depth();
    }
    else
    {
        return;
    }
    */

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboValuesId1);
    float *ptr = (float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
    if(ptr)
    {
        unsigned int dim0 = grid.width() * grid.height();
        unsigned int dim1 = grid.width();
        cimg_forXYZ(grid,x,y,z)
        {
            if(x!=showPlane.getValue()[0] && y!=showPlane.getValue()[1] && z!=showPlane.getValue()[2])
                continue;

            SCoord coord;
            getCoord(GCoord(x,y,z),coord);
            for(unsigned int w = 0; w < 3; ++w)
                ptr[w+3*(z+y*dim1+x*dim0)] = coord[w];
        }

        glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    }

}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::displayValuesVBO( const int& /*size*/, const int& /*axis*/) const
{
    // Enable VBO
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    // before draw, specify vertex and index arrays with their offsets
    glNormalPointer(GL_FLOAT, 0, (void*)sizeof(valuesVertices));
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vboValuesId2);
    glIndexPointer(GL_UNSIGNED_SHORT, 0, 0);

    // use only offset here instead of absolute pointer addresses
    glDrawElements(GL_TRIANGLE_STRIP, 12, GL_UNSIGNED_SHORT, (GLushort*)0+0); // TODO display following the indices size.

    // Disable VBO
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    // Bind the buffers to 0 by safety
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::genListCube()
{
    cubeList = glGenLists(1);
    glNewList(cubeList, GL_COMPILE);

    glBegin(GL_QUADS);
    glNormal3f(1,0,0); glVertex3d(1.0,-1.0,-1.0); glVertex3d(1.0,-1.0,1.0); glVertex3d(1.0,1.0,1.0); glVertex3d(1.0,1.0,-1.0);
    glNormal3f(-1,0,0); glVertex3d(-1.0,-1.0,-1.0); glVertex3d(-1.0,-1.0,1.0); glVertex3d(-1.0,1.0,1.0); glVertex3d(-1.0,1.0,-1.0);
    glNormal3f(0,1,0); glVertex3d(-1.0,1.0,-1.0); glVertex3d(1.0,1.0,-1.0); glVertex3d(1.0,1.0,1.0); glVertex3d(-1.0,1.0,1.0);
    glNormal3f(0,-1,0); glVertex3d(-1.0,-1.0,-1.0); glVertex3d(1.0,-1.0,-1.0); glVertex3d(1.0,-1.0,1.0); glVertex3d(-1.0,-1.0,1.0);
    glNormal3f(0,0,1); glVertex3d(-1.0,-1.0,1.0); glVertex3d(-1.0,1.0,1.0); glVertex3d(1.0,1.0,1.0); glVertex3d(1.0,-1.0,1.0);
    glNormal3f(0,0,-1); glVertex3d(-1.0,-1.0,-1.0); glVertex3d(-1.0,1.0,-1.0); glVertex3d(1.0,1.0,-1.0); glVertex3d(1.0,-1.0,-1.0);
    glEnd ();
    glEndList();

    wcubeList = glGenLists(1);
    glNewList(wcubeList, GL_COMPILE);
    glBegin(GL_LINE_LOOP);
    glNormal3f(1,0,0); glVertex3d(1.0,-1.0,-1.0); glVertex3d(1.0,-1.0,1.0); glVertex3d(1.0,1.0,1.0); glVertex3d(1.0,1.0,-1.0);
    glEnd ();
    glBegin(GL_LINE_LOOP);
    glNormal3f(-1,0,0); glVertex3d(-1.0,-1.0,-1.0); glVertex3d(-1.0,-1.0,1.0); glVertex3d(-1.0,1.0,1.0); glVertex3d(-1.0,1.0,-1.0);
    glEnd ();
    glBegin(GL_LINE_LOOP);
    glNormal3f(0,1,0); glVertex3d(-1.0,1.0,-1.0); glVertex3d(1.0,1.0,-1.0); glVertex3d(1.0,1.0,1.0); glVertex3d(-1.0,1.0,1.0);
    glEnd ();
    glBegin(GL_LINE_LOOP);
    glNormal3f(0,-1,0); glVertex3d(-1.0,-1.0,-1.0); glVertex3d(1.0,-1.0,-1.0); glVertex3d(1.0,-1.0,1.0); glVertex3d(-1.0,-1.0,1.0);
    glEnd ();
    glBegin(GL_LINE_LOOP);
    glNormal3f(0,0,1); glVertex3d(-1.0,-1.0,1.0); glVertex3d(-1.0,1.0,1.0); glVertex3d(1.0,1.0,1.0); glVertex3d(1.0,-1.0,1.0);
    glEnd ();
    glBegin(GL_LINE_LOOP);
    glNormal3f(0,0,-1); glVertex3d(-1.0,-1.0,-1.0); glVertex3d(-1.0,1.0,-1.0); glVertex3d(1.0,1.0,-1.0); glVertex3d(1.0,-1.0,-1.0);
    glEnd ();
    glEndList();

}


/*************************/
/*         Utils         */
/*************************/
template < class MaterialTypes>
int GridMaterial< MaterialTypes>::getIndex(const GCoord& icoord)
{
    if (!nbVoxels) return -1;
    for (int i=0; i<3; i++) if (icoord[i]<0 || icoord[i]>=dimension.getValue()[i]) return -1; // invalid icoord (out of grid)
    return icoord[0]+dimension.getValue()[0]*icoord[1]+dimension.getValue()[0]*dimension.getValue()[1]*icoord[2];
}

template < class MaterialTypes>
int GridMaterial< MaterialTypes>::getIndex(const SCoord& coord)
{
    if (!nbVoxels) return -1;
    GCoord icoord;
    if (!getiCoord(coord,icoord)) return -1;
    return getIndex(icoord);
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::getiCoord(const SCoord& coord, GCoord& icoord)
{
    if (!nbVoxels) return false;
    Real val;
    for (unsigned int i=0; i<3; i++)
    {
        val=(coord[i]-(Real)origin.getValue()[i])/(Real)voxelSize.getValue()[i];
        val=((val-floor(val))<0.5)?floor(val):ceil(val); //round
        if (val<0 || val>=dimension.getValue()[i]) return false;
        icoord[i]=(int)val;
    }
    return true;
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::getiCoord(const int& index, GCoord& icoord)
{
    if (!nbVoxels) return false;
    if (index<0 || index>=(int)nbVoxels) return false; // invalid index
    icoord[2]=index/(dimension.getValue()[0]*dimension.getValue()[1]);
    icoord[1]=(index-icoord[2]*dimension.getValue()[0]*dimension.getValue()[1])/dimension.getValue()[0];
    icoord[0]=index-icoord[2]*dimension.getValue()[0]*dimension.getValue()[1]-icoord[1]*dimension.getValue()[0];
    return true;
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::getCoord(const GCoord& icoord, SCoord& coord)
{
    if (!nbVoxels) return false;
    for (unsigned int i=0; i<3; i++) if (icoord[i]<0 || icoord[i]>=dimension.getValue()[i]) return false; // invalid icoord (out of grid)
    coord=this->origin.getValue();
    for (unsigned int i=0; i<3; i++) coord[i]+=(Real)this->voxelSize.getValue()[i]*(Real)icoord[i];
    return true;
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::getCoord(const int& index, SCoord& coord)
{
    if (!nbVoxels) return false;
    GCoord icoord;
    if (!getiCoord(index,icoord)) return false;
    else return getCoord(icoord,coord);
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::get6Neighbors ( const int& index, VUI& neighbors )
{
    neighbors.clear();
    if (!nbVoxels) return false;
    int i;
    GCoord icoord;
    if (!getiCoord(index,icoord)) return false;
    for (unsigned int j=0; j<3 ; j++)
    {
        icoord[j]+=1;
        i=getIndex(icoord);
        if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
        icoord[j]-=1;
        icoord[j]-=1;
        i=getIndex(icoord);
        if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
        icoord[j]+=1;
    }
    return true;
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::get18Neighbors ( const int& index, VUI& neighbors )
{
    neighbors.clear();
    if (!nbVoxels) return false;
    int i;
    GCoord icoord;
    if (!getiCoord(index,icoord)) return false;
    for (unsigned int j=0; j<3 ; j++)
    {
        icoord[j]+=1;
        i=getIndex(icoord);
        if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
        icoord[j]-=1;
        icoord[j]-=1;
        i=getIndex(icoord);
        if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
        icoord[j]+=1;
    }

    for (unsigned int k=0; k<3 ; k++)
    {
        icoord[k]+=1;
        for (unsigned int j=k+1; j<3 ; j++)
        {
            icoord[j]+=1;
            i=getIndex(icoord);
            if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
            icoord[j]-=1;
            icoord[j]-=1;
            i=getIndex(icoord);
            if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
            icoord[j]+=1;
        }
        icoord[k]-=2;
        for (unsigned int j=k+1; j<3 ; j++)
        {
            icoord[j]+=1;
            i=getIndex(icoord);
            if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
            icoord[j]-=1;
            icoord[j]-=1;
            i=getIndex(icoord);
            if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
            icoord[j]+=1;
        }
        icoord[k]+=1;
    }
    return true;
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::get26Neighbors ( const int& index, VUI& neighbors )
{
    neighbors.clear();
    if (!nbVoxels) return false;
    int i;
    GCoord icoord;
    if (!getiCoord(index,icoord)) return false;
    for (unsigned int j=0; j<3 ; j++)
    {
        icoord[j]+=1;
        i=getIndex(icoord);
        if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
        icoord[j]-=1;
        icoord[j]-=1;
        i=getIndex(icoord);
        if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
        icoord[j]+=1;
    }
    for (unsigned int k=0; k<3 ; k++)
    {
        icoord[k]+=1;
        for (unsigned int j=k+1; j<3 ; j++)
        {
            icoord[j]+=1;
            i=getIndex(icoord);
            if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
            icoord[j]-=1;
            icoord[j]-=1;
            i=getIndex(icoord);
            if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
            icoord[j]+=1;
        }
        icoord[k]-=2;
        for (unsigned int j=k+1; j<3 ; j++)
        {
            icoord[j]+=1;
            i=getIndex(icoord);
            if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
            icoord[j]-=1;
            icoord[j]-=1;
            i=getIndex(icoord);
            if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
            icoord[j]+=1;
        }
        icoord[k]+=1;
    }
    icoord[0]+=1;
    icoord[1]+=1;
    icoord[2]+=1;
    i=getIndex(icoord);
    if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
    icoord[2]-=1;
    icoord[2]-=1;
    i=getIndex(icoord);
    if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
    icoord[2]+=1;
    icoord[1]-=2;
    icoord[2]+=1;
    i=getIndex(icoord);
    if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
    icoord[2]-=1;
    icoord[2]-=1;
    i=getIndex(icoord);
    if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
    icoord[2]+=1;
    icoord[1]+=1;
    icoord[0]-=2;
    icoord[1]+=1;
    icoord[2]+=1;
    i=getIndex(icoord);
    if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
    icoord[2]-=1;
    icoord[2]-=1;
    i=getIndex(icoord);
    if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
    icoord[2]+=1;
    icoord[1]-=2;
    icoord[2]+=1;
    i=getIndex(icoord);
    if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
    icoord[2]-=1;
    icoord[2]-=1;
    i=getIndex(icoord);
    if (i!=-1) /*if (grid.data()[i])*/ neighbors.push_back(i);
    icoord[2]+=1;
    icoord[1]+=1;
    icoord[0]+=1;
    return true;
}



template < class MaterialTypes>
void GridMaterial< MaterialTypes>::getCompleteBasis(const SCoord& p,const unsigned int order,vector<Real>& basis)
{
    unsigned int j,k,count=0,dim=(order+1)*(order+2)*(order+3)/6; // complete basis of order 'order'
    basis.resize(dim);
    for (j=0; j<dim; j++) basis[j]=0;

    SCoord p2;
    for (j=0; j<3; j++) p2[j]=p[j]*p[j];
    SCoord p3;
    for (j=0; j<3; j++) p3[j]=p2[j]*p[j];

    count=0;
    // order 0
    basis[count]=1;
    count++;
    if (count==dim) return;
    // order 1
    for (j=0; j<3; j++)
    {
        basis[count]=p[j];
        count++;
    }
    if (count==dim) return;
    // order 2
    for (j=0; j<3; j++) for (k=j; k<3; k++)
        {
            basis[count]=p[j]*p[k];
            count++;
        }
    if (count==dim) return;
    // order 3
    basis[count]=p[0]*p[1]*p[2];
    count++;
    for (j=0; j<3; j++) for (k=0; k<3; k++)
        {
            basis[count]=p2[j]*p[k];
            count++;
        }
    if (count==dim) return;
    // order 4
    for (j=0; j<3; j++) for (k=j; k<3; k++)
        {
            basis[count]=p2[j]*p2[k];
            count++;
        }
    basis[count]=p2[0]*p[1]*p[2];
    count++;
    basis[count]=p[0]*p2[1]*p[2];
    count++;
    basis[count]=p[0]*p[1]*p2[2];
    count++;
    for (j=0; j<3; j++) for (k=0; k<3; k++) if (j!=k)
            {
                basis[count]=p3[j]*p[k];
                count++;
            }
    if (count==dim) return;

    return; // order>4 not implemented...
}

template < class MaterialTypes>
void GridMaterial< MaterialTypes>::getCompleteBasisDeriv(const SCoord& p,const unsigned int order,vector<SGradient>& basisDeriv)
{
    unsigned int j,k,count=0,dim=(order+1)*(order+2)*(order+3)/6; // complete basis of order 'order'

    basisDeriv.resize(dim);
    for (j=0; j<dim; j++) basisDeriv[j].fill(0);

    SCoord p2;
    for (j=0; j<3; j++) p2[j]=p[j]*p[j];
    SCoord p3;
    for (j=0; j<3; j++) p3[j]=p2[j]*p[j];

    count=0;
    // order 0
    count++;
    if (count==dim) return;
    // order 1
    for (j=0; j<3; j++)
    {
        basisDeriv[count][j]=1;
        count++;
    }
    if (count==dim) return;
    // order 2
    for (j=0; j<3; j++) for (k=j; k<3; k++)
        {
            basisDeriv[count][k]+=p[j];
            basisDeriv[count][j]+=p[k];
            count++;
        }
    if (count==dim) return;
    // order 3
    basisDeriv[count][0]=p[1]*p[2];
    basisDeriv[count][1]=p[0]*p[2];
    basisDeriv[count][2]=p[0]*p[1];
    count++;
    for (j=0; j<3; j++) for (k=0; k<3; k++)
        {
            basisDeriv[count][k]+=p2[j];
            basisDeriv[count][j]+=2*p[j]*p[k];
            count++;
        }
    if (count==dim) return;
    // order 4
    for (j=0; j<3; j++) for (k=j; k<3; k++)
        {
            basisDeriv[count][k]=2*p2[j]*p[k];
            basisDeriv[count][j]=2*p[j]*p2[k];
            count++;
        }
    basisDeriv[count][0]=2*p[0]*p[1]*p[2];
    basisDeriv[count][1]=p2[0]*p[2];
    basisDeriv[count][2]=p2[0]*p[1];
    count++;
    basisDeriv[count][0]=p2[1]*p[2];
    basisDeriv[count][1]=2*p[0]*p[1]*p[2];
    basisDeriv[count][2]=p[0]*p2[1];
    count++;
    basisDeriv[count][0]=p[1]*p2[2];
    basisDeriv[count][1]=p[0]*p2[2];
    basisDeriv[count][2]=2*p[0]*p[1]*p[2];
    count++;
    for (j=0; j<3; j++) for (k=0; k<3; k++) if (j!=k)
            {
                basisDeriv[count][k]=p3[j];
                basisDeriv[count][j]=3*p2[j]*p[k];
                count++;
            }
    if (count==dim) return;

    return; // order>4 not implemented...
}


template < class MaterialTypes>
void GridMaterial< MaterialTypes>::getCompleteBasisDeriv2(const SCoord& p,const unsigned int order,vector<SHessian>& basisDeriv)
{
    unsigned int j,k,count=0,dim=(order+1)*(order+2)*(order+3)/6; // complete basis of order 'order'

    basisDeriv.resize(dim);
    for (k=0; k<dim; k++) basisDeriv[k].fill(0);

    SCoord p2;
    for (j=0; j<3; j++) p2[j]=p[j]*p[j];

    count=0;
    // order 0
    count++;
    if (count==dim) return;
    // order 1
    count+=3;
    if (count==dim) return;
    // order 2
    for (j=0; j<3; j++) for (k=j; k<3; k++)
        {
            basisDeriv[count][k][j]+=1;
            basisDeriv[count][j][k]+=1;
            count++;
        }
    if (count==dim) return;
    // order 3
    basisDeriv[count][0][1]=p[2];
    basisDeriv[count][0][2]=p[1];
    basisDeriv[count][1][0]=p[2];
    basisDeriv[count][1][2]=p[0];
    basisDeriv[count][2][0]=p[1];
    basisDeriv[count][2][1]=p[0];
    count++;
    for (j=0; j<3; j++) for (k=0; k<3; k++)
        {
            basisDeriv[count][k][j]+=2*p[j];
            basisDeriv[count][j][k]+=2*p[j];
            count++;
        }
    if (count==dim) return;
    // order 4
    for (j=0; j<3; j++) for (k=j; k<3; k++)
        {
            basisDeriv[count][k][j]=4*p[j]*p[k];
            basisDeriv[count][k][k]=2*p2[j];
            basisDeriv[count][j][j]=2*p2[k];
            basisDeriv[count][j][k]=4*p[j]*p[k];
            count++;
        }
    basisDeriv[count][0][0]=2*p[1]*p[2];
    basisDeriv[count][0][1]=2*p[0]*p[2];
    basisDeriv[count][0][2]=2*p[0]*p[1];
    basisDeriv[count][1][0]=2*p[0]*p[2];
    basisDeriv[count][1][2]=p2[0];
    basisDeriv[count][2][0]=2*p[0]*p[1];
    basisDeriv[count][2][1]=p2[0];
    count++;
    basisDeriv[count][0][1]=2*p[1]*p[2];
    basisDeriv[count][0][2]=p2[1];
    basisDeriv[count][1][0]=2*p[1]*p[2];
    basisDeriv[count][1][1]=2*p[0]*p[2];
    basisDeriv[count][1][2]=2*p[0]*p[1];
    basisDeriv[count][2][0]=p2[1];
    basisDeriv[count][2][1]=2*p[0]*p[1];
    count++;
    basisDeriv[count][0][1]=p2[2];
    basisDeriv[count][0][2]=2*p[1]*p[2];
    basisDeriv[count][1][0]=p2[2];
    basisDeriv[count][1][2]=2*p[0]*p[2];
    basisDeriv[count][2][0]=2*p[1]*p[2];
    basisDeriv[count][2][1]=2*p[0]*p[2];
    basisDeriv[count][2][2]=2*p[0]*p[1];
    count++;

    for (j=0; j<3; j++) for (k=0; k<3; k++) if (j!=k)
            {
                basisDeriv[count][k][j]=3*p2[j];
                basisDeriv[count][j][j]=6*p[j]*p[k];
                basisDeriv[count][j][k]=3*p2[j];
                count++;
            }
    if (count==dim) return;

    return; // order>4 not implemented...
}

template < class MaterialTypes>
typename GridMaterial< MaterialTypes>::Real GridMaterial< MaterialTypes>::findWeightInRepartition(const unsigned int& pointIndex, const unsigned int& frameIndex)
{
    if (v_index.size()<=pointIndex) return 0;
    for ( unsigned int j = 0; j < nbRef && v_weights[pointIndex][j]!=0 ; ++j)
        if ( v_index[pointIndex][j] == frameIndex)
            return v_weights[pointIndex][j];
    return 0;
}

template < class MaterialTypes>
bool GridMaterial< MaterialTypes>::areRepsSimilar(const unsigned int i1,const unsigned int i2)
{
    Vec<nbRef,bool> checked; checked.fill(false);
    unsigned int i,j;
    for(i=0; i<nbRef; i++)
        if(v_weights[i1][i])
        {
            j=0; while(j<nbRef && v_index[i1][i]!=v_index[i2][j]) j++;
            if(j==nbRef) return false;
            if(v_weights[i2][j]==0) return false;
            checked[j]=true;
        }
    for(j=0; j<nbRef; j++) if(!checked[j] && v_weights[i2][j]!=0) return false;
    return true;
}

} // namespace material

} // namespace component

} // namespace sofa

#endif


