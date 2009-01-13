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
#ifndef SOFA_COMPONENT_FORCEFIELD_TETRAHEDRONFEMFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_TETRAHEDRONFEMFORCEFIELD_H

#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#ifdef SOFA_DEV
#include <sofa/component/topology/FittedRegularGridTopology.h>
#endif // SOFA_DEV
#include <sofa/helper/vector.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/Mat.h>


// corotational tetrahedron from
// @InProceedings{NPF05,
//   author       = "Nesme, Matthieu and Payan, Yohan and Faure, Fran\c{c}ois",
//   title        = "Efficient, Physically Plausible Finite Elements",
//   booktitle    = "Eurographics (short papers)",
//   month        = "august",
//   year         = "2005",
//   editor       = "J. Dingliana and F. Ganovelli",
//   keywords     = "animation, physical model, elasticity, finite elements",
//   url          = "http://www-evasion.imag.fr/Publications/2005/NPF05"
// }


namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;
using sofa::helper::vector;


/// This class can be overridden if needed for additionnal storage within template specializations.
template<class DataTypes>
class TetrahedronFEMForceFieldInternalData
{
public:
};


/** Compute Finite Element forces based on tetrahedral elements.
*/
template<class DataTypes>
class TetrahedronFEMForceField : public core::componentmodel::behavior::ForceField<DataTypes>, public virtual core::objectmodel::BaseObject
{
public:
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::VecReal VecReal;
    typedef VecCoord Vector;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;

    typedef core::componentmodel::topology::BaseMeshTopology::index_type Index;
    typedef core::componentmodel::topology::BaseMeshTopology::Tetra Element;
    typedef core::componentmodel::topology::BaseMeshTopology::SeqTetras VecElement;

    enum { SMALL = 0,   ///< Symbol of small displacements tetrahedron solver
            LARGE = 1,   ///< Symbol of large displacements tetrahedron solver
            POLAR = 2
         }; ///< Symbol of polar displacements tetrahedron solver

protected:

    /// @name Per element (tetrahedron) data
    /// @{

    /// Displacement vector (deformation of the 4 corners of a tetrahedron
    typedef VecNoInit<12, Real> Displacement;

    /// Material stiffness matrix of a tetrahedron
    typedef Mat<6, 6, Real> MaterialStiffness;

    /// Strain-displacement matrix
    typedef Mat<12, 6, Real> StrainDisplacement;

    /// Rigid transformation (rotation) matrix
    typedef MatNoInit<3, 3, Real> Transformation;

    /// Stiffness matrix ( = RJKJtRt  with K the Material stiffness matrix, J the strain-displacement matrix, and R the transformation matrix if any )
    typedef Mat<12, 12, Real> StiffnessMatrix;

    /// @}

    /// Vector of material stiffness of each tetrahedron
    typedef vector<MaterialStiffness> VecMaterialStiffness;
    typedef vector<StrainDisplacement> VecStrainDisplacement;  ///< a vector of strain-displacement matrices

    /// Vector of material stiffness matrices of each tetrahedron
    VecMaterialStiffness _materialsStiffnesses;
    VecStrainDisplacement _strainDisplacements;   ///< the strain-displacement matrices vector

    /// @name Full system matrix assembly support
    /// @{

    typedef std::pair<int,Real> Col_Value;
    typedef vector< Col_Value > CompressedValue;
    typedef vector< CompressedValue > CompressedMatrix;

    CompressedMatrix _stiffnesses;
    /// @}

    SReal m_potentialEnergy;

    core::componentmodel::topology::BaseMeshTopology* _mesh;
#ifdef SOFA_DEV
    topology::FittedRegularGridTopology* _trimgrid;
#endif // SOFA_DEV
    const VecElement *_indexedElements;
    VecCoord _initialPoints; ///< the intial positions of the points

    bool needUpdateTopology;

    TetrahedronFEMForceFieldInternalData<DataTypes> data;
    friend class TetrahedronFEMForceFieldInternalData<DataTypes>;

public:
    //For a faster contact handling with simplified compliance
    inline void getRotation(Transformation& R, unsigned int nodeIdx);


    DataPtr< VecCoord > f_initialPoints; ///< the intial positions of the points
    int method;
    Data<std::string> f_method; ///< the computation method of the displacements
    Real   _poissonRatio;
    DataPtr<Real> f_poissonRatio;
    Real   _youngModulus;
    DataPtr<Real> f_youngModulus;
    VecReal   _localStiffnessFactor;
    DataPtr<VecReal> f_localStiffnessFactor;
    bool   _updateStiffnessMatrix;
    DataPtr<bool> f_updateStiffnessMatrix;
    bool   _assembling;
    DataPtr<bool> f_assembling;

