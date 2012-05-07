﻿namespace _3DRRadio
{
    partial class Config
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Config));
            this.CMB_SerialPort = new System.Windows.Forms.ComboBox();
            this.CMB_Baudrate = new System.Windows.Forms.ComboBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // CMB_SerialPort
            // 
            this.CMB_SerialPort.FormattingEnabled = true;
            resources.ApplyResources(this.CMB_SerialPort, "CMB_SerialPort");
            this.CMB_SerialPort.Name = "CMB_SerialPort";
            this.CMB_SerialPort.SelectedIndexChanged += new System.EventHandler(this.CMB_SerialPort_SelectedIndexChanged);
            this.CMB_SerialPort.Click += new System.EventHandler(this.CMB_SerialPort_Click);
            // 
            // CMB_Baudrate
            // 
            this.CMB_Baudrate.FormattingEnabled = true;
            this.CMB_Baudrate.Items.AddRange(new object[] {
            resources.GetString("CMB_Baudrate.Items"),
            resources.GetString("CMB_Baudrate.Items1"),
            resources.GetString("CMB_Baudrate.Items2"),
            resources.GetString("CMB_Baudrate.Items3"),
            resources.GetString("CMB_Baudrate.Items4"),
            resources.GetString("CMB_Baudrate.Items5"),
            resources.GetString("CMB_Baudrate.Items6"),
            resources.GetString("CMB_Baudrate.Items7")});
            resources.ApplyResources(this.CMB_Baudrate, "CMB_Baudrate");
            this.CMB_Baudrate.Name = "CMB_Baudrate";
            this.CMB_Baudrate.SelectedIndexChanged += new System.EventHandler(this.CMB_Baudrate_SelectedIndexChanged);
            // 
            // panel1
            // 
            resources.ApplyResources(this.panel1, "panel1");
            this.panel1.MinimumSize = new System.Drawing.Size(781, 433);
            this.panel1.Name = "panel1";
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.CMB_SerialPort);
            this.groupBox1.Controls.Add(this.CMB_Baudrate);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.label2);
            resources.ApplyResources(this.groupBox1, "groupBox1");
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.TabStop = false;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.panel1);
            resources.ApplyResources(this.groupBox2, "groupBox2");
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.TabStop = false;
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackgroundImage = global::_3DRRadio.Properties.Resources._3dr3;
            resources.ApplyResources(this.pictureBox1, "pictureBox1");
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.TabStop = false;
            // 
            // Config
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.pictureBox1);
            this.Name = "Config";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox CMB_SerialPort;
        private System.Windows.Forms.ComboBox CMB_Baudrate;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
    }
}

