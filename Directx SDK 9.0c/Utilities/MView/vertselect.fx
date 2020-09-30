//
// Standard texture effect
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//

DWORD Colr = 0xffff0000;   // color 
VECTOR vcol;

//
// Technique 0 - One pass
//

technique T0
{

    pass P0
    {        
 		Lighting = TRUE;
        ColorVertex = FALSE;
		MaterialEmissive = <vcol>;

        PointSize = 7.0f;
        PointSize_Min = 7.0f;
        PointSize_Max = 7.0f;
        PointSpriteEnable = False;
        PointScaleEnable = False;
        CullMode = None;
        FillMode = Solid;
        ZEnable = False;

        // Stage1
        ColorOp[0] = Disable;
        AlphaOp[0] = Disable;
    }
}