    TetrahedronFEMForceField()
        : _mesh(NULL)
#ifdef SOFA_DEV
        , _trimgrid(NULL)
#endif // SOFA_DEV
        , _indexedElements(NULL)
        , needUpdateTopology(false)
        , f_initialPoints(initDataPtr(&f_initialPoints, &_initialPoints, "initialPoints", "Initial Position"))
        , f_method(initData(&f_method,std::string("large"),"method","\"small\", \"large\" (by QR) or \"polar\" displacements"))
        ,  _poissonRatio((Real)0.45f)
        , f_poissonRatio(initDataPtr(&f_poissonRatio,&_poissonRatio,"poissonRatio","FEM Poisson Ratio"))
        ,  _youngModulus((Real)5000)
        , f_youngModulus(initDataPtr(&f_youngModulus,&_youngModulus,"youngModulus","FEM Young Modulus"))
        , f_localStiffnessFactor(initDataPtr(&f_localStiffnessFactor,&_localStiffnessFactor,"localStiffnessFactor","Allow specification of different stiffness per element. If there are N element and M values are specified, the youngModulus factor for element i would be localStiffnessFactor[i*M/N]"))
        ,  _updateStiffnessMatrix(false)
        , f_updateStiffnessMatrix(initDataPtr(&f_updateStiffnessMatrix,&_updateStiffnessMatrix,"updateStiffnessMatrix",""))
        ,  _assembling(false)
        , f_assembling(initDataPtr(&f_assembling,&_assembling,"assembling",""))
    {}

    void parse(core::objectmodel::BaseObjectDescription* arg);

    void setPoissonRatio(Real val) { this->f_poissonRatio.setValue(val); }

    void setYoungModulus(Real val) { this->f_youngModulus.setValue(val); }

    void setMethod(std::string methodName)
    {
        if (methodName == "small")	this->setMethod(SMALL);
        else if (methodName  == "polar")	this->setMethod(POLAR);
        else
        {
            if (methodName != "large")
                serr << "unknown method: large method will be used. Remark: Available method are \"small\", \"polar\", \"large\" "<<sendl;
            this->setMethod(LARGE);
        }
    }

    void setMethod(int val)
    {
        method = val;
        switch(val)
        {
        case SMALL: f_method.setValue("small"); break;
        case POLAR: f_method.setValue("polar"); break;
        default   : f_method.setValue("large");
        };
    }

    void setUpdateStiffnessMatrix(bool val) { this->f_updateStiffnessMatrix.setValue(val); }

    void setComputeGlobalMatrix(bool val) { this->f_assembling.setValue(val); }

    virtual void init();
    virtual void reinit();

    virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    virtual void addDForce (VecDeriv& df, const VecDeriv& dx, double kFactor, double bFactor);

    virtual double getPotentialEnergy(const VecCoord& x);

    virtual void addKToMatrix(sofa::defaulttype::BaseMatrix *, SReal, unsigned int &);

    void draw();

protected:

    void computeStrainDisplacement( StrainDisplacement &J, Coord a, Coord b, Coord c, Coord d );
    Real peudo_determinant_for_coef ( const Mat<2, 3, Real>&  M );

    void computeStiffnessMatrix( StiffnessMatrix& S,StiffnessMatrix& SR,const MaterialStiffness &K, const StrainDisplacement &J, const Transformation& Rot );

    void computeMaterialStiffness(int i, Index&a, Index&b, Index&c, Index&d);

    void computeForce( Displacement &F, const Displacement &Depl, const MaterialStiffness &K, const StrainDisplacement &J );
    void computeForce( Displacement &F, const Displacement &Depl, const MaterialStiffness &K, const StrainDisplacement &J, double fact );

    ////////////// small displacements method
    void initSmall(int i, Index&a, Index&b, Index&c, Index&d);
    void accumulateForceSmall( Vector& f, const Vector & p, typename VecElement::const_iterator elementIt, Index elementIndex );
    void applyStiffnessSmall( Vector& f, const Vector& x, int i=0, Index a=0,Index b=1,Index c=2,Index d=3, double fact=1.0  );

    ////////////// large displacements method
    vector<helper::fixed_array<Coord,4> > _rotatedInitialElements;   ///< The initials positions in its frame
    vector<Transformation> _rotations;
    vector<Transformation> _initialRotations;
    void initLarge(int i, Index&a, Index&b, Index&c, Index&d);
    void computeRotationLarge( Transformation &r, const Vector &p, const Index &a, const Index &b, const Index &c);
    void accumulateForceLarge( Vector& f, const Vector & p, typename VecElement::const_iterator elementIt, Index elementIndex );
    void applyStiffnessLarge( Vector& f, const Vector& x, int i=0, Index a=0,Index b=1,Index c=2,Index d=3, double fact=1.0 );

    ////////////// polar decomposition method
    vector<Transformation>  _initialTransformation;
    vector<unsigned int> _rotationIdx;
    void initPolar(int i, Index&a, Index&b, Index&c, Index&d);
    void accumulateForcePolar( Vector& f, const Vector & p, typename VecElement::const_iterator elementIt, Index elementIndex );
    void applyStiffnessPolar( Vector& f, const Vector& x, int i=0, Index a=0,Index b=1,Index c=2,Index d=3, double fact=1.0  );

    void handleTopologyChange()
    {
        needUpdateTopology = true;
    }
};

#if defined(WIN32) && !defined(SOFA_COMPONENT_FORCEFIELD_TETRAHEDRONFEMFORCEFIELD_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_FORCEFIELD_API TetrahedronFEMForceField<defaulttype::Vec3dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_FORCEFIELD_API TetrahedronFEMForceField<defaulttype::Vec3fTypes>;
#endif
#endif

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
