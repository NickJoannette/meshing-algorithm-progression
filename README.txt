TABLE OF CONTENT
    1- PROJECT ABSTRACT
    2- SPECIFICATIONS
    3- ASSUMPTIONS
    4- CONFIGURATION
    5- USER NAVIGATION AND CONTROLS
    6  NOTES
    7- PROJECT TEAM
    
NOTE: 
- IF OPENING THE SOLUTION FILE IN VISUAL STUDIO, PLEASE ASSIGN YOUR CONFIRUGATION MODE TO DEBUG X86 TO RUN THE CODE; THE LIBRARIES ARE 32 BIT
    THERE IS A RELEASE_FOLDER INTENDED FOR DEPLOYMENT TO USERS IF NECESSARY FOR THE ASSIGNMENT, WHERE YOU CAN RUN THE PROGRAM BY OPENING THE .EXE

- ALL HEADER AND .CPP FILES ARE CONTAINED IN THE PA1_PRACTICE FOLDER

1- PROJECT ABSTRACT
    This deliverable (PA1) is the first part of a project divided in 3 deliverables (PA1, PA2 and PA3). The goal of the project is to learn practical aspects of OpenGL programming through a series of programming 
    assignments.
    PA1 implements the following requirements:
    - Create a simple virtual scenes consisting of objects. 
    - Display them as wireframe meshes by drawing triangles
    - Set up a virtual camera  and the necessary controls to view the scene from varied angles and distances.
    - Setup the necessary controls to manipulate the different models.
    
2- GENERAL SPECIFICATIONS (SEE PROJECT DESCIPTION FOR DETAILED SPECIFICATIONS)
    - Creates a 100x100 square grid (ground surface) in the XZ plane centered at the origin.
    - Window size must be 1024x768 with double buffering support.
    - One model for each team member must be created according to specifications indicated in project description document.
    - Models must be independent and be placed in the four corners of the grid; the fifth model is initially positioned at the center of the grid, aligned along the X axis.
    - The application must use OpenGL 3.1 and onwards and must include brief comments explaining each step.
    
3- OUR ASSUMPTIONS
    Assumptions we've made about the instructions:
    - Whenever "the model" is mentioned, we assume that the instructions are referring to an individual instance of one of the group member's letter & number model, such as the 'U9' model in 
      the instructions. As such, we made it possible for the user to select any of the five models on the grid by pressing one of the corresponding function keys between 'F1' and 'F5', 
      as described in the user navigations and controls section (eg. Press F1 to control group member's 1 model, F2 to control member 2's model, etc.). Once a model is selected, the movement, 
      scaling, and rotation controls are bound to the selected model until it has been de-selected. When a group member's model is selected, its color changes to show it is currently being controlled by the user.
    - Similarly, when "the world" is described, we assume that this refers to all models in the scene, including the grid and coordinate axes. When the arrow keys are used to change world orientation, the 
      world matrix is rotated to affect all models at once.
    
4- REQUIREMENTS/CONFIGURATION
    The project is developped on Visual Studio 2017 and requires installation and configuration of the following libraries:
    - GLFW-3.3.2
    - GLEW-2.1.0
    - GLM-0.9.9.8
    
