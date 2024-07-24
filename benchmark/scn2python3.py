#!/usr/bin/python

"""
For more information on this program, please type
python createPythonScene.py -h
or
./createPythonScene.py -h
"""
import re
import xml.etree.ElementTree as ET
from subprocess import check_output
import argparse
import sys
import os
import subprocess

def stringToVariableName(s):
    ### converting a string in a valid variable name
    # replace invalid characters
    s = re.sub('[^0-9a-zA-Z_]', '_', s)
    # replace leading characters until we find a letter or underscore
    s = re.sub('^[^a-zA-Z_]+', '_', s)
    return s

def chopStringAtChar(stringToChop,char,useContentBeforeChar=0) :
    choppedString = stringToChop
    choppedString_re = re.search(char,stringToChop[::-1])
    if choppedString_re is not None :
        posChar = choppedString_re.start()
        choppedString = stringToChop[len(stringToChop)-posChar:]
        if useContentBeforeChar :
            choppedString = stringToChop[:-choppedString_re.end()]
    return choppedString;

def getFilenameWithoutPathAndFilenameEndings(filename) :
    filenameWithoutPath = chopStringAtChar(filename,'/')
    filenameWithoutPathAndFilenameEndings = chopStringAtChar(filenameWithoutPath,'\.',useContentBeforeChar=1)
    return filenameWithoutPathAndFilenameEndings;

def getAbsolutePath(filename) :
    filenameAbsolutePath = str(check_output(['pwd'])[:-1]) + "/" + filename
    if filename[0] == '/' :
        filenameAbsolutePath = filename
    return filenameAbsolutePath;

def attributesToStringPython(child,printName) :
    attribute_str = str()
    for item in child.items() :
        if (not (item[0] == 'name') ) or printName :
            if not (item[1] == '@') :
                attribute_str += ", " + item[0] + "=\'" + item[1] + "\'"
            else :
                link = raw_input("The link of '"+child.tag+"' for the attribute '"+item[0]+"' does not point to a component, which link would you like to set? Please name the link without @, i.e. for @loader write loader: \n")
                attribute_str += ", " + item[0] + "=\'@" + link + "\'"
    return attribute_str;

def rootAttributesToStringPython(root,tabs) :
    attribute_str = str()
    for item in root.items() :
        if (not (item[0] == 'name') ) and (not (item[0] == 'showBoundingTree') ) :
            attribute_str += tabs+"rootNode.findData(\'" + item[0] + "\').value = \'" + item[1] + "\'\n"
    return attribute_str;

def childAttributesToStringPython(child,childName,tabs) :
    attribute_str = str()
    for item in child.items() :
        if (not (item[0] == 'name') ):
            attribute_str += tabs+stringToVariableName(childName)+"." + item[0] + " = \'" + item[1] + "\'\n"
    return attribute_str;

def attributesToStringXML(child) :
    attribute_str = str()
    for item in child.items() :
        attribute_str += " " + item[0] + "=\"" + item[1] + "\""
    return attribute_str;

def createObject(child) :
    createObject_str = "addObject(\'" + child.tag + "\'" + attributesToStringPython(child,1) +")"
    return createObject_str;

def createChild(childName) :
    createChild_str = "addChild(\'" + childName + "\'" +")"
    return createChild_str;

def getNodeName(node,numberOfUnnamedNodes) :
    nodeName = node.get('name')
    if nodeName is None :
        nodeName = 'unnamedNode_'+str(numberOfUnnamedNodes)
        node.set('name',nodeName)
        print("\nWARNING: unnamed node in input scene, used name " + nodeName)
        numberOfUnnamedNodes += 1
    return nodeName,numberOfUnnamedNodes

def printChildren(parent, tabs, numberOfUnnamedNodes, scenePath='rootNode', nodeIsRootNode=0, parentName='rootNode') :
    if not nodeIsRootNode :
        parentName = parent.get('name')
    parentVariableName = stringToVariableName(parentName)
    myChildren = str()
    for child in parent :
        if child.tag == "Node" :
            childName, numberOfUnnamedNodes = getNodeName(child,numberOfUnnamedNodes)
            currentScenePath = scenePath+"/"+childName
            currentChild  = "\n"+tabs+"# "+currentScenePath+"\n"
            currentChild += tabs+stringToVariableName(childName)+" = "+parentVariableName+"."+createChild(childName)+"\n"
            currentChild += tabs+stringToVariableName(childName)+" = "+stringToVariableName(childName)+"\n"
            currentChild += childAttributesToStringPython(child,childName,tabs)
            print (currentChild[:-1])
            myChildren += currentChild + printChildren(child,tabs,numberOfUnnamedNodes,scenePath=currentScenePath)
        else :
            if not child.tag == "include" :
                currentChild = tabs+parentVariableName+"."+createObject(child)+"\n"
                print (currentChild[:-1])
                myChildren += currentChild
            else :
                href = ""
                for item in child.items() :
                    if item[0] == "href" :
                        href = item[1]
                inputFilename = check_output(["locate",href])[:-1]
                tree = ET.parse(inputFilename)
                root = tree.getroot()
                fromExternalFile = printChildren(root,tabs,numberOfUnnamedNodes,scenePath=scenePath,nodeIsRootNode=1,parentName=parentVariableName)
                myChildren += fromExternalFile
                # print "WARNING: Included external file, please check the links starting from \n"+fromExternalFile[:100]+"\n... until ...\n"+fromExternalFile[-100:]
    return myChildren;

