# Node Based Particle System
**** 
Node Based Particle System is a bachelor's degree final project, developed by [Aitor Luque](<https://github.com/Aitorlb7>) under the supervision of [Jesus Díaz](<https://www.linkedin.com/in/jesusdz/>) at [CITM](<https://www.citm.upc.edu/>).

The main objective is to develop an open-source engine application to serve as a
fast and understandable editor tool for creating complex procedural Particle Systems
ready to be implemented in a final product, working towards a process speed up of the
big time investment this task usually takes.

The tool uses C++ , OpenGL and ImGui to build a node-based interface able to create
and edit a wide amount of nodes storing parameters and particle systems components
along their interactions, designed to be used without any previous technical
experience.

The structure of the tool is built as a resemblance of already existing engines with
node-based procedural generation tools such as Houdini and Unity.

* [Repository](https://github.com/Aitorlb7/Node_Based_Particle_System)

This project uses as a base engine our own 3D Engine [ASE](<https://github.com/paufiol/AnotherSmallEngine>) (Another Small Engine) developed alongside with [Pau Fiol](<https://github.com/paufiol>)

****
## Developer
### Aitor Luque - [Aitorlb7](<https://github.com/Aitorlb7>).

****
## How to Use
* **Mouse Wheel:** Zooms camera forward and backwards.
* **Alt + Right click:** Rotate camera over mouse position or else selected game object.
* **Alt + Center click(wheel):** Pan camera.
* **Left click:** Select object/Use Gizmo.
* **DELETE:** delete selected Game Object.
* **W:** Set Guizmo to Translate mode.
* **E:** Set Guizmo to Rotate mode.
* **R:** Set Guizmo to Scale mode.
* Within Editor:
  * **Ctrl+C:** Copy.
  * **Ctrl+X:** Cut.
  * **Ctrl+V:** Paste.
  * **Ctrl+Z:** Undo.
  * **Arrow Keys:** Navigation.
* Importing options:
  * Right Click on an item in the explorer -> Import Asset
  * Hold Left Click on an item in the explorer and drag into the scene.
* Saving and Loading an Scene
  * File -> Save Scene -> Write the Name of scene -> Press Enter
  * File -> Load Scene -> Open the expandable select the scene -> Press Load

****
## Libraries used
* STL (Standard Template Library)
* Dear ImGui
    * [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
    * [ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit)
    * [ImGuiNodeEditor](https://github.com/thedmd/imgui-node-editor)
* OpenGL 
* Glew 
* MatGeoLib
* Assimp
* Parson
* DevIL
****

**Warning:** The models won't load if Library folder is erased and the meta files are not, We load with the data stored inside the meta which points to the Custom Files stored in Library. --> To fix that erase all ".meta" files inside Assets folder.