5- USER NAVIGATION AND CONTROLS
    - RENDERING MODES
      All models are drawn with the GL_TRIANGLES primitive by default. Pressing the following keys will change the rendering mode:
        . Uppercase 'P' -> Render/draw all models using the GL_POINTS primitive.
        . Uppercase 'L' -> Render/draw all models using the GL_LINES primitive.
        . Uppercase 'T' -> Render/draw all models using the GL_TRIANGLES primitive.
    
    - MODEL CONTROLS (MOVEMENT, ROTATION AND SCALING):
        Choose the selected models using following keys:
        . F1 -> Selects Nick's Model (center of the grid at coordinate (0,0,0)).
        . F2 -> Selects Sami's Model (center of the grid at coordinate (-50,0,-50)).
        . F3 -> Selects Ale's Model (center of the grid at coordinate (50,0,-50)).
        . F4 -> Selects Adrien's Model (center of the grid at coordinate (50,0,50)).
        . F5 -> Selects Mohamad's Model (center of teh grid at coordinate (-50,0,50)).
        
        To scale the selected model, use the following keys:
        . U -> Scape Up by 1%
        . J -> Scale Down by 1%
        
        The user can control the selected model's position and orientation by using the following keys:
        . Uppercase A -> Move left (the negative x direction of that model).
        . Lowercase a -> Rotate left by 5 degrees (the positive y axis of that model).
        . Uppercase D -> Move right (the positive x direction of that model).
        . Lowercase d -> Rotate right by 5 degrees (the negative y axis of that model).
        . Uppercase W -> Move up (the positive y direction of that model).
        . Lowercase w -> Rotate around the positive x axis by 5 degrees.
        . Uppercase S -> Move down (the negative y direction of that model).
        . Lowercase s -> Rotate around the negative x axis by 5 degrees.
        
    - WORLD ORIENTATION
        . Left arrow    -> Rotate the world 1 radian CCW about the positive x axis.
        . Right arrow   -> Rotate the world 1 radian CCW about the negative x axis.
        . Up arrow      -> Rotate the world 1 radian CCW about the positive y axis.
        . Down arrow    -> Rotate the world 1 radian CCW about the negative y axis.
        . HOME          -> Reset the world matrix to its original orientation, and reset the camera's position, pitch, yaw, field of view and focus/viewing angle to their original values.
        
    - PAN AND TILT
        . With the right mouse button pressed, movement in the x direction of the window will pan the camera accordingly.
        . With the middle mouse button pressed, movement in the y direction of the window will tilt the camera accordingly.
        
    - ZOOM
        With the left mouse button pressed, movement in the y direction of the window will zoomin/out of the screen. 
        Dragging towards the top of the window zooms 'in' , while dragging towards the bottom zooms 'out'. There is no constraint to the FOV.
        
    - CAMERA CONTROL
        While holding the minus button ('-'), pressing the following keys will position the camera:
        . 'A' -> Move the camera along the negative x axis.
        . 'D' -> Move the camera along the positive x axis.
        . 'W' -> Move the camera forward.
        . 'S' -> Move the camera backward.
        . 'Q' -> Move the camera upward.
        . 'Z' -> Move the camera downward.
        . '2' -> Strafe the camera left.
        . '3' -> Strafe the camera right.
		
        
    - EXTRA FEATURES AND CONTROLS!
        . Map the vertices of the grid to a height map of the Earth
            Press LEFT_CTRL && G to map the y coordinates of the vertices of the grid to a height map of the Earth. 
            The shader will color it based on the height to make it look like the earth (green land, blue water). Press again to revert to the flat grid. 

6- NOTES
        i- Beginning camera position and orientation
            The camera begins at the position (0,30,90), with its viewing angle tilted down by -tan(30/90) degrees. 
            This means that the original point of focus is exactly the origin (0,0,0) as per the assignment instructions.
        
        ii- Grid orientation
            The grid is composed of 100 x 100 squares in the XZ-plane. It is centered at x=0, z=0, such that its center is at the origin in the XZ plane. 
            It is translated 0.001 units in the negative y direction to make the coordinate axes more visible.
        
        iii- Coordinate axes
             The coordinate axes are always drawn as GL_LINES to make them more visible, and are all 5 grid units in length. Their starting points are all at the origin (0,0,0).
             
        iV- World rotation normalization
            We have produced a function that gives a fixed speed movement, which depends on the FPS of each unique machine that will be running this project, so that the movement speed of the models will be the same regardless of the machine's speed.

7- PROJECT TEAM 
    Our project team is composed of the following members:
    - Nicholas Joanette
    - Sami Merhi
    - Mohamad Ghanmeh
    - Adrien Kamran
    - Ale Niang