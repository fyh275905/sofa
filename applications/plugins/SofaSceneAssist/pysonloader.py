#!/usr/bin/python
# -*- coding: utf-8 -*-
#/******************************************************************************
#*       SOFA, Simulation Open-Framework Architecture, development version     *
#*                (c) 2006-2015 INRIA, USTL, UJF, CNRS, MGH                    *
#*                                                                             *
#* This library is free software; you can redistribute it and/or modify it     *
#* under the terms of the GNU Lesser General Public License as published by    *
#* the Free Software Foundation; either version 2.1 of the License, or (at     *
#* your option) any later version.                                             *
#*                                                                             *
#* This library is distributed in the hope that it will be useful, but WITHOUT *
#* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
#* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
#* for more details.                                                           *
#*                                                                             *
#* You should have received a copy of the GNU Lesser General Public License    *
#* along with this library; if not, write to the Free Software Foundation,     *
#* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
#*******************************************************************************
#*                              SOFA :: Framework                              *
#*                                                                             *
#* Contributors: damien.marchal@univ-lille1.fr Copyright (C) CNRS              *
#*                                                                             *
#* Contact information: contact@sofa-framework.org                             *
#******************************************************************************/
import Sofa
import difflib
import hjson
import os

templates = {}
sofaComponents = []
for (name, desc) in Sofa.getAvailableComponents():
	sofaComponents.append(name)

SofaStackFrame = []

sofaRoot = None
	
imports = {}


class MyObjectHook(object):
	def __call__(self, s):
		return s

#@deprecated 
def instantiate(o, kvparams):
	"""Instanciate a template.
	   All the properties provided in kvparams are added the a new frame. 
	   This Frame is added at the end of the stack frame. 
	   The o parameter is holding the template classname and its corresponding code. 
	   The template is then executed in the context provided by the stackframe.  		
	"""
	# Build the new frame and add it t the stack
	frame = {}
	for key in kvparams:
		frame[key] = kvparams[key]	
	o.stack.append(frame)
	
	# Get the template 
	n, a= o.template[0]
	n=processNode(None, n, a, o.stack, frame)
	o.stack.pop(-1)
	return n

#@deprecated 
def dumpSofa(s, prefix=""):
	res = ""
	if isinstance(s, Node):
		res += prefix+"<Node name='"+str(s.name)+">'"+ "\n"
		for c in s.objects:
			res += dumpSofa(c, prefix+"    ") + "\n"		
		for c in s.children:
			res += dumpSofa(c, prefix+"    ") + "\n"
		res += prefix+"</Node>" + "\n"
	else:
		res += prefix+str(s) + "\n"
	return res
	

def flattenStackFrame(sf):
	"""Return the stack frame content into a single "flat" dictionnary.
	   The most recent entries are overriden the oldest.
	   """
	res = {}
	for frame in sf:
		for k in frame:
			res[k] = frame[k]
	return res	

def getFromStack(name, stack):
	"""Search in the stack for a given name. The search is proceeding from 
	   the most recent entries to the oldest. If 'name' cannot be found 
	   in the stack None is returned. """
	for frame in reversed(stack):
		for k in frame: 
			if k == name:
				return frame[k]
	return None

def populateFrame(cname, frame, stack):
	"""Initialize a frame from the current attributes of the 'self' object 
	   This is needed to expose the data as first class object. 
	""" 
	fself = getFromStack("self", stack)
	if fself == None:
		return 
	#for datafield in fself.getDataFields():
	#	frame[datafield] = lambda tname : sys.out.write("T NAME") 
					     

def processPython(parent, key, kv, stack, frame):
	"""Process a python fragment of code with context provided by the content of the stack."""
	try:
		r = flattenStackFrame(stack)
		exec(kv, r)
	except Exception, e:
		Sofa.msg_error(parent, "Unable to process Python "+str(e)+str("\n")+kv)

def evalPython(key, kv, stack, frame):
	"""Process a python fragment of code with context provided by the content of the stack."""
	r = flattenStackFrame(stack)
	return eval(kv, r)


def processParameter(parent, name, value, stack, frame):
	if isinstance(value, list):
		matches = difflib.get_close_matches(name, sofaComponents+templates.keys(), n=4)
		c=parent.createChild("[XX"+name+"XX]")
    		Sofa.msg_error(c, "Unknow parameter or Component [" + name + "] suggestions -> "+str(matches))
	else:		
		## Python Hook to build an eval function. 
		if value[0] == 'p' and value[1] == '"':
			value = evalPython(None, value[2:-1], stack, frame) 
				
		try:
			frame["self"].findData(name).setValueString(str(value))
		except Exception,e:
			Sofa.msg_error(parent, "Unable to get the argument " + name)
			
		if name == "name":
			frame[value] = frame["self"] 
			frame["name"] = value
		
