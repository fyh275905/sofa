The Python Scene Language for Sofa 
===========
The Python Scene Language (PSL) mixes the advantages of *XML* and *pyscn* in an unified and powerfull framework. It is also a language defined through an Abstract Semantics and it can then have multiple Concrete Semantics (JSON, H-JSON, XML, ...)

#### A small Introduction
PSL features:
- descriptive scenes (as XML)
- programable fragments (with embeded Python)
- scene templates (customisable dynamic element that can instantiated)
- libraries (for scene elements reuse and sharing)
- explicit aliasing (to simplify scene writing).
- preserve scene structure when it is loaded & saved.

To give you a taste of the language in its H-JSON flavor here is a small scene composed of two *psl* files. One os called SoftRobotActuator.psl and is a library of reusable component Template (as the PneuNet actuator). The other is the scene loaded in Sofa. Once imported, the template from the library can be instanciated in the scene. 
```css
/// The library file named SoftRobotActuators.psl
Template : {
	name : "PneuNets"
	properties : {
		numSection : 10
	}
	Node : {
		MechanicalObject : {
			name : "mstate"
			position : p"srange(0, numSection*3)"
		}
	}
}


/// The real scene.psl
Node : {
	name : "myNameIsRoot"

	Import : SoftRobotActuators
	Using : SoftRobotActuators.PneuNets-PneuNets

	Node : {
		Python : ''''
			Sofa.msg_info(myNameIsRoot, "PSL offer scene direct scene element access to python code with scoping !!!")
			for i in range(0,10):
				self.addChild("one")
				myNameIsRoot.addChild("three")
		'''
	}

	PneuNets : { 
		name:"myPneuNet" 
		numSections : 10
	}
}
```

The same scene can also be described using the XML flavor thus look like the following. 
```xml
<Node name : "myNameIsRoot">
	<Import library="SoftRobotActuators"/>
	<Using alias="SoftRobotActuators.PneuNets-PneuNets"/>

	<Node>
		<Python>
			Sofa.msg_info(myNameIsRoot, "PSL offer scene direct scene element access to python code with scoping !!!")
			for i in range(0,10):
				self.addChild("one")
				myNameIsRoot.addChild("three")
		</Python>
	</Node>

	<PneuNets name:"myPneuNet" numSections : "10"/>
</node>
```
The XML flavor is fully compatible with our classical XML scenes. 


We hope this example gave you some envy to learn more about it. Let's start with a big longer description. 

#### Installation & requirement. 
The language is under heavy developement so don't trust the code, the examples or the documentation. 
It I want it to be finished join the developement effort. 

The language is defined as a sofa Plugin named PSL which is currently it is only available in the PSL development branch. 

It makes use of the H-JSON parser available at: http://hjson.org/

Hjson installation :
```shell
git clone https://github.com/hjson/hjson-py.git
cd hjson-py
sudo python setup.py install
```

#### How to run examples

Running examples require to launch sofa from the PSL directory.

```shell
cd sofa/applications/plugins/PSL
yourBuildDirBin/runSofa examples/press_step1.pyson
```


#### Introduction. 
The language itself is defined either in term of abstract syntax or through a given concrete syntax. For the simplicity of the following we will employ the H-JSON concrete syntax as it provides both readbility, compactness and clarity. This H-JSON flavor of the language is currently implemented in Sofa but keep in mind that other alternatives are possible based on XML or YAML instead of H-JSON. 

Let's start with a simple scene example in XML
```xml
<Node name="root">
	<Node name="child1">
		<MechanicalObject name="mstate"/> 
		<OglModel filename="anObj.obj"/> 
	</Node>
</Node>
```

The equivalent scene PSL(HJSON) is the following 
```hjson
Node : {
	name : "root"
	Node : {
		name : "child1"
		MechanicalObject: { name : "mstate" }
		OglModel : { filename : "anObj.obj" }
	}
}
```

