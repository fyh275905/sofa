
/// CREATING A NEW PYTHON MODULE: _Compliant
///
/// @author Matthieu Nesme
/// @date 2016


#include <SofaPython/PythonMacros.h>
#include "Binding_AssembledSystem.h"

#include <sofa/helper/cast.h>
#include <sofa/simulation/Simulation.h>
#include "../assembly/AssemblyVisitor.h"



using namespace sofa::core;
using namespace sofa::core::objectmodel;
using namespace sofa::simulation;
using namespace sofa::component::linearsolver;



/// args are node + factors m,b,k to return the linear combinaison mM+bB+kK
extern "C" PyObject * _Compliant_getAssembledImplicitMatrix(PyObject * /*self*/, PyObject * args)
{
    PyObject* pyNode;

    float M,B,K;
    if (!PyArg_ParseTuple(args, "Offf", &pyNode, &M, &B, &K))
    {
        SP_MESSAGE_ERROR( "_Compliant_getAssembledImplicitMatrix: wrong arguments" );
        PyErr_BadArgument();
        Py_RETURN_NONE;
    }

    BaseNode* node=((PySPtr<Base>*)pyNode)->object->toBaseNode();
    if (!node)
    {
        SP_MESSAGE_ERROR( "_Compliant_getAssembledImplicitMatrix: first argument is not a BaseNode" );
        PyErr_BadArgument();
        Py_RETURN_NONE;
    }


//    SP_MESSAGE_INFO( "_Compliant_getAssembledImplicitMatrix: "<<M<<" "<<B<<" "<<K );

    MechanicalParams mparams = *MechanicalParams::defaultInstance();
    mparams.setMFactor( M );
    mparams.setBFactor( B );
    mparams.setKFactor( K );
    AssemblyVisitor assemblyVisitor(&mparams);
    node->getContext()->executeVisitor( &assemblyVisitor );
    AssembledSystem sys;
    assemblyVisitor.assemble(sys); // assemble system


//    SP_MESSAGE_INFO( "_ompliant_getAssembledImplicitMatrix: "<<sys.H );


    // todo returns a sparse matrix

    size_t size = sys.H.rows();

    PyObject* H = PyList_New(size);
    for( size_t row=0 ; row<size ; ++row )
    {
        PyObject* rowpython = PyList_New(size);

        for( size_t col=0 ; col<size ; ++col )
            PyList_SetItem( rowpython, col, PyFloat_FromDouble( sys.H.coeff(row,col) ) );

        PyList_SetItem( H, row, rowpython );
    }


    return H;
}




extern "C" PyObject * _Compliant_getImplicitAssembledSystem(PyObject * /*self*/, PyObject * args)
{
    PyObject* pyNode;
    if (!PyArg_ParseTuple(args, "O", &pyNode))
    {
        SP_MESSAGE_ERROR( "_Compliant_getAssembledImplicitMatrix: wrong arguments" );
        PyErr_BadArgument();
        Py_RETURN_NONE;
    }

    sofa::core::objectmodel::BaseNode* node=((PySPtr<sofa::core::objectmodel::Base>*)pyNode)->object->toBaseNode();
    if (!node)
    {
        SP_MESSAGE_ERROR( "_Compliant_getAssembledImplicitMatrix: first argument is not a BaseNode" );
        PyErr_BadArgument();
        Py_RETURN_NONE;
    }

    SReal dt = down_cast<Node>(node)->getDt();
    MechanicalParams mparams = *MechanicalParams::defaultInstance();

    // pure implicit coeff, TODO: parametrize these?
    mparams.setMFactor( 1.0 );
    mparams.setBFactor( -dt );
    mparams.setKFactor( -dt*dt );
    mparams.setDt( dt );

    AssemblyVisitor assemblyVisitor(&mparams);
    node->getContext()->executeVisitor( &assemblyVisitor );
    AssembledSystem* sys = new AssembledSystem();
    assemblyVisitor.assemble(*sys); // assemble system

    return SP_BUILD_PYPTR(AssembledSystem,AssembledSystem,sys,true);
}


// Methods of the module
SP_MODULE_METHODS_BEGIN(_Compliant)
SP_MODULE_METHOD(_Compliant,getAssembledImplicitMatrix)
SP_MODULE_METHOD(_Compliant,getImplicitAssembledSystem)
SP_MODULE_METHODS_END

