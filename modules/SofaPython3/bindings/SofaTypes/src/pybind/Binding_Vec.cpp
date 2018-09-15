#include "Binding_Vec.h"
#include <pybind11/operators.h>

#define BINDING_VEC_MAKE_NAME(N, type)                                         \
  std::string(std::string("Vec") + std::to_string(N) + typeid(type).name())

template <int N, class T> void addVec(py::class_<Vec<N, T>> &p, T type = 0) {
  typedef Vec<N, T> VecClass;
  p.def(py::init<>());

  p.def("set", [](VecClass &v, py::list l) {
    for (size_t i = 0; i < N && i < l.size(); ++i) {
      T &val = v[i];
      if (std::string(typeid(T).name()) == "i")
        val = int(l[i].cast<py::int_>());
      else
        val = double(l[i].cast<py::float_>());
    }
  });

  p.def("__getitem__",
        [](const VecClass &v, size_t i) {
          if (i >= v.size())
            throw py::index_error();
          return v[i];
        })
      .def("__setitem__", [](VecClass &v, size_t i, T d) {
        if (i >= v.size())
          throw py::index_error();
        T &val = v[i];
        val = d;
        return val;
      });
  p.def(py::self != py::self)
      .def(py::self * py::self)
      .def(py::self * float())
      .def(py::self *= float())
      .def(py::self + py::self)
      .def(py::self += py::self)
      .def(py::self - py::self)
      .def(py::self -= py::self);
  p.def("__str__", [](VecClass &v) {
    std::string s("(");
    s += std::to_string(v[0]);
    for (size_t i = 1; i < v.size(); ++i)
      s += std::string(", ") + std::to_string(v[i]);
    s += ")";
    return s;
  });
  p.def("__repr__", [type](VecClass &v) {
    std::string s = BINDING_VEC_MAKE_NAME(N, type) + "(";
    s += std::to_string(v[0]);
    for (size_t i = 1; i < v.size(); ++i)
      s += std::string(", ") + std::to_string(v[i]);
    s += ")";
    return s;
  });
}

// generic bindings for N > 12
template <int N, class T> struct VECTOR {
  static void addVec(py::module &m, T type = 0) {
    py::class_<Vec<N, T>> p(m, BINDING_VEC_MAKE_NAME(N, type).c_str());
    ::addVec(p, type);
  }
};

// Prevent calling bindings with N == 0
template <class T> struct VECTOR<0, T> {
  static void addVec(py::module &m, T type = 0) { SOFA_UNUSED(type); }
};

// specializations required for ctors / and variadic template methods
template <class T> struct VECTOR<1, T> {
  static void addVec(py::module &m, T type = 0) {
    typedef Vec<1, T> VecClass;
    py::class_<VecClass> p(m, BINDING_VEC_MAKE_NAME(1, type).c_str());
    p.def(py::init<T>());
    p.def("set", (void (VecClass::*)(T)) & VecClass::set);
    ::addVec(p, type);
  }
};

template <class T> struct VECTOR<2, T> {
  static void addVec(py::module &m, T type = 0) {
    typedef Vec<2, T> VecClass;
    py::class_<VecClass> p(m, BINDING_VEC_MAKE_NAME(2, type).c_str());
    p.def(py::init<T, T>());
    p.def("set", (void (VecClass::*)(T, T)) & VecClass::set);
    ::addVec(p, type);
  }
};

template <class T> struct VECTOR<3, T> {
  static void addVec(py::module &m, T type = 0) {
    typedef Vec<3, T> VecClass;
    py::class_<VecClass> p(m, BINDING_VEC_MAKE_NAME(3, type).c_str());
    p.def(py::init<T, T, T>());
    p.def("set", (void (VecClass::*)(T, T, T)) & VecClass::set);
    ::addVec(p, type);
  }
};

template <class T> struct VECTOR<4, T> {
  static void addVec(py::module &m, T type = 0) {
    typedef Vec<4, T> VecClass;
    py::class_<VecClass> p(m, BINDING_VEC_MAKE_NAME(4, type).c_str());
    p.def(py::init<T, T, T, T>());
    p.def("set", (void (VecClass::*)(T, T, T, T)) & VecClass::set);
    ::addVec(p, type);
  }
};

