﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ArdupilotMega.Controls.BackstageView;

namespace ArdupilotMega.GCSViews.ConfigurationView
{
    public partial class Setup : Form
    {
        public Setup()
        {
            InitializeComponent();

            this.backstageView.AddPage(new BackstageView.BackstageViewPage(new ConfigRadioInput(), "Radio Calibration"));
            this.backstageView.AddPage(new BackstageView.BackstageViewPage(new ConfigFlightModes(), "Flight Modes"));
            this.backstageView.AddPage(new BackstageView.BackstageViewPage(new ConfigHardwareOptions(), "Hardware Options"));
            this.backstageView.AddPage(new BackstageView.BackstageViewPage(new ConfigBatteryMonitoring(), "Battery Monitor"));
            this.backstageView.AddPage(new BackstageView.BackstageViewPage(new ConfigAccelerometerCalibration(), "Level Calibration"));
            this.backstageView.AddPage(new BackstageView.BackstageViewPage(new ConfigTradHeli(), "Heli Setup"));
        }
    }
}
