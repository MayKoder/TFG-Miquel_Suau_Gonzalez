#ifdef vertex
#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 model;
//uniform mat4 projection;
uniform vec4 inputColor;

out vec2 texCoord;
out vec4 inpColor;

void main()
{
inpColor = inputColor;
    texCoord = vec2((aPos.x + 1.0) / 2.0, 1 - (aPos.y + 1.0) / 2.0);
    gl_Position = model * vec4(aPos.x, aPos.y, 0.0, 1.0);
}  
#endif

#ifdef fragment
#version 330 core

//out vec4 FragColor;
in vec2 texCoord;
in vec4 inpColor;

void main()
{   
   //float maxX = 1.0 - 0.0;
   //float minX = 0.0;
   //float maxY = maxX / 1.0;
   //float minY = minX / 1.0;

   //if (texCoord.x < maxX && texCoord.x > minX && texCoord.y < maxY && texCoord.y > minY) 
   //{
   //  gl_FragColor = vec4(1.0f, 0.5f, 0.5f, 0.5f);
   //} else 
   //{
   //  gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 0.5f);
   //}

    gl_FragColor = vec4(inpColor);
}
#endif
