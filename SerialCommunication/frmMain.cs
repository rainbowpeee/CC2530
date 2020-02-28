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
        byte[] buff_data_ = new byte [7];
        long num_rx = 0;
        long num_rx_old = 0;
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
                port.Close();
                this.btnRead.Enabled = true;
                this.btnStopRead.Enabled = false;
            }
            if ((buff_data_[5] == 'a') && (buff_data_[6] == 'b'))
            {
                num_rx = ((buff_data_[0] - 48) * 10000) + ((buff_data_[1] - 48) * 1000) + ((buff_data_[2] - 48) * 100) + ((buff_data_[3] - 48) * 10) + (buff_data_[4] - 48);
                if(num_rx < 0)
                {
                    return RX_dat_ = num_rx_old.ToString();
                }
                else
                if (num_rx < 50)
                {
                    label2.Text = "今天还没动噢";
                }
                else if ((num_rx > 50) && (num_rx < 100))
                {
                    label2.Text = "挪了窝";
                }
                else if ((num_rx > 100) && (num_rx < 200))
                {
                    label2.Text = "可能动了几下";
                }
                else if ((num_rx > 200) && (num_rx < 500))
                {
                    label2.Text = "走了几步";
                }
                else if ((num_rx > 500) && (num_rx < 800))
                {
                    label2.Text = "散步猪";
                }
                else if ((num_rx > 800) && (num_rx < 1200))
                {
                    label2.Text = "运动猪";
                }
                else if ((num_rx > 1200) && (num_rx < 2000))
                {
                    label2.Text = "活泼猪";
                }
                else if ((num_rx > 2000) && (num_rx < 3000))
                {
                    label2.Text = "兴奋猪";
                }
                else if ((num_rx > 3000) && (num_rx < 5000))
                {
                    label2.Text = "暴走猪";
                }
                else if ((num_rx > 5000) && (num_rx < 10000))
                {
                    label2.Text = "所向披靡";
                }
                else if (num_rx > 10000)
                {
                    label2.Text = "无猪能及";
                }
                num_rx_old = num_rx;
                RX_dat_ = num_rx.ToString();
                return RX_dat_;
            }
            else
            {
                return RX_dat_ = "null";
            }
        }

        /// <summary>
        /// 在读取到数据时刷新文本框的信息
        /// </summary>
        private void RefreshInfoTextBox()
        {
            string value = this.ReadSerialData();
            txtInfo.Text = value;
        }
    }
}
