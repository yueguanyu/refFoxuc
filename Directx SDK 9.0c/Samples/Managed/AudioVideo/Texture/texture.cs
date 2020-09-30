//-----------------------------------------------------------------------------
// File: texture.cs
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Microsoft.DirectX.AudioVideoPlayback;
using Direct3D=Microsoft.DirectX.Direct3D;
using Microsoft.Samples.DirectX.UtilityToolkit;

namespace VideoTextureSample
{
    public class VideoTexture : IDeviceCreation 
    {
        #region Creation
        /// <summary>Create a new instance of the class</summary>
        public VideoTexture(Framework f) 
        { 
            // Store framework
            sampleFramework = f; 
        }
        #endregion

        private Framework sampleFramework = null; // Framework for samples
        // Vertex buffer to hold the data for the geometry
        private VertexBuffer vertexBuffer = null;
        // The actual video itself
        private Video videoTexture = null;
        // Can the video be rendered?
        private bool canRender = false;

        /// <summary>
        /// Called during device initialization, this code checks the device for some 
        /// minimum set of capabilities, and rejects those that don't pass by returning false.
        /// </summary>
        public bool IsDeviceAcceptable(Caps caps, Format adapterFormat, Format backBufferFormat, bool windowed)
        {
            // Any device is good
            return true;
        }

        /// <summary>
        /// This callback function is called immediately before a device is created to allow the 
        /// application to modify the device settings. The supplied settings parameter 
        /// contains the settings that the framework has selected for the new device, and the 
        /// application can make any desired changes directly to this structure.  Note however that 
        /// the sample framework will not correct invalid device settings so care must be taken 
        /// to return valid device settings, otherwise creating the Device will fail.  
        /// </summary>
        public void ModifyDeviceSettings(DeviceSettings settings, Caps caps)
        {
            // This application is designed to work on a pure device by not using 
            // any get methods, so create a pure device if supported and using HWVP.
            if ( (caps.DeviceCaps.SupportsPureDevice) && 
                ((settings.BehaviorFlags & CreateFlags.HardwareVertexProcessing) != 0 ) )
                settings.BehaviorFlags |= CreateFlags.PureDevice;

            // We will be using multiple threads
            settings.presentParams.ForceNoMultiThreadedFlag = false;
            settings.BehaviorFlags |= CreateFlags.MultiThreaded;
        }

        
        /// <summary>
        /// This event will be fired immediately after the Direct3D device has been 
        /// reset, which will happen after a lost device scenario. This is the best location to 
        /// create Pool.Default resources since these resources need to be reloaded whenever 
        /// the device is lost. Resources created here should be released in the OnLostDevice 
        /// event. 
        /// </summary>
        public void OnResetDevice(object sender, DeviceEventArgs e)
        {
            // Now Create the VB
            vertexBuffer = new VertexBuffer(typeof(CustomVertex.PositionNormalTextured), 100, 
                e.Device, Usage.WriteOnly, CustomVertex.PositionNormalTextured.Format, Pool.Default);
            OnCreateVertexBuffer();

            // Set up our view matrix. A view matrix can be defined given an eye point,
            // a point to lookat, and a direction for which way is up. Here, we set the
            // eye five units back along the z-axis and up three units, look at the
            // origin, and define "up" to be in the y-direction.
            e.Device.Transform.View = Matrix.LookAtLH( new Vector3( 0.0f, 3.0f,-5.0f ), 
                new Vector3( 0.0f, 0.0f, 0.0f ), new Vector3( 0.0f, 1.0f, 0.0f ) );

            // For the projection matrix, we set up a perspective transform (which
            // transforms geometry from 3D view space to 2D viewport space, with
            // a perspective divide making objects smaller in the distance). To build
            // a perpsective transform, we need the field of view (1/4 pi is common),
            // the aspect ratio, and the near and far clipping planes (which define at
            // what distances geometry should be no longer be rendered).
            e.Device.Transform.Projection = Matrix.PerspectiveFovLH( (float)Math.PI / 4.0f, 1.0f, 1.0f, 100.0f );

            // Setup the ambient color
            e.Device.RenderState.AmbientColor = unchecked((int)0xffffffff);
            // Turn off culling, so we see the front and back of the triangle
            e.Device.RenderState.CullMode = Cull.None;
            // Turn off D3D lighting
            e.Device.RenderState.Lighting = false;
            // Turn on the ZBuffer
            e.Device.RenderState.ZBufferEnable = true;

            e.Device.SamplerState[0].AddressU = TextureAddress.Clamp;
            e.Device.SamplerState[0].AddressV = TextureAddress.Clamp;

            string path = Utility.FindMediaFile("video\\skiing.avi");

            try
            {
                videoTexture = Video.FromFile(path);
                videoTexture.Ending += new System.EventHandler(this.MovieOver);
                videoTexture.TextureReadyToRender += new TextureRenderEventHandler(this.RenderIt);

                // Now start rendering to our texture
                videoTexture.RenderToTexture(e.Device);

                canRender = true;
            }
            catch(Exception err)
            {
                MessageBox.Show(string.Format("An error has occurred that will not allow this sample to continue.\r\nException={0}", err.ToString()), "This sample must exit.", MessageBoxButtons.OK, MessageBoxIcon.Information);
                sampleFramework.CloseWindow();
                throw err;
            }
        }

