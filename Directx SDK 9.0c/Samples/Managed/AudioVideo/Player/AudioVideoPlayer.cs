using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.AudioVideoPlayback;
using Microsoft.Samples.DirectX.UtilityToolkit;

namespace Player
{
    /// <summary>
    /// Summary description for Form1.
    /// </summary>
    public class AVPlayer : System.Windows.Forms.Form
    {

        private string filterText = "Video Files (*.avi; *.qt; *.mov; *.mpg; *.mpeg; *.m1v; *.wmv)|*.avi; *.qt; *.mov; *.mpg; *.mpeg; *.m1v; *.wmv|" +
            "Audio files (*.wav; *.mpa; *.mp2; *.mp3; *.au; *.aif; *.aiff; *.snd; *.wma)|*.wav; *.mpa; *.mp2; *.mp3; *.au; *.aif; *.aiff; *.snd; *.wma|" +
            "MIDI Files (*.mid, *.midi, *.rmi)|*.mid; *.midi; *.rmi|" +
            "Image Files (*.jpg, *.bmp, *.gif, *.tga)|*.jpg; *.bmp; *.gif; *.tga|" +
            "All Files (*.*)|*.*";

        private Video videoClip = null;
        private Audio audioClip = null;

        #region Winforms variables

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;
        private System.Windows.Forms.MainMenu mnuMain;
        private System.Windows.Forms.OpenFileDialog ofdOpen;
        private System.Windows.Forms.MenuItem menuItem3;
        private System.Windows.Forms.MenuItem mnuFile;
        private System.Windows.Forms.MenuItem mnuOpen;
        private System.Windows.Forms.MenuItem menuItem1;
        private System.Windows.Forms.MenuItem mnuPlay;
        private System.Windows.Forms.MenuItem mnuStop;
        private System.Windows.Forms.MenuItem mnuPause;
        private System.Windows.Forms.MenuItem menuItem2;
        private System.Windows.Forms.MenuItem mnuFull;
        private System.Windows.Forms.MenuItem mnuExit;
        #endregion

