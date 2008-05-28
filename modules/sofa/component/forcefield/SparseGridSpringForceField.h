#ifndef SOFA_COMPONENT_FORCEFIELD_SPARSEGRIDSPRINGFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_SPARSEGRIDSPRINGFORCEFIELD_H

#include <sofa/component/forcefield/StiffSpringForceField.h>
#include <sofa/component/topology/MultiResSparseGridTopology.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

template<class DataTypes>
class SparseGridSpringForceField : public StiffSpringForceField<DataTypes>
{
    /// make the spring force field of a sparsegrid
public:
    typedef StiffSpringForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;

protected:

    DataField< Real > linesStiffness;
    DataField< Real > linesDamping;
    DataField< Real > quadsStiffness;
    DataField< Real > quadsDamping;
    DataField< Real > cubesStiffness;
    DataField< Real > cubesDamping;

    DataField< std::string > filename;
    typedef topology::MultiResSparseGridTopology::SparseGrid Voxels;
    typedef topology::MultiResSparseGridTopology::SparseGrid::Index3D Index3D;


public:
    SparseGridSpringForceField(core::componentmodel::behavior::MechanicalState<DataTypes>* object1, core::componentmodel::behavior::MechanicalState<DataTypes>* object2)
        : StiffSpringForceField<DataTypes>(object1, object2),
          linesStiffness  (dataField(&linesStiffness,Real(0),"linesStiffness","Lines Stiffness"))
          , linesDamping  (dataField(&linesDamping  ,Real(0),"linesDamping"  ,"Lines Damping"))
          , quadsStiffness(dataField(&quadsStiffness,Real(0),"quadsStiffness","Quads Stiffness"))
          , quadsDamping  (dataField(&quadsDamping  ,Real(0),"quadsDamping"  ,"Quads Damping"))
          , cubesStiffness(dataField(&cubesStiffness,Real(0),"cubesStiffness","Cubes Stiffness"))
          , cubesDamping  (dataField(&cubesDamping  ,Real(0),"cubesDamping"  ,"Cubes Damping"))
    {
    }

    SparseGridSpringForceField()
        :
        linesStiffness  (dataField(&linesStiffness,Real(0),"linesStiffness","Lines Stiffness"))
        , linesDamping  (dataField(&linesDamping  ,Real(0),"linesDamping"  ,"Lines Damping"))
        , quadsStiffness(dataField(&quadsStiffness,Real(0),"quadsStiffness","Quads Stiffness"))
        , quadsDamping  (dataField(&quadsDamping  ,Real(0),"quadsDamping"  ,"Quads Damping"))
        , cubesStiffness(dataField(&cubesStiffness,Real(0),"cubesStiffness","Cubes Stiffness"))
        , cubesDamping  (dataField(&cubesDamping  ,Real(0),"cubesDamping"  ,"Cubes Damping"))
    {
    }

    virtual void parse(core::objectmodel::BaseObjectDescription* arg);

    Real getStiffness() const { return linesStiffness.getValue(); }
    Real getLinesStiffness() const { return linesStiffness.getValue(); }
    Real getQuadsStiffness() const { return quadsStiffness.getValue(); }
    Real getCubesStiffness() const { return cubesStiffness.getValue(); }
    void setStiffness(Real val)
    {
        linesStiffness.setValue(val);
        quadsStiffness.setValue(val);
        cubesStiffness.setValue(val);
    }
    void setLinesStiffness(Real val)
    {
        linesStiffness.setValue(val);
    }
    void setQuadsStiffness(Real val)
    {
        quadsStiffness.setValue(val);
    }
    void setCubesStiffness(Real val)
    {
        cubesStiffness.setValue(val);
    }


    Real getDamping() const { return linesDamping.getValue(); }
    Real getLinesDamping() const { return linesDamping.getValue(); }
    Real getQuadsDamping() const { return quadsDamping.getValue(); }
    Real getCubesDamping() const { return cubesDamping.getValue(); }
    void setDamping(Real val)
    {
        linesDamping.setValue(val);
        quadsDamping.setValue(val);
        cubesDamping.setValue(val);
    }
    void setLinesDamping(Real val)
    {
        linesDamping.setValue(val);
    }
    void setQuadsDamping(Real val)
    {
        quadsDamping.setValue(val);
    }
    void setCubesDamping(Real val)
    {
        cubesDamping.setValue(val);
    }

    virtual void addForce(VecDeriv& f1, VecDeriv& f2, const VecCoord& x1, const VecCoord& x2, const VecDeriv& v1, const VecDeriv& v2);

    virtual void addDForce(VecDeriv& df1, VecDeriv& df2, const VecDeriv& dx1, const VecDeriv& dx2, double kFactor, double bFactor);

    virtual void draw();

    void setFileName(char* name)
    {
        filename.setValue( std::string(name) );
    }

    char * getFileName()const {return filename.getValue();}
};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
