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
#ifndef SOFA_COMPONENT_FORCEFIELD_TETRAHEDRALCOROTATIONALFEMFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_TETRAHEDRALCOROTATIONALFEMFORCEFIELD_H

#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/component/topology/TetrahedronData.h>
#include <sofa/helper/vector.h>
#include <sofa/defaulttype/Vec.h>
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
using namespace sofa::component::topology;



/** Compute Finite Element forces based on tetrahedral elements.
*/
template<class DataTypes>
class TetrahedralCorotationalFEMForceField : public core::componentmodel::behavior::ForceField<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(TetrahedralCorotationalFEMForceField, DataTypes), SOFA_TEMPLATE(core::componentmodel::behavior::ForceField, DataTypes));

    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::VecReal VecReal;
    typedef VecCoord Vector;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;


    enum { SMALL = 0, ///< Symbol of small displacements tetrahedron solver
            LARGE = 1, ///< Symbol of large displacements tetrahedron solver
            POLAR = 2  ///< Symbol of polar displacements tetrahedron solver
         };
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

    /// the information stored for each tetrahedron
    class TetrahedronInformation
    {
    public:
        /// material stiffness matrices of each tetrahedron
        MaterialStiffness materialMatrix;
        /// the strain-displacement matrices vector
        StrainDisplacement strainDisplacementMatrix;
        /// large displacement method
        helper::fixed_array<Coord,4> rotatedInitialElements;
        Transformation rotation;
        /// polar method
        Transformation initialTransformation;

        TetrahedronInformation()
        {
        }

        /// Output stream
        inline friend std::ostream& operator<< ( std::ostream& os, const TetrahedronInformation& /*tri*/ )
        {
            return os;
        }

        /// Input stream
        inline friend std::istream& operator>> ( std::istream& in, TetrahedronInformation& /*tri*/ )
        {
            return in;
        }
    };
    /// container that stotes all requires information for each tetrahedron
    TetrahedronData<TetrahedronInformation> tetrahedronInfo;

    /// @name Full system matrix assembly support
    /// @{

    typedef std::pair<int,Real> Col_Value;
    typedef vector< Col_Value > CompressedValue;
    typedef vector< CompressedValue > CompressedMatrix;
    typedef unsigned int Index;

    CompressedMatrix _stiffnesses;
    /// @}

    double m_potentialEnergy;

    sofa::core::componentmodel::topology::BaseMeshTopology* _topology;

public:
    int method;
    Data<std::string> f_method; ///< the computation method of the displacements
    Data<Real> _poissonRatio;
    Data<Real> _youngModulus;
    Data<VecReal> _localStiffnessFactor;
    Data<bool> _updateStiffnessMatrix;
    Data<bool> _assembling;

    TetrahedralCorotationalFEMForceField()
        : f_method(initData(&f_method,std::string("large"),"method","\"small\", \"large\" (by QR) or \"polar\" displacements"))
        , _poissonRatio(core::objectmodel::BaseObject::initData(&_poissonRatio,(Real)0.45f,"poissonRatio","FEM Poisson Ratio"))
        , _youngModulus(core::objectmodel::BaseObject::initData(&_youngModulus,(Real)5000,"youngModulus","FEM Young Modulus"))
        , _localStiffnessFactor(core::objectmodel::BaseObject::initData(&_localStiffnessFactor,"localStiffnessFactor","Allow specification of different stiffness per element. If there are N element and M values are specified, the youngModulus factor for element i would be localStiffnessFactor[i*M/N]"))
        , _updateStiffnessMatrix(core::objectmodel::BaseObject::initData(&_updateStiffnessMatrix,false,"updateStiffnessMatrix",""))
        , _assembling(core::objectmodel::BaseObject::initData(&_assembling,false,"computeGlobalMatrix",""))
    {
        this->addAlias(&_assembling, "assembling");
    }

    void setPoissonRatio(Real val) { this->_poissonRatio.setValue(val); }

    void setYoungModulus(Real val) { this->_youngModulus.setValue(val); }

    void setMethod(int val) { method = val; }

    void setUpdateStiffnessMatrix(bool val) { this->_updateStiffnessMatrix.setValue(val); }

    void setComputeGlobalMatrix(bool val) { this->_assembling.setValue(val); }

    virtual void init();
    virtual void reinit();

    virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    virtual void addDForce (VecDeriv& df, const VecDeriv& dx);

    virtual double getPotentialEnergy(const VecCoord& x);

    virtual void addKToMatrix(sofa::defaulttype::BaseMatrix *, SReal, unsigned int &);

    // handle topological changes
    virtual void handleTopologyChange();

    // Getting the rotation of the vertex by averaing the rotation of neighboring elements
    void getRotation(Transformation& R, unsigned int nodeIdx);
    void getRotations() {};
    void getElementRotation(Transformation& R, unsigned int elementIdx);

    // Getting the stiffness matrix of index i
    void getElementStiffnessMatrix(Real* stiffness, unsigned int nodeIdx);

    void draw();

protected:

    void computeStrainDisplacement( StrainDisplacement &J, Coord a, Coord b, Coord c, Coord d );
    Real peudo_determinant_for_coef ( const Mat<2, 3, Real>&  M );

    void computeStiffnessMatrix( StiffnessMatrix& S,StiffnessMatrix& SR,const MaterialStiffness &K, const StrainDisplacement &J, const Transformation& Rot );

    void computeMaterialStiffness(int i, Index&a, Index&b, Index&c, Index&d);

    void computeForce( Displacement &F, const Displacement &Depl, const MaterialStiffness &K, const StrainDisplacement &J );

    ////////////// small displacements method
    void initSmall(int i, Index&a, Index&b, Index&c, Index&d);
    void accumulateForceSmall( Vector& f, const Vector & p, Index elementIndex );
    void applyStiffnessSmall( Vector& f, const Vector& x, int i=0, Index a=0,Index b=1,Index c=2,Index d=3  );

    ////////////// large displacements method
    void initLarge(int i, Index&a, Index&b, Index&c, Index&d);
    void computeRotationLarge( Transformation &r, const Vector &p, const Index &a, const Index &b, const Index &c);
    void accumulateForceLarge( Vector& f, const Vector & p, Index elementIndex );
    void applyStiffnessLarge( Vector& f, const Vector& x, int i=0, Index a=0,Index b=1,Index c=2,Index d=3 );

    ////////////// polar decomposition method
    void initPolar(int i, Index&a, Index&b, Index&c, Index&d);
    void accumulateForcePolar( Vector& f, const Vector & p,Index elementIndex );
    void applyStiffnessPolar( Vector& f, const Vector& x, int i=0, Index a=0,Index b=1,Index c=2,Index d=3  );

    /// the callback function called when a tetrahedron is created
    static void CFTetrahedronCreationFunction (int , void* ,
            TetrahedronInformation &,
            const Tetrahedron& , const helper::vector< unsigned int > &, const helper::vector< double >&);



};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
