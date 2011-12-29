﻿using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.IO;

namespace ArdupilotMega
{
    class SerialPort : System.IO.Ports.SerialPort,ICommsSerial
    {


        public void toggleDTR()
        {
            DtrEnable = true;
            System.Threading.Thread.Sleep(100);
            DtrEnable = false;
        }
    }
}