        /// <summary>
        /// This event function will be called fired after the Direct3D device has 
        /// entered a lost state and before Device.Reset() is called. Resources created
        /// in the OnResetDevice callback should be released here, which generally includes all 
        /// Pool.Default resources. See the "Lost Devices" section of the documentation for 
        /// information about lost devices.
        /// </summary>
        private void OnLostDevice(object sender, EventArgs e)
        {
            lock(this)
            {
                try
                {
                    canRender = false;
                    if (vertexBuffer != null)
                        vertexBuffer.Dispose();

                    vertexBuffer = null;

                    if (videoTexture != null)
                        videoTexture.Dispose();

                    videoTexture = null;
                }
                catch{}
            }
        }

        /// <summary>
        /// This event will be fired immediately after the Direct3D device has 
        /// been destroyed, which generally happens as a result of application termination or 
        /// windowed/full screen toggles. Resources created in the OnCreateDevice event 
        /// should be released here, which generally includes all Pool.Managed resources. 
        /// </summary>
        private void OnDestroyDevice(object sender, EventArgs e)
        {
            lock(this)
            {
                canRender = false;
            }
        }


        /// <summary>
        /// Fired when the video has a frame ready to be rendered into the texture
        /// </summary>
        void RenderIt(object sender, TextureRenderEventArgs e)
        {
            lock(this)
            {
                Device device = sampleFramework.Device;
                // We will be updating the texture now
                using(e.Texture)
                {
                    if (!canRender)
                        return; // Cannot render right now

                    bool beginSceneCalled = false;
                    // Clear the render target and the zbuffer 
                    device.Clear(ClearFlags.ZBuffer | ClearFlags.Target, 0x000000ff, 1.0f, 0);

                    try
                    {
                        device.BeginScene();
                        beginSceneCalled = true;

                        // Make sure this isn't being updated at this time
                        // Setup the world matrix
                        device.Transform.World = Matrix.RotationAxis(new Vector3((float)Math.Cos(Environment.TickCount / 250.0f),1,(float)Math.Sin(Environment.TickCount / 250.0f)), Environment.TickCount / 1000.0f );

                        // Set the texture
                        device.SetTexture(0,e.Texture);
                        device.SetStreamSource(0, vertexBuffer, 0);
                        device.VertexFormat = CustomVertex.PositionNormalTextured.Format;
                        device.DrawPrimitives(PrimitiveType.TriangleStrip, 0, (4*25)-2);
                    }
                    finally
                    {
                        if (beginSceneCalled)
                            device.EndScene();
                    }

                    device.Present();
                }
            }
        }
        void MovieOver(object sender, EventArgs e)
        {
            videoTexture.Stop();
            videoTexture.Play();
        }
        void OnCreateVertexBuffer()
        {
            // Create a vertex buffer (100 customervertex)
            CustomVertex.PositionNormalTextured[] verts = (CustomVertex.PositionNormalTextured[])vertexBuffer.Lock(0,0); // Lock the buffer (which will return our structs)
            for (int i = 0; i < 50; i++)
            {
                // Fill up our structs
                float theta = (float)(2 * Math.PI * i) / 49;
                verts[2 * i].Position = new Vector3((float)Math.Sin(theta), -1, (float)Math.Cos(theta));
                verts[2 * i].Normal = new Vector3((float)Math.Sin(theta), 0, (float)Math.Cos(theta));
                verts[2 * i].Tu       = ((float)i)/(50-1);
                verts[2 * i].Tv       = 1.0f;
                verts[2 * i + 1].Position = new Vector3((float)Math.Sin(theta), 1, (float)Math.Cos(theta));
                verts[2 * i + 1].Normal = new Vector3((float)Math.Sin(theta), 0, (float)Math.Cos(theta));
                verts[2 * i + 1].Tu       = ((float)i)/(50-1);
                verts[2 * i + 1].Tv       = 0.0f;
            }
            // Unlock (and copy) the data
            vertexBuffer.Unlock();
        }

