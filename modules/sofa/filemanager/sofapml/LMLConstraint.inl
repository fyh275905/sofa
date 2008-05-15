#ifndef LMLCONSTRAINT_INL
#define LMLCONSTRAINT_INL

#include "sofa/core/componentmodel/behavior/Constraint.inl"
#include "LMLConstraint.h"
#include "sofa/helper/gl/template.h"

namespace sofa
{

namespace filemanager
{

namespace pml
{


template<class DataTypes>
LMLConstraint<DataTypes>::LMLConstraint(Loads* loadsList, const map<unsigned int, unsigned int> &atomIndexToDOFIndex, MechanicalState<DataTypes> *mm)
    : Constraint<DataTypes>(mm), atomToDOFIndexes(atomIndexToDOFIndex)
{
    mmodel = mm;
    loads = new Loads();
    Load * load;
    SReal dirX, dirY, dirZ;
    this->setName("loads");

    //for each load, we search which ones are translations applied on the body nodes
    for (unsigned int i=0 ; i<loadsList->numberOfLoads() ; i++)
    {
        load = loadsList->getLoad(i);
        if (load->getType() == "Translation")
        {
            if (load->getDirection().isToward())
            {
                std::map<unsigned int, unsigned int>::const_iterator titi = atomIndexToDOFIndex.find(load->getDirection().getToward());
                if (titi != atomIndexToDOFIndex.end())
                {
                    unsigned int dofInd = titi->second;
                    dirX = (*mm->getX())[dofInd].x();
                    dirY = (*mm->getX())[dofInd].y();
                    dirZ = (*mm->getX())[dofInd].z();
                }
            }
            else
                load->getDirection(dirX, dirY, dirZ);
            unsigned int cpt=0;
            for (unsigned int j=0 ; j<load->numberOfTargets(); j++)
            {
                std::map<unsigned int, unsigned int>::const_iterator result = atomIndexToDOFIndex.find(load->getTarget(j));
                if (result != atomIndexToDOFIndex.end())
                {
                    cpt++;
                    if (load->getDirection().isToward())
                        addConstraint(result->second, Deriv(dirX-(*mm->getX())[result->second].x(),dirY-(*mm->getX())[result->second].y(),dirZ-(*mm->getX())[result->second].z()) );
                    else
                        addConstraint(result->second, Deriv(dirX,dirY,dirZ) );
                    if (load->getDirection().isXNull() && load->getValue(0) != 0) // fix targets on the X axe
                        fixDOF(result->second, 0);
                    if (load->getDirection().isYNull() && load->getValue(0) != 0) // fix targets on the Y axe
                        fixDOF(result->second, 1);
                    if (load->getDirection().isZNull() && load->getValue(0) != 0) // fix targets on the Z axe
                        fixDOF(result->second, 2);
                }
            }
            if (cpt > 0)
                loads->addLoad(load);
        }
    }
}


template<class DataTypes>
LMLConstraint<DataTypes>*  LMLConstraint<DataTypes>::addConstraint(unsigned int index, Deriv trans)
{
    this->targets.push_back(index);
    trans.normalize();
    this->translations.push_back(trans);
    this->directionsNULLs.push_back(Deriv(1,1,1));
    this->initPos.push_back(Deriv(0,0,0));
    return this;
}

template<class DataTypes>
LMLConstraint<DataTypes>*  LMLConstraint<DataTypes>::removeConstraint(int index)
{
    std::vector<unsigned int>::iterator it1=targets.begin();
    VecDerivIterator it2=translations.begin();
    VecDerivIterator it3=directionsNULLs.begin();
    while(it1 != targets.end() && *it1!=(unsigned)index)
    {
        it1++;
        it2++;
        it3++;
    }

    targets.erase(it1);
    translations.erase(it2);
    directionsNULLs.erase(it3);

    return this;
}


template<class DataTypes>
void LMLConstraint<DataTypes>::fixDOF(int index, int axe)
{
    //set the value to 1 on the corrects vector component
    std::vector<unsigned int>::iterator it1=targets.begin();
    VecDerivIterator it2 = directionsNULLs.begin();
    while(it1 != targets.end() && *it1!=(unsigned)index)
    {
        it1++;
        it2++;
    }

    (*it2)[axe] = 0;
}


template<class DataTypes>
void LMLConstraint<DataTypes>::projectResponse(VecDeriv& dx)
{
    //VecCoord& x = *this->mmodel->getX();
    //dx.resize(x.size());
    SReal time = this->getContext()->getTime();
    SReal prevTime = time - this->getContext()->getDt();

    std::vector<unsigned int>::iterator it1=targets.begin();
    VecDerivIterator it2=translations.begin();
    VecDerivIterator it3=directionsNULLs.begin();
    Load * load;
    SReal valTime, prevValTime;

    for (unsigned int i=0 ; i<loads->numberOfLoads() ; i++)
    {
        load = loads->getLoad(i);
        valTime = load->getValue(time);
        prevValTime = load->getValue(prevTime);
        for(unsigned int j=0 ; j<load->numberOfTargets(); j++)
        {
            if ( atomToDOFIndexes.find(load->getTarget(j)) != atomToDOFIndexes.end() )
            {
                //Deriv dirVec(0,0,0);
                if (load->getDirection().isXNull() && valTime != 0)
                    (*it3)[0]=0;	// fix targets on the X axe
                if (load->getDirection().isYNull() && valTime != 0)
                    (*it3)[1]=0;	// fix targets on the Y axe
                if (load->getDirection().isZNull() && valTime != 0)
                    (*it3)[2]=0;	// fix targets on the Z axe

                if (valTime == 0)
                    (*it3) = Deriv(1,1,1);
                else
                {
                    if (load->getDirection().isToward())
                    {
                        std::map<unsigned int, unsigned int>::const_iterator titi = atomToDOFIndexes.find(load->getDirection().getToward());
                        if (titi != atomToDOFIndexes.end())
                        {
                            (*it2) = (*mmodel->getX())[titi->second] - (*mmodel->getX())[*it1];
                            it2->normalize();
                        }
                    }
                    //cancel the dx value on the axes fixed (where directionNULLs value is equal to 1)
                    //Or apply the translation vector (where directionNULLs value is equal to 0)
                    dx[*it1][0] = ((*it2)[0]*valTime)-((*it2)[0]*prevValTime)*(*it3)[0];
                    dx[*it1][1] = ((*it2)[1]*valTime)-((*it2)[1]*prevValTime)*(*it3)[1];
                    dx[*it1][2] = ((*it2)[2]*valTime)-((*it2)[2]*prevValTime)*(*it3)[2];
                }

                it1++;
                it2++;
                it3++;
            }
        }
    }
}

template<class DataTypes>
void LMLConstraint<DataTypes>::projectPosition(VecCoord& x)
{
    SReal time = getContext()->getTime();

    std::vector<unsigned int>::iterator it1=targets.begin();
    VecDerivIterator it2=translations.begin();
    VecDerivIterator it3=initPos.begin();
    Load * load;

    for (unsigned int i=0 ; i<loads->numberOfLoads() ; i++)
    {
        load = loads->getLoad(i);
        for(unsigned int j=0 ; j<load->numberOfTargets(); j++)
        {
            if ( atomToDOFIndexes.find(load->getTarget(j)) != atomToDOFIndexes.end() )
            {
                if ( (*it3)[0]==0.0 && (*it3)[1]==0.0 && (*it3)[2]==0.0)
                    *it3 = x[*it1];
                if(  load->getDirection().isXNull() && load->getDirection().isYNull() && load->getDirection().isZNull() )
                    *it3 = x[*it1];

                if (load->getValue(time) != 0.0)
                {
                    if (load->getDirection().isToward())
                    {
                        std::map<unsigned int, unsigned int>::const_iterator titi = atomToDOFIndexes.find(load->getDirection().getToward());
                        if (titi != atomToDOFIndexes.end())
                        {
                            (*it2) = (*mmodel->getX())[titi->second] - (*mmodel->getX())[*it1];
                            it2->normalize();
                        }
                    }
                    x[*it1] = (*it3) + (*it2)*load->getValue(time);
                }

                it1++;
                it2++;
                it3++;
            }
        }
    }
}



// -- VisualModel interface
template<class DataTypes>
void LMLConstraint<DataTypes>::draw()
{

    if (!getContext()->getShowBehaviorModels()) return;

    VecCoord& x = *mmodel->getX();
    glDisable (GL_LIGHTING);
    glColor4f (1,0.5,0.5,1);

    glPointSize(10);

    //for Fixed points, display a big red point
    glBegin (GL_POINTS);
    VecDerivIterator it2 = directionsNULLs.begin();
    for (std::vector<unsigned int>::const_iterator it = this->targets.begin(); it != this->targets.end(); ++it)
    {
        if ((*it2)[0]==0 && (*it2)[1]==0 && (*it2)[2]==0 )
            helper::gl::glVertexT(x[*it]);
        it2++;
    }
    glEnd();

    //for translated points, display a little red segment with translation direction
    glPointSize(1);
    glBegin( GL_LINES );
    VecDerivIterator it3 = translations.begin();
    it2 = directionsNULLs.begin();
    for (std::vector<unsigned int>::const_iterator it = this->targets.begin(); it != this->targets.end(); ++it)
    {
        if ((*it2)[0]==1 || (*it2)[1]==1 || (*it2)[2]==1 )
        {
            helper::gl::glVertexT(x[*it]);
            helper::gl::glVertexT(x[*it]+*it3);
        }
        it3++;
        it2++;
    }
    glEnd();


}

}
}
}

#endif //LMLCONSTRAINT_INL