def createObject(parentNode, name, stack , frame, kv):
	print("CREATE OBJECT {"+name+"} WITH: "+str(kv)+ "in context "+parentNode.name) 
	if name in sofaComponents:
		n=None
		if "name" in frame:
			n = parentNode.createObject(name, **kv)
		else:
			n = parentNode.createObject(name, **kv)	
		return n
		
	failureObject = parentNode.createObject("Undefined", **kv)
	Sofa.msg_error(failureObject, "Unable to create object "+str(key)) 
	return failureObject
	
def processObjectDict(obj, dic, stack, frame):
	for key,value in dic:
		if key == "Python":				
			processPython(obj, key, value, stack, frame)
		else:
			processParameter(obj, key, value, stack ,frame)
								
def processObject(parent, key, kv, stack, frame):
    try:
	global sofaComponents 
	populateFrame(key, frame, stack)
	frame = {}
	kwargs = {}
	if not isinstance(kv, list):
		kv = [("name" , kv)]
		
	for k,v in kv:
		if v[0] == 'p' and v[1] == '"':
			v = evalPython(None, v[2:-1], stack, frame) 
		
		if k == "name":	
			frame["name"] = v
		
		kwargs[k] = str(v)

	stack.append(frame)
	frame["self"] = obj = createObject(parent, key, stack, frame, kwargs)
	stack.pop(-1) 	
	
	if key == "RequiredPlugin" : 
		sofaComponents = []
		for (name, desc) in Sofa.getAvailableComponents():
			sofaComponents.append(name)	
	
	return obj
    except Exception:
    	c=parent.createChild("[XX"+key+"XX]")
    	Sofa.msg_error(c, "Problem in creating the Object")
   	return None
   		
# TODO add a warning to indicate that a template is loaded twice.
def importTemplates(content):
	templates = {}
	for key, value in content:
		if key == "Template":
			name = "undefined"
			properties = {}
			rvalue = []
			for k,v in value:
				if k == "name":
					name = str(v)
				elif k == "properties":
					properties = v
				else:
					rvalue.append((k, v))
			templates[name] = {"properties":properties, "content" : rvalue}
		else:
			Sofa.msg_warning("SceneLoaderPYSON", "An imported file contains something that is not a Template.")

	return templates 

# TODO gérer les imports circulaires...
def processImport(parent, key, kv, stack, frame):
	global imports, templates
	if not (isinstance(kv, str) or isinstance(kv, unicode)):
		print("Expecting a single 'string' entry....in procesImport " + str(type(kv)))
		return 
	filename = kv+".pyson"
	if not os.path.exists(filename):
		dircontent = os.listdir(os.getcwd())
		matches = difflib.get_close_matches(filename, dircontent, n=4)
		Sofa.msg_error(parent, "The file '" + filename + "' does not exists. Do you mean: "+str(matches))
		return 
	Sofa.msg_info(parent, "Importing "+ os.getcwd() + "/"+filename)
		
	f = open(filename).read()
	loadedcontent = hjson.loads(f, object_pairs_hook=MyObjectHook())
	imports[filename] = importTemplates(loadedcontent)
	print("IMPORTED TEMPLATE: " + str(imports[filename].keys())) 

	for tname in imports[filename].keys():
		templates[kv+"."+tname] = imports[filename][tname]	
	print("TEMPLATES: "+str(templates))

def processTemplate(parent, key, kv, stack, frame):
	global templates 
	name = "undefined"
	properties = {}
	pattern = [] 
	for key,value in kv:
		if key == "name":
			name = value
		elif key == "properties":
			properties = value
		else:
			pattern.append( (key, value) ) 
	o = parent.createObject("Template", name=str(name))
	o.listening = True 
	o.setTemplate(value)
	frame[str(name)] = o
	templates[str(name)] = o  
	return o

aliases = {}
def processAlias(parent, key, kv, stack, frame):
	global aliases	
	oldName, newName = kv.split('-')
	aliases[newName]=oldName