def getElement (node,name) :
    for childId in range(len(node)) :
        curChild = node[childId]
        if curChild.get('name') == name :
            return node, curChild, childId
        if curChild.tag == 'Node' :
            resultParent,resultChild,resultChildId = getElement (curChild,name)
            if resultParent is not None :
                return resultParent,resultChild,resultChildId
    return None,None,None

def indent(elem, level=0):
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

def parseInput() :
    # for more information on the parser go to
    # https://docs.python.org/2/library/argparse.html#module-argparse
    parser = argparse.ArgumentParser(
        description='Script to transform a Sofa scene from xml to python',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,# ArgumentDefaultsHelpFormatter
        epilog='''The output of this script uses the python plugin of sofa. The python plugin allows for a manipulation of a scene at run time. More informations about the plugin itself can be found in sofa/applications/plugins/SofaPython/doc/SofaPython.pdf. If you prefer to only produce one output file O.py (instead of producing two ouputfiles O.scn and O.py), then set the flag --py. To be able to run a scene O.py, the sofa python plugin has to be added in the sofa plugin manager, i.e. add the sofa python plugin in runSofa->Edit->PluginManager. Author of createPythonScene.py: Christoph PAULUS, christoph.paulus@inria.fr''')
    parser.add_argument('inputScenes', metavar='I', type=str, nargs='+',help='Filename(s) of the standard scene(s)')
    parser.add_argument('-n', nargs='?', help='Node to replace by python script, if equals None the complete scene is replaced by a python script')
    parser.add_argument('-o', nargs='*', help='Filename(s) of the transformed scene(s), if equals None the script generates the output filename(s) by adding Python to the input filenames')
    parser.add_argument('-s', dest='onlyOutputPythonScript', action='store_const', default=0, const=1, help='Output .scn and .py file')
    args = parser.parse_args()
    return parser,args;

def getFilename() :
    filename = __file__
    if filename[0] == "." :
        filename = subprocess.getstatusoutput('pwd')[1]+filename[1:]
    else :
        filename = subprocess.getstatusoutput('pwd')[1]+"/"+filename
    return filename

def writePythonFile(info_str,classNamePythonFile,node,outputFilenamePython,produceSceneAndPythonFile=1,nodeIsRootNode=1) :
    tabs = "    "

    # introduce parameter that counts the number of unnamed nodes
    numberOfUnnamedNodes = 0

    # construct a string, with all the python commands
    pythonFile_str = "\"\"\"\n"
    pythonFile_str += info_str
    pythonFile_str += "\"\"\"\n\n"
    pythonFile_str += "import sys\n"
    pythonFile_str += "import Sofa\n\n"
    
    pythonFile_str += "def createScene(rootNode):\n\n"
    print ("\n\nThe function createScene contains \n\n")
    if nodeIsRootNode :
        pythonFile_str += tabs+"# rootNode\n"
        pythonFile_str += printChildren(node,tabs,numberOfUnnamedNodes,nodeIsRootNode=1)
    else :
        pythonFile_str += tabs+"# "+classNamePythonFile+"\n"
        pythonFile_str += printChildren(node,tabs,numberOfUnnamedNodes,classNamePythonFile,nodeIsRootNode=1)
    pythonFile_str += "\n"+tabs+"return 0;\n\n"

    pythonFile_str += "def main():\n"
    pythonFile_str += tabs+"import SofaRuntime\n"
    pythonFile_str += tabs+"import Sofa.Gui\n\n"
    pythonFile_str += tabs+"root = Sofa.Core.Node('root')\n"
    pythonFile_str += tabs+"createScene(root)\n"
    pythonFile_str += tabs+"Sofa.Simulation.init(root)\n\n"
    pythonFile_str += tabs+"Sofa.Gui.GUIManager.Init('myscene', 'qglviewer')\n"
    pythonFile_str += tabs+"Sofa.Gui.GUIManager.createGUI(root, __file__)\n"
    pythonFile_str += tabs+"Sofa.Gui.GUIManager.SetDimension(1080, 1080)\n"
    pythonFile_str += tabs+"Sofa.Gui.GUIManager.MainLoop(root)\n"
    pythonFile_str += tabs+"Sofa.Gui.GUIManager.closeGUI()\n\n\n"
    pythonFile_str += "# Function used only if this script is called from a python environment\n"
    pythonFile_str += "if __name__ == '__main__':\n"
    pythonFile_str += tabs+"main()\n"

    # write python file
    f_py = open(outputFilenamePython,'w')
    f_py.write(pythonFile_str)
    f_py.close()

