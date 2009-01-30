#ifndef SOFA_COMPONENT_CONSTRAINT_BILATERALINTERACTIONCONSTRAINT_INL
#define SOFA_COMPONENT_CONSTRAINT_BILATERALINTERACTIONCONSTRAINT_INL

#include <sofa/component/constraint/BilateralInteractionConstraint.h>

#include <sofa/defaulttype/Vec.h>
#include <sofa/helper/gl/template.h>
namespace sofa
{

namespace component
{

namespace constraint
{

template<class DataTypes>
void BilateralInteractionConstraint<DataTypes>::init()
{
    assert(this->object1);
    assert(this->object2);
}

template<class DataTypes>
void BilateralInteractionConstraint<DataTypes>::applyConstraint(unsigned int &constraintId)
{
    int tm1, tm2;
    tm1 = m1.getValue();
    tm2 = m2.getValue();

    assert(this->object1);
    assert(this->object2);

    VecConst& c1 = *this->object1->getC();
    VecConst& c2 = *this->object2->getC();

    Coord cx(1,0,0), cy(0,1,0), cz(0,0,1);

    cid = constraintId;
    constraintId+=3;

    SparseVecDeriv svd1;
    SparseVecDeriv svd2;

    this->object1->setConstraintId(cid);
    svd1.insert(tm1, -cx);
    c1.push_back(svd1);

    this->object2->setConstraintId(cid);
    svd2.insert(tm2, cx);
    c2.push_back(svd2);

    this->object1->setConstraintId(cid+1);
    svd1.set(tm1, -cy);
    c1.push_back(svd1);

    this->object2->setConstraintId(cid+1);
    svd2.set(tm2, cy);
    c2.push_back(svd2);

    this->object1->setConstraintId(cid+2);
    svd1.set(tm1, -cz);
    c1.push_back(svd1);

    this->object2->setConstraintId(cid+2);
    svd2.set(tm2, cz);
    c2.push_back(svd2);


}

template<class DataTypes>
void BilateralInteractionConstraint<DataTypes>::getConstraintValue(defaulttype::BaseVector* v, bool freeMotion)
{
    if (!freeMotion)
        sout<<"WARNING has to be implemented for method based on non freeMotion"<<sendl;

    if (freeMotion)
        dfree = (*this->object2->getXfree())[m2.getValue()] - (*this->object1->getXfree())[m1.getValue()];
    else
        dfree = (*this->object2->getX())[m2.getValue()] - (*this->object1->getX())[m1.getValue()];

    v->set(cid, dfree[0]);
    v->set(cid+1, dfree[1]);
    v->set(cid+2, dfree[2]);
}

template<class DataTypes>
void BilateralInteractionConstraint<DataTypes>::getConstraintId(long* id, unsigned int &offset)
{
    if (!yetIntegrated)
    {
        id[offset++] = -(int)cid;

        yetIntegrated = true;
    }
    else
    {
        id[offset++] = cid;
    }
}
#ifdef SOFA_DEV
template<class DataTypes>
void BilateralInteractionConstraint<DataTypes>::getConstraintResolution(std::vector<core::componentmodel::behavior::ConstraintResolution*>& resTab, unsigned int& offset)
{
//	resTab[offset] = new BilateralConstraintResolution3Dof();
//	offset += 3;
    for(int i=0; i<3; i++)
        resTab[offset++] = new BilateralConstraintResolution();
}
#endif

template<class DataTypes>
void BilateralInteractionConstraint<DataTypes>::draw()
{
    if (!this->getContext()->getShowInteractionForceFields()) return;

    glDisable(GL_LIGHTING);
    glPointSize(10);
    glBegin(GL_POINTS);
    glColor4f(1,0,1,1);
    helper::gl::glVertexT((*this->object1->getX())[m1.getValue()]);
    helper::gl::glVertexT((*this->object2->getX())[m2.getValue()]);
    glEnd();
    glPointSize(1);
}

} // namespace constraint

} // namespace component

} // namespace sofa

#endif
