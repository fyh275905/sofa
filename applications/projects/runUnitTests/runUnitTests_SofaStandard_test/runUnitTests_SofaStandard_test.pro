load(sofa/pre)

TEMPLATE = lib

HEADERS += \
    initSofaStandard_test.h \
    Matrix_test.inl \

SOURCES += \
    initSofaStandard_test.cpp \
    Matrix_test.cpp \
 
LIBS += -l$${BOOST_PREFIX}boost_unit_test_framework$$BOOST_SUFFIX

DEFINES += SOFA_BUILD_SofaStandard_test

load(sofa/post)


