import QtQuick 2.0
import Scene 1.0
import Viewer 1.0

QtObject {
    id: root

    property string name: ""
    property var scene: null
    property Viewer viewer: null

    property var mouseClickedMapping: Array()
    property var mouseDoubleClickedMapping: Array()
    property var mousePressedMapping: Array()
    property var mouseReleasedMapping: Array()
    property var mouseWheelMapping: null
    property var mouseMoveMapping: null

    property var keyPressedMapping: Array()
    property var keyReleasedMapping: Array()

    // mapping between user interaction and binding
    function addMouseClickedMapping(button, binding) {
        mouseClickedMapping[button] = binding;
    }

    function addMouseDoubleClickedMapping(button, binding) {
        mouseDoubleClickedMapping[button] = binding;
    }

    function addMousePressedMapping(button, binding) {
        mousePressedMapping[button] = binding;
    }

    function addMouseReleasedMapping(button, binding) {
        mouseReleasedMapping[button] = binding;
    }

    function setMouseWheelMapping(binding) {
        mouseWheelMapping = binding;
    }

    function setMouseMoveMapping(binding) {
        mouseMoveMapping = binding;
    }

    function addKeyPressedMapping(key, binding) {
        keyPressedMapping[key] = binding;
    }

    function addKeyReleasedMapping(key, binding) {
        keyReleasedMapping[key] = binding;
    }

    // event
    function mouseClicked(mouse) {
        var binding = mouseClickedMapping[mouse.button];
        if(binding)
            binding(mouse);
    }

    function mouseDoubleClicked(mouse) {
        var binding = mouseDoubleClickedMapping[mouse.button];
        if(binding)
            binding(mouse);
    }

    function mousePressed(mouse) {
        var binding = mousePressedMapping[mouse.button];
        if(binding)
            binding(mouse);
    }

    function mouseReleased(mouse) {
        var binding = mouseReleasedMapping[mouse.button];
        if(binding)
            binding(mouse);
    }

    function mouseWheel(wheel) {
        var binding = mouseWheelMapping;
        if(binding)
            binding(wheel);
    }

    function mouseMove(mouse) {
        var binding = mouseMoveMapping;
        if(binding)
            binding(mouse);
    }

    function keyPressed(event) {
        var binding = keyPressedMapping[event.key];
        if(binding)
            binding(event);
    }

    function keyReleased(event) {
        var binding = keyReleasedMapping[event.key];
        if(binding)
            binding(event);
    }

    //Component.onCompleted: if(scene) init();
    onSceneChanged:
    {
        //if(scene)
        //    scene.Component.completed();

        //console.log(scene);
        //if(scene) init();
    }

    readonly property QtObject d: QtObject {

        readonly property var sceneConnections: Connections {
            target: root
            onSceneChanged: root.init();
            Component.onCompleted: root.init();
        }
    }

    function init() {
        console.error("UserInteractor_* must implement the init function !");
    }
}