template <class T> struct VECTOR<5, T> {
  static void addVec(py::module &m, T type = 0) {
    typedef Vec<5, T> VecClass;
    py::class_<Vec<5, T>> p(m, BINDING_VEC_MAKE_NAME(5, type).c_str());
    p.def(py::init<T, T, T, T, T>());
    p.def("set", (void (VecClass::*)(T, T, T, T, T)) & VecClass::set);
    ::addVec(p, type);
  }
};

template <class T> struct VECTOR<6, T> {
  static void addVec(py::module &m, T type = 0) {
    typedef Vec<6, T> VecClass;
    py::class_<Vec<6, T>> p(m, BINDING_VEC_MAKE_NAME(6, type).c_str());
    p.def(py::init<T, T, T, T, T, T>());
    p.def("set", (void (VecClass::*)(T, T, T, T, T, T)) & VecClass::set);
    ::addVec(p, type);
  }
};

template <class T> struct VECTOR<7, T> {
  static void addVec(py::module &m, T type = 0) {
    typedef Vec<7, T> VecClass;
    py::class_<Vec<7, T>> p(m, BINDING_VEC_MAKE_NAME(7, type).c_str());
    p.def(py::init<T, T, T, T, T, T, T>());
    p.def("set", (void (VecClass::*)(T, T, T, T, T, T, T)) & VecClass::set);
    ::addVec(p, type);
  }
};

template <class T> struct VECTOR<8, T> {
  static void addVec(py::module &m, T type = 0) {
    typedef Vec<8, T> VecClass;
    py::class_<Vec<8, T>> p(m, BINDING_VEC_MAKE_NAME(8, type).c_str());
    p.def(py::init<T, T, T, T, T, T, T, T>());
    p.def("set", (void (VecClass::*)(T, T, T, T, T, T, T, T)) & VecClass::set);
    ::addVec(p, type);
  }
};

template <class T> struct VECTOR<9, T> {
  static void addVec(py::module &m, T type = 0) {
    typedef Vec<9, T> VecClass;
    py::class_<Vec<9, T>> p(m, BINDING_VEC_MAKE_NAME(9, type).c_str());
    p.def(py::init<T, T, T, T, T, T, T, T, T>());
    p.def("set",
          (void (VecClass::*)(T, T, T, T, T, T, T, T, T)) & VecClass::set);
    ::addVec(p, type);
  }
};

template <class T> struct VECTOR<12, T> {
  static void addVec(py::module &m, T type = 0) {
    typedef Vec<12, T> VecClass;
    py::class_<Vec<12, T>> p(m, BINDING_VEC_MAKE_NAME(12, type).c_str());
    p.def(py::init<T, T, T, T, T, T, T, T, T, T, T, T>());
    p.def("set", (void (VecClass::*)(T, T, T, T, T, T, T, T, T, T, T, T)) &
                     VecClass::set);
    ::addVec(p, type);
  }
};

void moduleAddVec(py::module &m) {

  VECTOR<1, int>::addVec(m);
  VECTOR<2, int>::addVec(m);
  VECTOR<3, int>::addVec(m);
  VECTOR<4, int>::addVec(m);
  VECTOR<5, int>::addVec(m);
  VECTOR<6, int>::addVec(m);
  VECTOR<7, int>::addVec(m);
  VECTOR<8, int>::addVec(m);
  VECTOR<9, int>::addVec(m);
  VECTOR<12, int>::addVec(m);
  VECTOR<1, double>::addVec(m);
  VECTOR<2, double>::addVec(m);
  VECTOR<3, double>::addVec(m);
  VECTOR<4, double>::addVec(m);
  VECTOR<5, double>::addVec(m);
  VECTOR<6, double>::addVec(m);
  VECTOR<7, double>::addVec(m);
  VECTOR<8, double>::addVec(m);
  VECTOR<9, double>::addVec(m);
  VECTOR<12, double>::addVec(m);
}
