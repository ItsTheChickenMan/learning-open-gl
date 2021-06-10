// pre-created definitions

#ifndef PRACTICE_SHAPES_H
#define PRACTICE_SHAPES_H

// acute triangle
float triangle_vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,	0.5f, 0.0f
};

float rectangle_vertices[] = {
	// position						 // texture coordinates
	 0.5f,  0.5f, 0.0f,		 1.0f,	 1.0f, // top right  
	 0.5f, -0.5f, 0.0f,		 1.0f,   0.0f, // bottom right
	-0.5f, -0.5f, 0.0f, 	 0.0f,	 0.0f, // bottom left
	-0.5f,  0.5f, 0.0f,		 0.0f,	 1.0f  // top left
};

// indices to make a rectangle
u32 rectangle_indices[] = {
	0, 1, 3,
	1, 2, 3
};

// cube vertices (stolen from Joey DeVries)
float cube_vertices[] = {
		// vertices							// texture		// normals
    -0.5f, -0.5f, -0.5f,  	0.0f, 0.0f,  	0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  	1.0f, 0.0f,  	0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  	0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  	0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  	0.0f, 1.0f,  	0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  	0.0f, 0.0f,  	0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  	0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  	1.0f, 0.0f,  	0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 	0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  	1.0f, 1.0f,  	0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  	0.0f, 1.0f,  	0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  	0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  	-1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  	-1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  	-1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  	-1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, 		0.0f, 0.0f,  	-1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  	-1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  	1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  	1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  	1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  	1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  	1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  	1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  	0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  	1.0f, 1.0f,  	0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  	1.0f, 0.0f,  	0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  	1.0f, 0.0f,  	0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  	0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  	0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  	0.0f, 1.0f,  	0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  	0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  	0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  	0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  	0.0f, 0.0f,  	0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  	0.0f, 1.0f,  	0.0f,  1.0f,  0.0f
};

#endif