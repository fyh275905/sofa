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
#ifndef SOFA_COMPONENT_FORCEFIELD_TRIANGULARFEMFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_TRIANGULARFEMFORCEFIELD_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/component/topology/TriangleData.h>
#include <sofa/component/topology/EdgeData.h>
#include <sofa/component/topology/PointData.h>
#include <newmat/newmat.h>
#include <newmat/newmatap.h>



namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::defaulttype;
using sofa::helper::vector;
using namespace sofa::component::topology;


/** corotational triangle from
 * @InProceedings{NPF05,
 *   author       = "Nesme, Matthieu and Payan, Yohan and Faure, Fran\c{c}ois",
 *   title        = "Efficient, Physically Plausible Finite Elements",
 *   booktitle    = "Eurographics (short papers)",
 *   month        = "august",
 *   year         = "2005",
 *   editor       = "J. Dingliana and F. Ganovelli",
 *   keywords     = "animation, physical model, elasticity, finite elements",
 *   url          = "http://www-evasion.imag.fr/Publications/2005/NPF05"
 * }
 */
template<class DataTypes>
class TriangularFEMForceField : public core::componentmodel::behavior::ForceField<DataTypes>, public virtual core::objectmodel::BaseObject
{
public:
    typedef core::componentmodel::behavior::ForceField<DataTypes> Inherited;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::VecReal VecReal;
    typedef VecCoord Vector;
    typedef typename DataTypes::Coord    Coord   ;
    typedef typename DataTypes::Deriv    Deriv   ;
    typedef typename Coord::value_type   Real    ;

    typedef sofa::core::componentmodel::topology::BaseMeshTopology::index_type Index;
    typedef sofa::core::componentmodel::topology::BaseMeshTopology::Triangle Element;
    typedef sofa::core::componentmodel::topology::BaseMeshTopology::SeqTriangles VecElement;

    static const int SMALL = 1;										    ///< Symbol of small displacements triangle solver
    static const int LARGE = 0;									    	///< Symbol of large displacements triangle solver

protected:

    bool _anisotropicMaterial;						// used to turn on / off optimizations
    typedef Vec<6, Real> Displacement;								///< the displacement vector
    typedef Mat<3, 3, Real> MaterialStiffness;						///< the matrix of material stiffness
    typedef sofa::helper::vector<MaterialStiffness> VecMaterialStiffness;    ///< a vector of material stiffness matrices
    typedef Mat<6, 3, Real> StrainDisplacement;						///< the strain-displacement matrix
    typedef sofa::helper::vector<StrainDisplacement> VecStrainDisplacement;	///< a vector of strain-displacement matrices
    typedef Mat<3, 3, Real > Transformation;						///< matrix for rigid transformations like rotations

    class TriangleInformation
    {
    public:
        /// material stiffness matrices of each tetrahedron
        MaterialStiffness materialMatrix;
        ///< the strain-displacement matrices vector
        StrainDisplacement strainDisplacementMatrix;
        // large displacement method
        helper::fixed_array<Coord,3> rotatedInitialElements;
        Transformation rotation;
        // strain vector
        Vec<3,Real> strain;
        // stress vector
        Vec<3,Real> stress;
        Transformation initialTransformation;
        Coord principalStressDirection;
        Real maxStress;
        Coord principalStrainDirection;
        Real maxStrain;
        TriangleInformation() { }
    };

    class EdgeInformation
    {
    public:
        EdgeInformation()
            :fracturable(false) {};

        bool fracturable;
    };

    class VertexInformation
    {
    public:
        VertexInformation()
            :sumEigenValues(0.0) {};

        Coord meanStrainDirection;
        double sumEigenValues;

        /// Output stream
        inline friend std::ostream& operator<< ( std::ostream& os, const VertexInformation& /*vi*/)
        {
            return os;
        }
        /// Input stream
        inline friend std::istream& operator>> ( std::istream& in, VertexInformation& /*vi*/)
        {
            return in;
        }
    };

    TriangleData<TriangleInformation> triangleInfo;
    PointData<VertexInformation> vertexInfo;
    EdgeData<EdgeInformation> edgeInfo;

    sofa::core::componentmodel::topology::BaseMeshTopology* _topology;
    //const VecElement *_indexedElements;
    //Data< VecCoord > _initialPoints; ///< the intial positions of the points
    VecCoord* _initialPoints;
    //     int _method; ///< the computation method of the displacements


    bool updateMatrix;
    int lastFracturedEdgeIndex;

public:

    TriangularFEMForceField();

    //virtual const char* getTypeName() const { return "TriangularFEMForceField"; }

