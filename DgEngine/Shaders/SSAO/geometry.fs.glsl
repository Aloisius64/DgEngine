#version 450 core
                                                                        
in vec3 ViewPos;

layout (location = 0) out vec3 PosOut;   
                                                                            
void main()                                 
{                                           
    PosOut = ViewPos;
}
