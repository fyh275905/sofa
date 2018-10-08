#ifndef PYTHONMODULE_SOFA_BINDING_PYTHONCONTROLLER_H
#define PYTHONMODULE_SOFA_BINDING_PYTHONCONTROLLER_H

#include "Binding_BaseObject.h"

#include <sofa/core/behavior/BaseController.h>

template class pybind11::class_<sofa::core::behavior::BaseController,
                          sofa::core::objectmodel::BaseObject,
                          sofa::core::sptr<sofa::core::behavior::BaseController>>;


namespace sofapython3
{
using sofa::core::behavior::BaseController;

class PythonController : public BaseController
{
public:
    SOFA_CLASS(PythonController, BaseController);
    void init() override ;
    void reinit() override;

    PythonController()
    {
        std::cout << "PythonController() at "<<(void*)this << std::endl;
    }

    ~PythonController()
    {
        std::cout << "~PythonController()" << std::endl;
    }

};

template <typename T> class py_shared_ptr : public sofa::core::sptr<T>
{
public:
    py_shared_ptr(T *ptr) ;
};


void moduleAddPythonController(py::module &m);

} /// namespace sofapython3

#endif /// PYTHONMODULE_SOFA_BINDING_PYTHONCONTROLLER_H
