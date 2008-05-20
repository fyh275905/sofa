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
#ifndef SOFA_COMPONENT_FORCEFIELD_NONUNIFORMHEXAHEDRONFEMFORCEFIELDDENSITY_H
#define SOFA_COMPONENT_FORCEFIELD_NONUNIFORMHEXAHEDRONFEMFORCEFIELDDENSITY_H


#include <sofa/component/forcefield/NonUniformHexahedronFEMForceFieldAndMass.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;
using sofa::helper::vector;

/** Need a SparseGridTopology with _sparseGrid->_nbVirtualFinerLevels >= this->_nbVirtualFinerLevels

@InProceedings{NPF06,
author       = "Nesme, Matthieu and Payan, Yohan and Faure, Fran\c{c}ois",
title        = "Animating Shapes at Arbitrary Resolution with Non-Uniform Stiffness",
booktitle    = "Eurographics Workshop in Virtual Reality Interaction and Physical Simulation (VRIPHYS)",
month        = "nov",
year         = "2006",
organization = "Eurographics",
address      = "Madrid",
url          = "http://www-evasion.imag.fr/Publications/2006/NPF06"
}


 */


template<class DataTypes>
class NonUniformHexahedronFEMForceFieldDensity :  public NonUniformHexahedronFEMForceFieldAndMass<DataTypes>
{
public:
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef VecCoord Vector;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;
#ifdef SOFA_NEW_HEXA
    typedef topology::MeshTopology::SeqHexas VecElement;
#else
    typedef topology::MeshTopology::SeqCubes VecElement;
#endif

    typedef HexahedronFEMForceFieldAndMass<DataTypes> HexahedronFEMForceFieldAndMassT;
    typedef HexahedronFEMForceField<DataTypes> HexahedronFEMForceFieldT;

    typedef typename HexahedronFEMForceFieldAndMassT::ElementStiffness ElementStiffness;
    typedef typename HexahedronFEMForceFieldAndMassT::MaterialStiffness MaterialStiffness;
    typedef typename HexahedronFEMForceFieldAndMassT::MassT MassT;
    typedef typename HexahedronFEMForceFieldAndMassT::ElementMass ElementMass;


public:

    NonUniformHexahedronFEMForceFieldDensity():NonUniformHexahedronFEMForceFieldAndMass<DataTypes>()
    {
        densityFile=initData(&this->densityFile,"densityFile","RAW File containing gray scale density");
        dimensionDensityFile = initData(&this->dimensionDensityFile, "dimensionDensityFile", "dimension of the RAW file");
    }

    void init();
    void draw();
// 	virtual void rein
    void drawSphere(double r, int lats, int longs, const Coord &pos);

protected:
    Data< std::string > densityFile;
    Data< Vec<3,unsigned int> > dimensionDensityFile;
    vector< vector < vector<unsigned char > > >voxels;
// 	  vector< int > stiffnessFactor;
    void computeCoarseElementStiffness( ElementStiffness &K, ElementMass &coarseMassElement, const int elementIndice,  int level);

    void computeMaterialStiffness(MaterialStiffness &m, double youngModulus, double poissonRatio);
};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
