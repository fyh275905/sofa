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
#ifndef SOFA_COMPONENT_MASS_MATRIXMASS_H
#define SOFA_COMPONENT_MASS_MATRIXMASS_H



#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/behavior/Mass.h>
#include <sofa/core/objectmodel/Event.h>
#include <sofa/component/topology/PointData.h>
#include <sofa/helper/vector.h>

namespace sofa
{



namespace component
{

namespace mass
{


/**
Mass Matrices. By default a diagonal matrix is created with diagonal to _defaultValue. Else matrices have to be given in the .scn file.
It is possible to use lumped matrices.
*/

template <class DataTypes, class MassType>
class MatrixMass : public core::behavior::Mass<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(MatrixMass,DataTypes,MassType), SOFA_TEMPLATE(core::behavior::Mass,DataTypes));

    typedef core::behavior::Mass<DataTypes> Inherited;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;
    typedef core::objectmodel::Data<VecCoord> DataVecCoord;
    typedef core::objectmodel::Data<VecDeriv> DataVecDeriv;

    typedef helper::vector<MassType> VecMass;


    Data< VecMass > f_mass;
    Data< bool >    _lumped;
    Data< Real >    _defaultValue;

    VecMass _lumpedMasses; ///< lumped mass matrices

    const VecMass* _usedMassMatrices; ///< what VecMass is used to represent matrices ? f_mass.getValue() or _lumpedMasses ?



    MatrixMass()
        :  f_mass( initData(&f_mass, "massMatrices", "values of the particles masses") )
        , _lumped(initData( &_lumped, false, "lumped", ""))
        , _defaultValue( initData(&_defaultValue, (Real)1.0,"defaultValue", "real default value") )
        , _usingDefaultDiagonalMatrices(false)
    {
    };

    ~MatrixMass();


    void clear();
    //void addMass(const MassType& mass);
    void resize(int vsize);


    virtual void init();
    virtual void reinit();


    // -- Mass interface
    void addMDx(DataVecDeriv& f, const DataVecDeriv& dx, double factor, const core::MechanicalParams* mparams);

    void accFromF(DataVecDeriv& a, const DataVecDeriv& f, const core::MechanicalParams* mparams);

    void addForce(DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v, const core::MechanicalParams* mparams);

    void addGravityToV(DataVecDeriv& d_v, const core::MechanicalParams* mparams);

    double getKineticEnergy(const DataVecDeriv& v, const core::MechanicalParams* mparams) const;  ///< vMv/2 using dof->getV()

    double getPotentialEnergy(const DataVecCoord& x, const core::MechanicalParams* mparams) const;   ///< Mgx potential in a uniform gravity field, null at origin

    /// Add Mass contribution to global Matrix assembling
    //void addMToMatrix(defaulttype::BaseMatrix * mat, double mFact, unsigned int &offset);
    void addMToMatrix(const sofa::core::behavior::MultiMatrixAccessor* matrix, const core::MechanicalParams *mparams);

    double getElementMass(unsigned int index) const;
    void getElementMass(unsigned int index, defaulttype::BaseMatrix *m) const;

protected:
    MassType diagonalMass( const Real& m ); ///< return a diagonal matrix mass with value m on all the diagonal
    MassType lump( const MassType& m ); ///< lump the matrix m, ie sum line on diagonal
    void lumpMatrices( ); ///< lump all mass matrices
    void defaultDiagonalMatrices( ); ///< compute default diagonal matrices
    bool _usingDefaultDiagonalMatrices; ///< default diagonal matrices are used

};

} // namespace mass

} // namespace component

} // namespace sofa

#endif
