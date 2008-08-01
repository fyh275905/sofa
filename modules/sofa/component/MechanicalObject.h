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
#ifndef SOFA_COMPONENT_MECHANICALOBJECT_H
#define SOFA_COMPONENT_MECHANICALOBJECT_H

#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/core/objectmodel/XDataPtr.h>
#include <sofa/core/objectmodel/VDataPtr.h>
#include <sofa/defaulttype/BaseVector.h>
#include <sofa/defaulttype/Quat.h>
#include <vector>
#include <assert.h>
#include <fstream>

namespace sofa
{

namespace component
{


using namespace core::componentmodel::behavior;
using namespace core::objectmodel;
using sofa::defaulttype::Vector3;

/// This class can be overridden if needed for additionnal storage within template specializations.
template<class DataTypes>
class MechanicalObjectInternalData
{
public:
};

template <class DataTypes>
class MechanicalObject : public MechanicalState<DataTypes>
{
public:
    typedef MechanicalState<DataTypes> Inherited;
    typedef typename Inherited::VecId VecId;
    typedef typename Inherited::VMultiOp VMultiOp;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::SparseDeriv SparseDeriv;
    typedef typename DataTypes::SparseVecDeriv SparseVecDeriv;
    typedef typename DataTypes::VecConst VecConst;

protected:
    VecCoord* x;
    VecDeriv* v;
    VecDeriv* f;
    VecDeriv* dx;
    VecCoord* x0;
    VecCoord* reset_position;

    VecDeriv* v0;
    VecDeriv* internalForces;
    VecDeriv* externalForces;
    VecCoord* xfree; // stores the position of the mechanical objet after a free movement (p.e. gravity action)
    VecDeriv* vfree; // stores the velocity of the mechanical objet after a free movement (p.e. gravity action)

    // Constraints stored in the Mechanical State
    // The storage is a SparseMatrix
    // Each constraint (Type TConst) contains the index of the related DOF
    VecConst *c;
    sofa::helper::vector<unsigned int> constraintId;

    bool initialized;
    Data< Vector3 > translation;
    Data< Vector3> rotation;
    Data< SReal > scale;
    Data< std::string > filename;

    /// @name Integration-related data
    /// @{

    sofa::helper::vector< VecCoord * > vectorsCoord;
    sofa::helper::vector< VecDeriv * > vectorsDeriv;
    sofa::helper::vector< VecConst * > vectorsConst;
    int vsize; ///< Number of elements to allocate in vectors

    void setVecCoord(unsigned int index, VecCoord* v);
    void setVecDeriv(unsigned int index, VecDeriv* v);
    void setVecConst(unsigned int index, VecConst* v);

    /// @}

    MechanicalObjectInternalData<DataTypes> data;

    friend class MechanicalObjectInternalData<DataTypes>;

    std::ofstream* m_gnuplotFileX;
    std::ofstream* m_gnuplotFileV;

    class Loader;

public:

    MechanicalObject();
    MechanicalObject& operator = ( const MechanicalObject& );

    virtual ~MechanicalObject();

    virtual bool load(const char* filename);

    virtual void parse ( BaseObjectDescription* arg );

    XDataPtr<DataTypes>* const f_X;
    VDataPtr<DataTypes>* const f_V;
    VDataPtr<DataTypes>* const f_F;
    VDataPtr<DataTypes>* const f_Dx;
    XDataPtr<DataTypes>* const f_Xfree;
    VDataPtr<DataTypes>* const f_Vfree;

    XDataPtr<DataTypes>* const f_X0;

    Data<SReal> restScale;

    Data<bool> debugViewIndices;
    Data<float> debugViewIndicesScale;

    virtual VecCoord* getX()  { f_X->beginEdit(); return x;  }
    virtual VecDeriv* getV()  { f_V->beginEdit(); return v;  }
    virtual VecDeriv* getF()  { f_F->beginEdit(); return f;  }
    virtual VecDeriv* getExternalForces()  { return externalForces;  }
    virtual VecDeriv* getDx() { f_Dx->beginEdit(); return dx; }
    virtual VecConst* getC() { return c;}
    virtual VecCoord* getXfree() { f_Xfree->beginEdit(); return xfree; }
    virtual VecDeriv* getVfree() { f_Vfree->beginEdit(); return vfree;  }
    VecCoord* getX0() { f_X0->beginEdit(); return x0;}

    virtual const VecCoord* getX()  const { return x;  }
    virtual const VecCoord* getX0()  const { return x0;  }
    virtual const VecDeriv* getV()  const { return v;  }
    virtual const VecDeriv* getV0()  const { return v0;  }
    virtual const VecDeriv* getF()  const { return f;  }
    virtual const VecDeriv* getExternalForces()  const { return externalForces;  }
    virtual const VecDeriv* getDx() const { return dx; }
    virtual const VecConst* getC() const { return c; }
    virtual const VecCoord* getXfree() const { return xfree; }
    virtual const VecDeriv* getVfree()  const { return vfree;  }

// 	SReal getScale(){return scale.getValue();};

    virtual void init();
    virtual void reinit();

    virtual void storeResetState();

    virtual void reset();