def reinstanciateTemplate(templateInstance):
	global templates

	key = templateInstance.name 
	instanceProperties = eval(templateInstance.src)
	print("RE-Instanciate template: "+ templateInstance.name )
	print("             properties: "+ str(instanceProperties) )
	
	
	#print("TODO: "+str(dir(templateInstance)))
	for c in templateInstance.getChildren():
		templateInstance.removeChild(c)
	
	c = templateInstance.getObjects()
	for o in c:
		templateInstance.removeObject(o)
	
	# Is there a template with this name, if this is the case 
	# Retrieve the associated templates .
	if isinstance(templates[key], Sofa.Template):
		n = templates[key].getTemplate()
		print("L0 ? ") 
	else: 
		print("LA")
		n = templates[key]["content"] 
		for k,v in templates[key]["properties"]:
			if not k in frame:
				frame[k] = str(v)
	print("?? "+str(templates))		
	print("Template: "+str(n))
	frame = {}
	frame["parent"]=templateInstance
	for k,v in instanceProperties:
		frame[k] = v
	stack = [frame]
	n = processNode(templateInstance, "Node", n, stack, frame, doCreate=True)
	#n.name = key
	

def instanciateTemplate(parent, key, kv, stack, frame):
	global templates
	print("Instanciate template: "+key + "-> "+str(kv))
	for k,v in kv:
		frame[k] = v
	n=None
	if isinstance(templates[key], Sofa.Template):
		n = templates[key].getTemplate()
	else: 
		n = templates[key]["content"] 
		for k,v in templates[key]["properties"]:
			if not k in frame:
				frame[k] = str(v)
	print("Template: "+str(n))
	n = processNode(parent, "Node", n, stack, frame, doCreate=True)
	n.name = key
	

	for k,v in kv:
		if not hasattr(n, k):
			print("ADDING NEW ATTRIBUTE "+str(n))
			if isinstance(v, int):
				n.addData(k, key+".Properties", "Help", "d", v)
			elif isinstance(v, str):
				n.addData(k, key+".Properties", "Help", "s", v)
			elif isinstance(v, float):
				n.addData(k, key+".Properties", "Help", "f", v)
			data = n.findData(k) 
			templates[key].trackData(data) 

	n.addData("src", key+".Properties", "No help", "s", repr(kv))
	
	
def processNode(parent, key, kv, stack, frame, doCreate=True):
	global templates, aliases
	#print("PN:"+ parent.name + " : " + key + " stack frame is: "+str(stack))
	stack.append(frame)
	populateFrame(key, frame, stack)
	if doCreate:
		tself = frame["self"] = parent.createChild("undefined")
	else:
		tself = parent	
	if isinstance(kv, list):
		for key,value in kv:
			if isinstance(key, unicode):
				key = str(key)

			if key in aliases:
				print("Alias resolution to: "+aliases[key])
				key = aliases[key]

			if key == "Import":
				n = processImport(tself, key, value, stack, {})		
			elif key == "Node":
				n = processNode(tself, key, value, stack, {})	
			elif key == "Python":	
				processPython(tself, key, value, stack, {})
			elif key == "Template":
				tself.addObject( processTemplate(tself, key, value, stack, {}) )
			elif key == "Using":
				processAlias(tself, key,value, stack, frame)			
			elif key in sofaComponents:
				o = processObject(tself, key, value, stack, {})
				if o != None:
					tself.addObject(o)
			elif key in templates:
				instanciateTemplate(tself, key,value, stack, frame)
			else:
				processParameter(tself, key, value, stack, frame)
	else:
		print("LEAF: "+kv)
	stack.pop(-1)
	return tself


def processTree(parent, key, kv):
	stack = []
	frame = {}
	if isinstance(kv, list):
		for key,value in kv:
			print("PROCESSING:" + str(key))
			if key == "Import":
				print("Importing: "+value+".pyjson")
			elif key == "Node":
				processNode(parent, key, value, stack, globals())
			elif key == "Python":	
				processPython(parent, key, value, stack, globals())
			elif key in sofaComponents:
				processObject(parent, key, value, stack, globals())
			else:
				processParameter(parent, key, value, stack, frame)
	else:
		print("LEAF: "+kv)


def saveTree(rootNode, space):
	print(space+"Node : {")
	nspace=space+"    "
	for child in rootNode.getChildren():
		saveTree(child, nspace)
		
	for obj in rootNode.getObjects():
		print(nspace+obj.getClassName() + " : { " )
		print(nspace+"    name : "+str(obj.name)) 
		print(nspace+" } ")	
		
	print(space+"}")
	
def save(rootNode, filename):
	print("PYSCIN SAVE: "+str(filename))
	saveTree(rootNode,"")

def load(rootNode, filename):
	global sofaRoot
	sofaRoot = rootNode
	filename = os.path.abspath(filename)
	dirname = os.path.dirname(filename) 

	print("PYSCIN LOAD: "+str(filename))
	print("PYSCIN ROOT: "+str(dirname))

	olddirname = os.getcwd()
	os.chdir(dirname)	

	f = open(filename).read()
	r = processTree(sofaRoot, "", hjson.loads(f, object_pairs_hook=MyObjectHook()))

	os.chdir(olddirname)	
	return r