    virtual ~TriangularFEMForceField();
    virtual void init();
    virtual void reinit();
    virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);
    virtual void addDForce (VecDeriv& df, const VecDeriv& dx);
    virtual double getPotentialEnergy(const VecCoord& x);
    virtual void handleTopologyChange();

    void draw();

    int method;
    Data<std::string> f_method;
    Data<Real> f_poisson;
    Data<Real> f_young;
    Data<Real> f_damping;
    Data<bool> f_fracturable;
    Data<bool> showStressValue;
    Data<bool> showStressVector;

    Real getPoisson() { return f_poisson.getValue(); }
    void setPoisson(Real val) { f_poisson.setValue(val); }
    Real getYoung() { return f_young.getValue(); }
    void setYoung(Real val) { f_young.setValue(val); }
    Real getDamping() { return f_damping.getValue(); }
    void setDamping(Real val) { f_damping.setValue(val); }
    int  getMethod() { return method; }
    void setMethod(int val) { method = val; }
    int  getFracturedEdge();
    void getFractureCriteria(int element, Deriv& direction, Real& value);
    /// Compute value of stress along a given direction (typically the fiber direction and transverse direction in anisotropic materials)
    void computeStressAlongDirection(Real &stress_along_dir, Index elementIndex, const Coord &direction, const Vec<3,Real> &stress);
    /// Compute value of stress along a given direction (typically the fiber direction and transverse direction in anisotropic materials)
    void computeStressAlongDirection(Real &stress_along_dir, Index elementIndex, const Coord &direction);
    /// Compute value of stress across a given direction (typically the fracture direction)
    void computeStressAcrossDirection(Real &stress_across_dir, Index elementIndex, const Coord &direction, const Vec<3,Real> &stress);
    /// Compute value of stress across a given direction (typically the fracture direction)
    void computeStressAcrossDirection(Real &stress_across_dir, Index elementIndex, const Coord &direction);
    /// Compute current stress
    void computeStress(Vec<3,Real> &stress, Index elementIndex);

protected :

    void computeDisplacementSmall(Displacement &D, Index elementIndex, const VecCoord &p);
    void computeDisplacementLarge(Displacement &D, Index elementIndex, const Transformation &R_2_0, const VecCoord &p);
    void computeStrainDisplacement( StrainDisplacement &J, Coord a, Coord b, Coord c );
    void computeStrain(Vec<3,Real> &strain, const StrainDisplacement &J, const Displacement &D);
    void computeStress(Vec<3,Real> &stress, MaterialStiffness &K, Vec<3,Real> &strain);
    void computeForce(Displacement &F, Index elementIndex, const VecCoord &p);
    void computePrincipalStrain(Index elementIndex, Vec<3,Real> &strain);
    void computePrincipalStress(Index elementIndex, Vec<3,Real> &stress);

    static void TRQSTriangleCreationFunction (int , void* , TriangleInformation &, const Triangle& , const sofa::helper::vector< unsigned int > &, const sofa::helper::vector< double >&);

    /// f += Kx where K is the stiffness matrix and x a displacement
    virtual void applyStiffness( VecCoord& f, Real h, const VecCoord& x );
    virtual void computeMaterialStiffness(int i, Index& a, Index& b, Index& c);

    ////////////// small displacements method
    void initSmall(int i, Index&a, Index&b, Index&c);
    void accumulateForceSmall( VecCoord& f, const VecCoord & p, Index elementIndex);
    void accumulateDampingSmall( VecCoord& f, Index elementIndex );
    void applyStiffnessSmall( VecCoord& f, Real h, const VecCoord& x );

    ////////////// large displacements method
    //sofa::helper::vector< helper::fixed_array <Coord, 3> > _rotatedInitialElements;   ///< The initials positions in its frame
    //sofa::helper::vector< Transformation > _rotations;
    void initLarge(int i, Index&a, Index&b, Index&c);
    void computeRotationLarge( Transformation &r, const VecCoord &p, const Index &a, const Index &b, const Index &c);
    void accumulateForceLarge( VecCoord& f, const VecCoord & p, Index elementIndex);
    void accumulateDampingLarge( VecCoord& f, Index elementIndex );
    void applyStiffnessLarge( VecCoord& f, Real h, const VecCoord& x );
};


#if defined(WIN32) && !defined(SOFA_COMPONENT_FORCEFIELD_TRIANGULARFEMFORCEFIELD_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_FORCEFIELD_API TriangularFEMForceField<defaulttype::Vec3dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_FORCEFIELD_API TriangularFEMForceField<defaulttype::Vec3fTypes>;
#endif
#endif

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