The drawback SCN files is that everything is static. This is why more and more people are using python 
to describe scene as it allows to write: 
```python
root = Sofa.createNode("root")
child1 = root.createNode("child1")
child1.createObject("MechanicalObject", name="mstate")
child1.createObject("OglModel", name="anObj.obj") 
for i in range(0,10):
	child1.createNode("child_"+str(i))
```

The equivalent scene PSL(HJSON) is the following 
```hjson
Node : {
	name : "root"
	Node : {
		name : "child1"
		MechanicalObject: { name : "mstate" }
		OglModel : { filename : "anObj.obj" }
		Python : '''
			for i in range(0, 10):
				child1.createNode("child_"+str(i))
		'''
	}
}
```

At first sight the PSL version look a bit more complex. But it solve a deep problem of the python version. It can  preserve the scene structure when it is loaded & saved. This is because in python scenes the script is executed (consumed) at loading time and is not part of the scene. The consequence is that the only possible saving is to store the *result* of the execution of the script, totally loosing the advantages of python as visible in the previous scene saved in python: 
```python
root = Sofa.createNode("root")
child1 = root.createNode("child1")
child1.createObject("MechanicalObject", name="mstate")
child1.createObject("OglModel", name="anObj.obj") 
child1.createNode("child_0")
child1.createNode("child_1")
child1.createNode("child_2")
child1.createNode("child_3")
child1.createNode("child_4")
child1.createNode("child_5")
child1.createNode("child_6")
child1.createNode("child_7")
child1.createNode("child_8")
child1.createNode("child_9")
```

With PSL, this is not a problem because the dynamic fragment are stored *un-executed* in the scene graph. They can thus be easily modifie, re-run and saved. 

#### Templates
A Template is a component that stores a sub-graph in its textual, or parsed, form. The Template then can be instantiated 
in the graph.

```hjson
Node : {
	name : "root"
	Template : {
		name : "MyTemplate"
		properties : { name : "undefined"
			       numpoints : 3 
		}
		Node : {
			name : p"aName" 
			MechanicalObject: { position=p"range(0, numparts*3)" }
			UniformMass : {}
			Node : {
				name : "visual"
				BarycentricMapping : {}
				OglModel : { filename = "myOBJ.obj"}
			}
		}
	}

	/// The template can then be instantiated using its name as in:
	MyTemplate : {
		name : "defined1"
		numpoints : 100 
	}
	
	MyTemplate : {
		name : "defined2"
		numpoints : 10 
	}
	
	/// Or using Python 
	Python : '''
		for i in range(0,10):
			instantiate(root, "MyTemplate", {name:"defined"+str(i), numpoints : i})
		'''
}
```

#### Import 
To allow template re-usability it is possible to store them in file or directories that can be imported with the Import directive. 
In a file named mylibrary.pyjson" define  a template 
```hjson
	Template : { name : "MotorA" ... }
	Template : { name : "MotorB" ... }
	Template : { name : "MotorC" .... }
```

Then in your scene file you load and use the template in the following way:
```hjson
Node : {
	Import : mylibrary 
	
	mylibrary.MotorA : {}
	mylibrary.MotorB : {}
	... 
}
```

##### Aliasing
In Sofa the aliasing system is implicit and the alias are defined in the sofa code source. This is really trouble some as users need to *discover* that in a scene "Mesh" is in fact an alias to a "MeshTopology" object. Without proper tools the solution is often to search in the source code which was an alias. 

In PSL we are preserving the use of Alias but we make them explicit. So each scene can defined its own set of alias and anyone reading the scene knows what are the alias and what are the real underlying objects. 
```hjson 
	Import : mylibrary 

	Alias : TSPhereModel-CollisionSphere
	Alias : mylibrary.MotorA-MotorA
	Alias : mylibrary.MotorB-MotorB
	
	/// Now we can use either
	TSPhereModel : {}
	
	/// or
	CollisionSphere : {}
```