        /// <summary>Create the object and call the open file method</summary>
        public AVPlayer()
        {
            InitializeComponent();
            OpenFile();
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            CleanupObjects();
            if( disposing )
            {
                if (components != null) 
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.mnuMain = new System.Windows.Forms.MainMenu();
            this.mnuFile = new System.Windows.Forms.MenuItem();
            this.mnuOpen = new System.Windows.Forms.MenuItem();
            this.menuItem3 = new System.Windows.Forms.MenuItem();
            this.mnuExit = new System.Windows.Forms.MenuItem();
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.mnuPlay = new System.Windows.Forms.MenuItem();
            this.mnuStop = new System.Windows.Forms.MenuItem();
            this.mnuPause = new System.Windows.Forms.MenuItem();
            this.ofdOpen = new System.Windows.Forms.OpenFileDialog();
            this.menuItem2 = new System.Windows.Forms.MenuItem();
            this.mnuFull = new System.Windows.Forms.MenuItem();
            // 
            // mnuMain
            // 
            this.mnuMain.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                    this.mnuFile,
                                                                                    this.menuItem1});
            // 
            // mnuFile
            // 
            this.mnuFile.Index = 0;
            this.mnuFile.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                    this.mnuOpen,
                                                                                    this.menuItem3,
                                                                                    this.mnuExit});
            this.mnuFile.Text = "&File";
            // 
            // mnuOpen
            // 
            this.mnuOpen.Index = 0;
            this.mnuOpen.Shortcut = System.Windows.Forms.Shortcut.CtrlO;
            this.mnuOpen.Text = "&Open Clip";
            this.mnuOpen.Click += new System.EventHandler(this.mnuOpen_Click);
            // 
            // menuItem3
            // 
            this.menuItem3.Index = 1;
            this.menuItem3.Text = "-";
            // 
            // mnuExit
            // 
            this.mnuExit.Index = 2;
            this.mnuExit.Text = "E&xit";
            this.mnuExit.Click += new System.EventHandler(this.mnuExit_Click);
            // 
            // menuItem1
            // 
            this.menuItem1.Index = 1;
            this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                      this.mnuPlay,
                                                                                      this.mnuStop,
                                                                                      this.mnuPause,
                                                                                      this.menuItem2,
                                                                                      this.mnuFull});
            this.menuItem1.Text = "&Control";
            // 
            // mnuPlay
            // 
            this.mnuPlay.Index = 0;
            this.mnuPlay.Shortcut = System.Windows.Forms.Shortcut.CtrlP;
            this.mnuPlay.Text = "&Play";
            this.mnuPlay.Click += new System.EventHandler(this.mnuPlay_Click);
            // 
            // mnuStop
            // 
            this.mnuStop.Index = 1;
            this.mnuStop.Shortcut = System.Windows.Forms.Shortcut.CtrlS;
            this.mnuStop.Text = "&Stop";
            this.mnuStop.Click += new System.EventHandler(this.mnuStop_Click);
            // 
            // mnuPause
            // 
            this.mnuPause.Index = 2;
            this.mnuPause.Shortcut = System.Windows.Forms.Shortcut.CtrlA;
            this.mnuPause.Text = "P&ause";
            this.mnuPause.Click += new System.EventHandler(this.mnuPause_Click);
            // 
            // menuItem2
            // 
            this.menuItem2.Index = 3;
            this.menuItem2.Text = "-";
            // 
            // mnuFull
            // 
            this.mnuFull.Index = 4;
            this.mnuFull.Text = "Toggle Fu&llscreen\t<Alt-Enter>";
            this.mnuFull.Click += new System.EventHandler(this.mnuFull_Click);
            // 
            // AVPlayer
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(264, 38);
            this.Menu = this.mnuMain;
            this.Name = "AVPlayer";
            this.Text = "Audio Video Player";

        }
        #endregion

        /// <summary>Cleans up resources used by the player</summary>
        private void CleanupObjects()
        {
            if (videoClip != null)
            {
                videoClip.Stop();
                videoClip.Dispose();
            }
            videoClip = null;

            if (audioClip != null)
            {
                audioClip.Stop();
                audioClip.Dispose();
            }
            audioClip = null;
        }

        private void OpenFile()
        {
            if ((ofdOpen.InitialDirectory == null) || (ofdOpen.InitialDirectory == string.Empty))
                ofdOpen.InitialDirectory = Utility.SdkMediaPath;

            ofdOpen.Filter = filterText;
            ofdOpen.Title = "Open media file";
            ofdOpen.CheckFileExists = true;

            if (ofdOpen.ShowDialog(this) == DialogResult.OK)
            {
                // Cleanup previous
                CleanupObjects();

                // Now let's try to open this file
                try
                {
                    // First try to open this as a video file
                    videoClip = new Video(ofdOpen.FileName);
                    videoClip.Ending += new System.EventHandler(this.ClipEnded);
                    videoClip.Owner = this;
                    // Start playing now
                    videoClip.Play();
                }
                catch
                {
                    try
                    {
                        // opening this as a video file failed.. Maybe it's audio only?
                        audioClip = new Audio(ofdOpen.FileName);
                        audioClip.Ending += new System.EventHandler(this.ClipEnded);
                        // Start playing now
                        audioClip.Play();
                    }
                    catch
                    {
                        MessageBox.Show("This file could not be opened.", "Invalid file.", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }
            }
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main() 
        {
            using (AVPlayer player = new AVPlayer())
            {
                Application.Run(player);
            }
        }

        /// <summary>
        /// Fired when the audio or video has ended
        /// </summary>
        private void ClipEnded(object sender, System.EventArgs e)
        {
            // The clip has ended, stop and restart it
            if (videoClip != null)
            {
                videoClip.Stop();
                videoClip.Play();
            }
            else
            {
                if (audioClip != null)
                {
                    audioClip.Stop();
                    audioClip.Play();
                }
            }
        }

        private void mnuOpen_Click(object sender, System.EventArgs e)
        {
            this.OpenFile();
        }

        private void mnuPlay_Click(object sender, System.EventArgs e)
        {
            if (videoClip != null)
                videoClip.Play();
            else
            {
                if (audioClip != null)
                    audioClip.Play();
            }
        }

        private void mnuStop_Click(object sender, System.EventArgs e)
        {
            if (videoClip != null)
                videoClip.Stop();
            else
            {
                if (audioClip != null)
                    audioClip.Stop();
            }
        }

        private void mnuPause_Click(object sender, System.EventArgs e)
        {
            if (videoClip != null)
                videoClip.Pause();
            else
            {
                if (audioClip != null)
                    audioClip.Pause();
            }
        }

        private void mnuExit_Click(object sender, System.EventArgs e)
        {
            this.Dispose();
        }

        private void mnuFull_Click(object sender, System.EventArgs e)
        {
            if (videoClip != null)
                videoClip.Fullscreen = !videoClip.Fullscreen;
        }
        protected override void OnKeyDown(System.Windows.Forms.KeyEventArgs e)
        {
            if ( (e.Alt) && (e.KeyCode == System.Windows.Forms.Keys.Return))
            {
                mnuFull_Click(mnuFull, null);
            }

            // Allow the control to handle the keystroke now
            base.OnKeyDown(e);
        }
    }
}