def transformXMLSceneToPythonScene(inputScene,produceSceneAndPythonFile,outputFilename,nodeToPythonScript) :
    # get the correct input filename
    pythonFilename = getFilename()
    # get the correct names for the output files
    pythonFilenameWithoutPath = chopStringAtChar(pythonFilename,'/')
    outputFilenameWithoutPath = chopStringAtChar(outputFilename,'/')
    classNamePythonFile = getFilenameWithoutPathAndFilenameEndings(inputScene)

    # get absolute paths
    inputSceneWithAbsPath = getAbsolutePath(inputScene)
    outputFilenameWithAbsPath = getAbsolutePath(outputFilename)

    # string with a few informations on the file
    info_str = outputFilenameWithoutPath + "\n"
    info_str += "is based on the scene \n"
    info_str += inputSceneWithAbsPath + "\n"
    info_str += "but it uses the SofaPython3 plugin. \n\n"
    info_str += "The current file has been written by the python script\n"
    info_str += pythonFilename + "\n"

    # load the xml file into the internal values
    tree = ET.parse(inputScene)
    root = tree.getroot()
    if produceSceneAndPythonFile :
        if nodeToPythonScript is not None :
            # change the xmltree - only a node has been replaced by a python script
            parent,node,nodeId = getElement(root,nodeToPythonScript)
            if node == None :
                print ("ERROR: node "+nodeToPythonScript+" is not part of "+inputScene)
                sys.exit()
            print ("Information: replacing a node by a python script, may result in broken links in the scene")
            parent.remove(node)
            outputPythonFilename = outputFilename+nodeToPythonScript+".py"
            outputPythonFilenameWithoutPath = outputFilenameWithoutPath+nodeToPythonScript+".py"
            pythonObject = ET.Element('PythonScriptController',attrib=dict(name=nodeToPythonScript,listening="1",filename=outputPythonFilenameWithoutPath,classname=nodeToPythonScript))
            parent.insert(nodeId,pythonObject)
            writePythonFile(info_str,nodeToPythonScript,node,outputPythonFilename,nodeIsRootNode=0)
        else :
            # write a xml tree to lance the python script
            outputPythonFilename = outputFilename+".py"
            writePythonFile(info_str,classNamePythonFile,root,outputPythonFilename)
            childrenToRemove = []
            for child in root :
                childrenToRemove.append(child)
            for child in childrenToRemove :
                root.remove(child)
            outputPythonFilenameWithoutPath = outputFilenameWithoutPath+".py"
            pythonObject = ET.Element('PythonScriptController',attrib=dict(name=classNamePythonFile,listening="1",filename=outputPythonFilenameWithoutPath,classname=classNamePythonFile))
            root.insert(0,pythonObject)

        # add the component required plugin to the xml tree and output the xml tree into a file
        requiredPlugin = ET.Element('RequiredPlugin',attrib=dict(name="SofaPython", pluginName="SofaPython"))
        root.insert(0,requiredPlugin)
        # indent(root)
        # comment = ET.Comment(info_str)
        # root.insert(0,comment)
        tree.write(outputFilename+'.scn')
    else :
        writePythonFile(info_str,classNamePythonFile,root,outputFilename+'.py',0)

def main() :
    # parse the console input
    print ('Input')
    parser,args = parseInput()
    produceSceneAndPythonFile = args.onlyOutputPythonScript
    nodeToPythonScript = args.n
    if nodeToPythonScript and not produceSceneAndPythonFile :
        print ("ERROR: If you would like to replace a node using -n, please also use -s to produce a scene. If you would like to replace the complete scene by a python script (recommended), then please remove the argument -n.")
        sys.exit()

    # transform each standard scene to a python scene
    for i in range(len(args.inputScenes)) :
        inputScene = args.inputScenes[i]
        outputFilename = chopStringAtChar(inputScene,'\.',useContentBeforeChar=1)+'Python'
        if args.o is not None :
            if i < len(args.o) :
                outputFilename = args.o[i]
        print ('Input Scene: '+inputScene+', replace node: '+str(nodeToPythonScript)+', output: '+outputFilename+', produce .scn and .py: '+str(produceSceneAndPythonFile))
        transformXMLSceneToPythonScene(inputScene,produceSceneAndPythonFile,outputFilename,nodeToPythonScript)

if __name__ == '__main__':
    main()