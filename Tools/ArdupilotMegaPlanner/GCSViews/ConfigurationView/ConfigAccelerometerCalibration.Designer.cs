﻿namespace ArdupilotMega.GCSViews.ConfigurationView
{
    partial class ConfigAccelerometerCalibration
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ConfigAccelerometerCalibration));
            this.label28 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.pictureBoxQuadX = new System.Windows.Forms.PictureBox();
            this.pictureBoxQuad = new System.Windows.Forms.PictureBox();
            this.BUT_levelac2 = new ArdupilotMega.MyButton();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxQuadX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxQuad)).BeginInit();
            this.SuspendLayout();
            // 
            // label28
            // 
            resources.ApplyResources(this.label28, "label28");
            this.label28.Name = "label28";
            // 
            // label16
            // 
            resources.ApplyResources(this.label16, "label16");
            this.label16.Name = "label16";
            // 
            // label15
            // 
            resources.ApplyResources(this.label15, "label15");
            this.label15.Name = "label15";
            // 
            // pictureBoxQuadX
            // 
            this.pictureBoxQuadX.Cursor = System.Windows.Forms.Cursors.Hand;
            this.pictureBoxQuadX.Image = global::ArdupilotMega.Properties.Resources.quadx;
            resources.ApplyResources(this.pictureBoxQuadX, "pictureBoxQuadX");
            this.pictureBoxQuadX.Name = "pictureBoxQuadX";
            this.pictureBoxQuadX.TabStop = false;
            this.pictureBoxQuadX.Click += new System.EventHandler(this.pictureBoxQuadX_Click);
            // 
            // pictureBoxQuad
            // 
            this.pictureBoxQuad.Cursor = System.Windows.Forms.Cursors.Hand;
            this.pictureBoxQuad.Image = global::ArdupilotMega.Properties.Resources.quad;
            resources.ApplyResources(this.pictureBoxQuad, "pictureBoxQuad");
            this.pictureBoxQuad.Name = "pictureBoxQuad";
            this.pictureBoxQuad.TabStop = false;
            this.pictureBoxQuad.Click += new System.EventHandler(this.pictureBoxQuad_Click);
            // 
            // BUT_levelac2
            // 
            resources.ApplyResources(this.BUT_levelac2, "BUT_levelac2");
            this.BUT_levelac2.Name = "BUT_levelac2";
            this.BUT_levelac2.UseVisualStyleBackColor = true;
            this.BUT_levelac2.Click += new System.EventHandler(this.BUT_levelac2_Click);
            // 
            // ConfigAccelerometerCalibration
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.label28);
            this.Controls.Add(this.label16);
            this.Controls.Add(this.label15);
            this.Controls.Add(this.pictureBoxQuadX);
            this.Controls.Add(this.pictureBoxQuad);
            this.Controls.Add(this.BUT_levelac2);
            this.Name = "ConfigAccelerometerCalibration";
            this.Load += new System.EventHandler(this.ConfigAccelerometerCalibration_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxQuadX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxQuad)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.PictureBox pictureBoxQuadX;
        private System.Windows.Forms.PictureBox pictureBoxQuad;
        private MyButton BUT_levelac2;
    }
}
