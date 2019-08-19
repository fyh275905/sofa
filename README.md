[![SOFA, Simulation Open-Framework Architecture](https://www.sofa-framework.org/wp-content/uploads/2013/01/SOFA_LOGO_ORANGE_2-normal.png)](https://www.sofa-framework.org/)

<br/>

[![Documentation](https://img.shields.io/badge/doc-on_website-green.svg)](https://www.sofa-framework.org/community/doc/)
[![Forum](https://img.shields.io/badge/support-on_forum-blue.svg)](https://www.sofa-framework.org/community/forum/)
[![Gitter](https://badges.gitter.im/sofa-framework/sofa.svg)](https://gitter.im/sofa-framework/sofa)
[![Contact](https://img.shields.io/badge/contact-on_website-orange.svg)](https://www.sofa-framework.org/consortium/contact/)  

## Introduction

SOFA is an open source framework primarily targeted at real-time simulation, 
with an emphasis on medical simulation.  
It is mainly intended for the research community to help foster newer 
algorithms, but can also be used as an efficient prototyping tool.  
SOFA's advanced software architecture allows:  
(1) the creation of complex and evolving simulations by combining new algorithms
    with existing algorithms;  
(2) the modification of key parameters of the simulation  such as deformable
    behavior, surface representation, solvers, constraints, collision algorithm,
    etc. by simply editing an XML file;  
(3) the synthesis of complex models from simpler ones using a scene-graph
    description;  
(4) the efficient simulation of the dynamics of interacting objects using
    abstract equation solvers; and  
(5) the comparison of various algorithms available in SOFA. 


## Installation

For up-to-date instructions, please refer to the SOFA documentation:  
https://www.sofa-framework.org/documentation

### CGAL Warning on MAC OSX

+ On MAC OSX systems, liniking with the cgal library packaged with homebrew or macports oftem fails.
It is best to download the tarball of the release you prefer, and manually compile in order to get the build to complete all image tests.
  - Pick your cgal release from [here](https://github.com/CGAL/cgal/releases)
  - Create a build directory within the decompressed tar folder: `cmake -DCMAKE_BUILD_TYPE=Release ..`
  - make j2
  - make install

### Installing with SoftRobots, SofaROSConnector and STLIB libraries

+ Clone this repo recursively:

   - With Git 2.13 upwards:
   ```
      git clone --recurse-submodules -j$nproc https://github.com/lakehanne/sofa --branch v19.06 
   ```

    - With Git 1.6.5 upwards:
     
     ```
        git clone --recursive git://github.com/lakehanne/sofa --branch v19.06
     ```
+ Then create a `build` folder within the root folder of this projrect. An example CMakeLists options to be built is specified in [sofa-cmake.sh](/sofa-cmake.sh). You can edit the options as you wish for components you want to build. When done, simply run the bash script from within the newly created build folder as 

```
   bash ../sofa-cmake.sh
```

## Contribution

The SOFA community will be pleased to welcome you!  
Find all the ways to get involved here: https://www.sofa-framework.org/community/get-involved/

Before creating any issue or pull request, please read carefully [our CONTRIBUTING rules](https://github.com/sofa-framework/sofa/blob/master/CONTRIBUTING.md).


## Information

### Authors
See [Authors.txt](https://github.com/sofa-framework/sofa/blob/master/Authors.txt)

### License
SOFA is LGPL, except:
- applications/projects (GPL)
- applications/sofa (GPL)
- applications/tutorials (GPL)
- applications/plugins/OptiTrackNatNet (GPL)
- applications/plugins/SofaPML (GPL)
- applications/plugins/Registration (QPL)
- sub-directories with a license file specifying a different license

LGPL refers to the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2.1 of the License, or (at your option) any later 
version.

GPL refers to the GNU General Public License as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.

### Contact information
contact@sofa-framework.org

-----------------------------------------------------------------------------

SOFA, Simulation Open-Framework Architecture  
(c) 2006-2017 INRIA, USTL, UJF, CNRS, MGH
