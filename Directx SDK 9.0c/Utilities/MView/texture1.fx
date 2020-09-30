//
// Standard texture effect
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//

VECTOR Ambient;
VECTOR Specular;
VECTOR Diffuse;
VECTOR Emissive;
float Power;
TEXTURE Texture0 < string Name = ""; >;

//
// Technique 0 - One pass
//

technique T0
{

    pass P0
    {        
    MaterialPower = <Power>;
    MaterialEmissive = <Emissive>;
    MaterialAmbient = <Ambient>;
    MaterialSpecular = <Specular>;
    MaterialDiffuse = <Diffuse>;
    // Stage0
    ColorOp[0]   = Modulate;
    ColorArg1[0] = Texture;
    ColorArg2[0] = Current;
    AlphaOp[0]   = Modulate;
    AlphaArg1[0] = Texture;
    AlphaArg2[0] = Current;

    MinFilter[0] = Linear;
    MagFilter[0] = Linear;
    MipFilter[0] = Point;

    Texture[0] = <Texture0>;

    // Stage1
    ColorOp[1] = Disable;
    AlphaOp[1] = Disable;
    }
}