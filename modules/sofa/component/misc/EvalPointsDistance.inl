#ifndef SOFA_COMPONENT_MISC_EVALPOINTSDISTANCE_INL
#define SOFA_COMPONENT_MISC_EVALPOINTSDISTANCE_INL

#include "EvalPointsDistance.h"
#include <sofa/defaulttype/DataTypeInfo.h>
#include <sofa/simulation/tree/AnimateBeginEvent.h>
#include <sofa/simulation/tree/AnimateEndEvent.h>
#include <sofa/simulation/tree/UpdateMappingEndEvent.h>
#include <sofa/helper/gl/template.h>

#include <fstream>

namespace sofa
{

namespace component
{

namespace misc
{

template<class DataTypes>
EvalPointsDistance<DataTypes>::EvalPointsDistance()
    : f_draw( initData(&f_draw, true, "draw", "activate rendering of lines between associated points"))
    , f_filename( initData(&f_filename, "filename", "output file name"))
    , f_period( initData(&f_period, 1.0, "period", "period between outputs"))
    , distMean( initData(&distMean, 1.0, "distMean", "mean distance (OUTPUT)"))
    , distMin( initData(&distMin, 1.0, "distMin", "min distance (OUTPUT)"))
    , distMax( initData(&distMax, 1.0, "distMax", "max distance (OUTPUT)"))
    , distDev( initData(&distDev, 1.0, "distDev", "distance standard deviation (OUTPUT)"))
    , mstate1(NULL)
    , mstate2(NULL)
    , outfile(NULL)
    , lastTime(0)
{
    this->f_listening.setValue(true);
}

template<class DataTypes>
EvalPointsDistance<DataTypes>::~EvalPointsDistance()
{
    if (outfile)
        delete outfile;
}

template<class DataTypes>
void EvalPointsDistance<DataTypes>::init()
{
    if (!mstate1 || !mstate2)
        mstate1 = mstate1 = dynamic_cast<core::componentmodel::behavior::MechanicalState<DataTypes>*>(this->getContext()->getMechanicalState());

    if (!mstate1 || !mstate2)
        return;

    const std::string& filename = f_filename.getValue();
    if (!filename.empty())
    {
        outfile = new std::ofstream(filename.c_str());
        if( !outfile->is_open() )
        {
            std::cerr << "Error creating file "<<filename<<std::endl;
            delete outfile;
            outfile = NULL;
        }
        else
            (*outfile) << "# time\tmean\tmin\tmax\tdev" << std::endl;
    }

}

template<class DataTypes>
void EvalPointsDistance<DataTypes>::reset()
{
    lastTime = 0;
}

template<class DataTypes>
double EvalPointsDistance<DataTypes>::eval()
{
    if (!mstate1 || !mstate2)
        return 0.0;
    const VecCoord& x1 = *mstate1->getX();
    const VecCoord& x2 = *mstate2->getX();
    return this->doEval(x1, x2);
}

template<class DataTypes>
double EvalPointsDistance<DataTypes>::doEval(const VecCoord& x1, const VecCoord& x2)
{
    const int n = (x1.size()<x2.size())?x1.size():x2.size();
    double dsum = 0;
    double dmin = 0;
    double dmax = 0;
    double d2 = 0;
    for (int i=0; i<n; ++i)
    {
        double d = (x1[i]-x2[i]).norm();
        dsum += d;
        d2 += d*d;
        if (i==0 || d < dmin) dmin = d;
        if (i==0 || d > dmax) dmax = d;
    }
    double dmean = (n>0)?dsum/n : 0.0;
    double ddev = ((n>1)?sqrtf((float)(d2/n - (dsum/n)*(dsum/n))) : 0.0);
    distMean.setValue(dmean);
    distMin.setValue(dmin);
    distMax.setValue(dmax);
    distDev.setValue(ddev);
    return dmean;
}

template<class DataTypes>
void EvalPointsDistance<DataTypes>::draw()
{
    if (!f_draw.getValue())
        return;
    if (!mstate1 || !mstate2)
        return;
    const VecCoord& x1 = *mstate1->getX();
    const VecCoord& x2 = *mstate2->getX();
    this->doDraw(x1,x2);
}

template<class DataTypes>
void EvalPointsDistance<DataTypes>::doDraw(const VecCoord& x1, const VecCoord& x2)
{
    const int n = (x1.size()<x2.size())?x1.size():x2.size();
    glDisable(GL_LIGHTING);
    glColor3f(1.0f,0.5f,0.5f);
    glBegin(GL_LINES);
    for (int i=0; i<n; ++i)
    {
        helper::gl::glVertexT(x1[i]);
        helper::gl::glVertexT(x2[i]);
    }
    glEnd();
}

template<class DataTypes>
void EvalPointsDistance<DataTypes>::handleEvent(sofa::core::objectmodel::Event* event)
{
    if (!mstate1 || !mstate2)
        return;
    std::ostream *out = (outfile==NULL)? &std::cout : outfile;
    //if (/* simulation::tree::AnimateBeginEvent* ev = */ dynamic_cast<simulation::tree::AnimateBeginEvent*>(event))
    //if (/* simulation::tree::AnimateEndEvent* ev = */ dynamic_cast<simulation::tree::AnimateEndEvent*>(event))
    if (/* simulation::tree::UpdateMappingEndEvent* ev = */ dynamic_cast<simulation::tree::UpdateMappingEndEvent*>(event))
    {
        double time = getContext()->getTime();
        // write the state using a period
        if (time+getContext()->getDt()/2 >= (lastTime + f_period.getValue()))
        {
            eval();
            (*out) << time << "\t" << distMean.getValue() << "\t" << distMin.getValue() << "\t" << distMax.getValue() << "\t" << distDev.getValue() << std::endl;
            lastTime += f_period.getValue();
        }
    }
}

} // namespace misc

} // namespace component

} // namespace sofa

#endif
