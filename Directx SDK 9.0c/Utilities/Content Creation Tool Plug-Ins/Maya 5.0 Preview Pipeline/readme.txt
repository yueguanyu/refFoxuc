Installation Instructions
=========================

The "install.bat" installation script is located in the same directory as this readme.txt file
(default is C:\Program Files\Microsoft DirectX 9.0 SDK (Summer 2004)\Utilities\Plug-Ins\Maya 5.0 Preview Pipeline).
This script assumes that Maya 5.0 is installed to the default location (C:\Program Files\AliasWavefront\Maya5.0). 
If Maya is installed to a different location please edit the script to match your configuration.

1. Run "install.bat" to copy the plug-in files to the Maya folders
2. Launch Maya
3. From the main menu select "Window->Settings/Preferences->Plugin Manager..."
4. Locate "D3DMayaExportX.mll" in the list and check the boxes next to "loaded" and "auto load" and
   click the Close button
5. You should now see a "DirectX" main menu item indicating the plug-in was successfully installed



User Guide
==========

To preview your meshes press DirectX->Viewers->* where * is the selected viewing system.  
Floating indicates a floating window and the rest indicate which window you would like to DirectX 
viewer to bind over.  The view will then be driven by the 'persp' camera.

The Preview Pipeline defines a new material type which encapsulates a D3DX Effect file (*.fx). 
Maya objects which are shaded with this "DirectX Shader" material will be rendered within the
Preview Pipeline viewer window using the associated Effect.

To create a "DirectX Shader" material:

1. From the main menu select "Window->Rendering Editors->Hypershade..." to open the Hypershade
   window.
2. Verify that "Create Materials" is selected within the creation dropdown menu
3. Click "DirectX Shader" to create an empty material. At this point no .fx file is associated
   with the material.
4. Open the Attribute Editor and select the tab for the new material (the default name is
   "DirectXShader1"). 
5. Click the "Select and Customize FX-File" button to open the FX Editor dialog. Using this dialog
   you can select a .fx file for the material and adjust Effect parameters. Click "Open" to
   select a .fx file and notice after a file is selected that the Effect's editable parameters 
   appear in the listbox. Click "Done" when finished.
6. To apply the material Middle-mouse drag the material from the Hypershade window onto an object.
   
The Preview Pipeline viewer window should now be rendering the object using the associated effect.
The viewer window can either be a standard floating window, in which you can move and adjust the 
window as normal; or can be bound to a Maya panel. To adjust the viewer window mode select an option 
from the "DirectX->Viewers..." main menu item. Note that the camera for the viewer window is 
automatically tied to Maya's perspective view.

Animation:
Animation is gathered whenever the "DirectX->Gather Animation" menu is selected.  Note that the viewer 
does not support animation.Note that MView does not support animation is the same way that DXCC does.  
Use the multi-animation sample to view it.  You must turn on all animation sets simultaneously since 
each animation set animates 1 bone in this usage scenareo. 

Inverted Geometry:
The Preview Pipeline captures geometry as is.  It does correct for winding order when the mesh's 
world matrix determinant is negative.  But it does not correct the geometry based on render flags.  
The following "render stat" flags may appear as inverted geometry "Double Sided" and "Opposite."  
The artist should have these setting off and instead use the "Edit Polygon->Normals->Reverse" 
command instead to correctly alter the geometry.



NOT SUPPORTED:  (* indicates it will likely appear in future releases)
*Skinning (files are saved in current mesh pose rather than bind pose + weights)
Non-linear skinning methodes such as bulges
*Shader conversion Maya to DirectX 
Animated attributes other than the node transform
Animated shader parameters
Vertex Animation
Maya Texture Nodes (DXCC uses it's own texture loading)
Multiple Vertex Positions, Colors, and Normals











