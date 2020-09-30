//
// Standard texture effect
// Copyright {c} 2000 Microsoft Corporation. All rights reserved.
//

VECTOR vClr;

//
// Technique 0 - One pass
//

technique T0
{

    pass P0
    {        
		Lighting = TRUE;
        ColorVertex = FALSE;
		MaterialEmissive = <vClr>;
		MaterialAmbient = {0.0f, 0.0f, 0.0f, 0.0f};
		MaterialSpecular = {0.0f, 0.0f, 0.0f, 0.0f};
		MaterialDiffuse = {0.0f, 0.0f, 0.0f, 0.0f};

        // Stage1
        ColorOp[0] = Disable;
        AlphaOp[0] = Disable;
    }
}