        /// <summary>
        /// Entry point to the program. Initializes everything and goes into a message processing 
        /// loop. Idle time is used to render the scene.
        /// </summary>
        static int Main() 
        {
            using(Framework sampleFramework = new Framework())
            {
                VideoTexture sample = new VideoTexture(sampleFramework);
                // Set the callback functions. These functions allow the sample framework to notify
                // the application about device changes, user input, and windows messages.  The 
                // callbacks are optional so you need only set callbacks for events you're interested 
                // in. However, if you don't handle the device reset/lost callbacks then the sample 
                // framework won't be able to reset your device since the application must first 
                // release all device resources before resetting.  Likewise, if you don't handle the 
                // device created/destroyed callbacks then the sample framework won't be able to 
                // recreate your device resources.
                sampleFramework.Disposing += new EventHandler(sample.OnDestroyDevice);
                sampleFramework.DeviceLost += new EventHandler(sample.OnLostDevice);
                sampleFramework.DeviceReset += new DeviceEventHandler(sample.OnResetDevice);

                try
                {

                    // Show the cursor and clip it when in full screen
                    sampleFramework.SetCursorSettings(true, true);

                    // Initialize the sample framework and create the desired window and Direct3D 
                    // device for the application. Calling each of these functions is optional, but they
                    // allow you to set several options which control the behavior of the sampleFramework.
                    sampleFramework.Initialize( true, false, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
                    sampleFramework.CreateWindow("VideoTexture");
                    sampleFramework.CreateDevice( 0, true, Framework.DefaultSizeWidth, Framework.DefaultSizeHeight, 
                        sample);

                    // Ignore size changes
                    sampleFramework.IsIgnoringSizeChanges = true;
                    // Pause rendering since it will be done when the texture is ready
                    sampleFramework.Pause(false, true); 

                    // Pass control to the sample framework for handling the message pump and 
                    // dispatching render calls. The sample framework will call your FrameMove 
                    // and FrameRender callback when there is idle time between handling window messages.
                    sampleFramework.MainLoop();

                }
#if(DEBUG)
                catch (Exception e)
                {
                    // In debug mode show this error (maybe - depending on settings)
                    sampleFramework.DisplayErrorMessage(e);
#else
            catch
            {
                // In release mode fail silently
#endif
                    // Ignore any exceptions here, they would have been handled by other areas
                    return (sampleFramework.ExitCode == 0) ? 1 : sampleFramework.ExitCode; // Return an error code here
                }

                // Perform any application-level cleanup here. Direct3D device resources are released within the
                // appropriate callback functions and therefore don't require any cleanup code here.
                return sampleFramework.ExitCode;
            }
        }
    }
}
