using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SerialCommunication
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
            Control.CheckForIllegalCrossThreadCalls = false;
        }


        private void frmMain_Load(object sender, EventArgs e)
        {
            this.cmbSerials.Items.AddRange(SerialPort.GetPortNames());
            this.cmbSerials.SelectedIndex = this.cmbSerials.Items.Count - 1;//Arduino一般在最后一个串口

            this.btnRead.Enabled = true;
            this.btnStopRead.Enabled = false;
        }

        void port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            this.RefreshInfoTextBox();
        }

        private void btnStartRead_Click(object sender, EventArgs e)
        {
            this.InitialSerialPort();

            this.btnRead.Enabled = false;
            this.btnStopRead.Enabled = true;
        }

        private void btnStopRead_Click(object sender, EventArgs e)
        {
            this.DisposeSerialPort();

            this.btnStopRead.Enabled = false;
            this.btnRead.Enabled = true;
        }


        private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.DisposeSerialPort();
        }

        private SerialPort port = null;
        /// <summary>
        /// 初始化串口实例
        /// </summary>
        private void InitialSerialPort()
        {
            try
            {
                string portName = this.cmbSerials.SelectedItem.ToString();
                port = new SerialPort(portName, 115200);
                port.Encoding = Encoding.ASCII;
                port.DataReceived += port_DataReceived;
                port.Open();
            }
            catch (Exception ex)
            {
                MessageBox.Show("初始化串口发生错误：" + ex.Message, "提示信息", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// 关闭并销毁串口实例
        /// </summary>
        private void DisposeSerialPort()
        {
            if (port != null)
            {
                try
                {
                    if (port.IsOpen)
                    {
                        port.Close();
                    }
                    port.Dispose();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("关闭串口发生错误：" + ex.Message, "提示信息", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }


        /// <summary>
        /// 从串口读取数据并转换为字符串形式
        /// </summary>
        /// <returns></returns>
        /// char
        byte[] buff_data_ = new byte [5];
        long num_rx = 0;
        private string ReadSerialData()
        {
            string RX_dat_;
            int value;
            try
            {
                if (port != null && port.BytesToRead > 0)
                {
                    value = port.Read(buff_data_, 0, port.BytesToRead);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("读取串口数据发生错误：" + ex.Message, "提示信息", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            num_rx =  (buff_data_[0]*10)+ buff_data_[1];
         
            string RX_dat = num_rx.ToString();
            return RX_dat;
        }

        /// <summary>
        /// 在读取到数据时刷新文本框的信息
        /// </summary>
        private void RefreshInfoTextBox()
        {
            string value = this.ReadSerialData();
            txtInfo.Text = value;
        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (port.IsOpen)
            {
                String TX_dat = "ab" + textBox1.Text + textBox2.Text;
                port.Write(TX_dat);
            }
            else
            {
                MessageBox.Show("串口发生错误");
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