    virtual void writeX(std::ostream& out);
    virtual void readX(std::istream& in);
    virtual double compareX(std::istream& in);
    virtual void writeV(std::ostream& out);
    virtual void readV(std::istream& in);
    virtual double compareV(std::istream& in);

    virtual void writeState( std::ostream& out );

    virtual void initGnuplot(const std::string path);
    virtual void exportGnuplot(Real time);

    virtual void resize( int vsize);

    virtual bool addBBox(double* minBBox, double* maxBBox);

    int getSize() const
    {
        return vsize;
    }
    double getPX(int i) const { Real x=0.0,y=0.0,z=0.0; DataTypes::get(x,y,z,(*getX())[i]); return (SReal)x; }
    double getPY(int i) const { Real x=0.0,y=0.0,z=0.0; DataTypes::get(x,y,z,(*getX())[i]); return (SReal)y; }
    double getPZ(int i) const { Real x=0.0,y=0.0,z=0.0; DataTypes::get(x,y,z,(*getX())[i]); return (SReal)z; }

    /** \brief Overwrite values at index outputIndex by the ones at inputIndex.
     *
     */
    void replaceValue (const int inputIndex, const int outputIndex);



    /** \brief Exchange values at indices idx1 and idx2.
     *
     */
    void swapValues (const int idx1, const int idx2);



    /** \brief Reorder values according to parameter.
     *
     * Result of this method is :
     * newValue[ i ] = oldValue[ index[i] ];
     */
    void renumberValues( const sofa::helper::vector<unsigned int> &index );



    /** \brief Replace the value at index by the sum of the ancestors values weithed by the coefs.
     *
     * Sum of the coefs should usually equal to 1.0
     */
    void computeWeightedValue( const unsigned int i, const sofa::helper::vector< unsigned int >& ancestors, const sofa::helper::vector< double >& coefs);

    /** \brief Compute the values attached to a new point.
     *
     */
    void computeNewPoint( const unsigned int i, const sofa::helper::vector< double >& m_x);

    // Force the position of a point (and force its velocity to zero value)
    void forcePointPosition( const unsigned int i, const sofa::helper::vector< double >& m_x);

    virtual void applyTranslation (const double dx,const double dy,const double dz);

    // rotation using Euler Angles in degree
    virtual void applyRotation (const double rx, const double ry, const double rz);

    virtual void applyRotation (const defaulttype::Quat q);

    virtual void applyScale (const double s);

    /// Get the indices of the particles located in the given bounding box
    void getIndicesInSpace(sofa::helper::vector<unsigned>& indices, Real xmin, Real xmax, Real ymin, Real ymax, Real zmin, Real zmax) const;


    /// @Base Matrices and Vectors Interface
    /// @{

    /// Add the Mechanical State Dimension [DOF number * DOF dimension] to the global matrix dimension
    virtual void contributeToMatrixDimension(unsigned int * const, unsigned int * const);

    /// Load local mechanical data stored in the state in a global BaseVector basically stored in solvers
    virtual void loadInBaseVector(defaulttype::BaseVector *, VecId , unsigned int &);

    /// Add data stored in a BaseVector to a local mechanical vector of the MechanicalState
    virtual void addBaseVectorToState(VecId , defaulttype::BaseVector *, unsigned int &);

    /// Update offset index during the subgraph traversal
    virtual void setOffset(unsigned int &);

    /// @}

    void setFilename(std::string s) {filename.setValue(s);};
    std::string getFilename() {return filename.getValue();};

    virtual void addDxToCollisionModel(void);

    void setConstraintId(unsigned int);
    sofa::helper::vector<unsigned int>& getConstraintId();


    /// @name Integration related methods
    /// @{

    virtual void beginIntegration(Real dt);

    virtual void endIntegration(Real dt);

    virtual void accumulateForce();

    VecCoord* getVecCoord(unsigned int index);
    const VecCoord* getVecCoord(unsigned int index) const;

    VecDeriv* getVecDeriv(unsigned int index);
    const VecDeriv* getVecDeriv(unsigned int index) const;

    VecConst* getVecConst(unsigned int index);
    const VecConst* getVecConst(unsigned int index) const;

    virtual void vAvail(VecId& v);

    virtual void vAlloc(VecId v);

    virtual void vFree(VecId v);

    virtual void vOp(VecId v, VecId a = VecId::null(), VecId b = VecId::null(), double f=1.0);

    virtual void vMultiOp(const VMultiOp& ops);

    virtual void vThreshold( VecId a, double threshold );

    virtual double vDot(VecId a, VecId b);

    virtual void setX(VecId v);

    virtual void setXfree(VecId v);

    virtual void setVfree(VecId v);

    virtual void setV(VecId v);

    virtual void setF(VecId v);

    virtual void setDx(VecId v);

    virtual void setC(VecId v);

    virtual void resetForce();

    virtual void resetConstraint();

    /// @}

    /// @name Debug
    /// @{
    virtual void printDOF( VecId, std::ostream& =std::cerr );
    virtual unsigned printDOFWithElapsedTime(VecId, unsigned =0, unsigned =0, std::ostream& =std::cerr );
    //
    void draw();
    /// @}

    // handle state changes
    virtual void handleStateChange();

protected:
    sofa::core::componentmodel::topology::BaseMeshTopology* _topology;


};

} // namespace component

} // namespace sofa

#endif
