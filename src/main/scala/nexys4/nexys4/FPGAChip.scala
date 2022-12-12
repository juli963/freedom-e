// See LICENSE for license details.
package sifive.freedom.Nexys4

import Chisel._
import chisel3.core.{attach, Input, Output}
import chisel3.experimental.{withClockAndReset, withClock, RawModule, Analog}

import freechips.rocketchip.config._
import freechips.rocketchip.diplomacy.{LazyModule}
import freechips.rocketchip.devices.debug._

import sifive.blocks.devices.gpio._
import sifive.blocks.devices.spi._
import sifive.blocks.devices.uart._
import sifive.blocks.devices.pinctrl.{BasePin}

import sifive.fpgashells.ip.xilinx.{IBUFG, IOBUF, PULLUP, reset_sys, PowerOnResetFPGAOnly}

import sifive_copy.fpgashells.ip.xilinx.{m_mmcm}
//-------------------------------------------------------------------------
// Nexys4FPGAChip
//-------------------------------------------------------------------------

class Nexys4FPGAChip(implicit override val p: Parameters) extends Nexys4Shell {

  //-----------------------------------------------------------------------
  // Clock divider
  //-----------------------------------------------------------------------
  val slow_clock = Wire(Bool())

  // Divide clock by 256, used to generate 32.768 kHz clock for AON block
  withClockAndReset(clock_8MHz, ~mmcm_locked) {
    val clockToggleReg = RegInit(false.B)
    val (_, slowTick) = Counter(true.B, 256) 
    when (slowTick) {clockToggleReg := ~clockToggleReg}
    slow_clock := clockToggleReg
  }
  //val slow_io = IO(Bool(OUTPUT))
  //slow_io := slow_clock

  //-----------------------------------------------------------------------
  // DUT
  //-----------------------------------------------------------------------
  withClockAndReset(clock_32MHz, ck_rst) {
    val dut = Module(new Nexys4Platform)

    //---------------------------------------------------------------------
    // SPI flash IOBUFs
    //---------------------------------------------------------------------

    IOBUF(qspi_sck, dut.io.pins.qspi.sck)
    IOBUF(qspi_cs,  dut.io.pins.qspi.cs(0))

    IOBUF(qspi_dq(0), dut.io.pins.qspi.dq(0))
    IOBUF(qspi_dq(1), dut.io.pins.qspi.dq(1))
    IOBUF(qspi_dq(2), dut.io.pins.qspi.dq(2))
    IOBUF(qspi_dq(3), dut.io.pins.qspi.dq(3))

    //---------------------------------------------------------------------
    // JTAG IOBUFs
    //---------------------------------------------------------------------

    dut.io.pins.jtag.TCK.i.ival := IBUFG(IOBUF(jd_10).asClock).asUInt


    IOBUF(jd_1, dut.io.pins.jtag.TMS)
    PULLUP(jd_1)

    IOBUF(jd_2, dut.io.pins.jtag.TDI)
    PULLUP(jd_2)

    IOBUF(jd_3, dut.io.pins.jtag.TDO)

    // mimic putting a pullup on this line (part of reset vote)
    SRST_n := IOBUF(jd_4)
    PULLUP(jd_4)

    // jtag reset
    val jtag_power_on_reset = PowerOnResetFPGAOnly(clock_32MHz)
    dut.io.jtag_reset := jtag_power_on_reset

    // debug reset
    dut_ndreset := dut.io.ndreset

    //---------------------------------------------------------------------
    // Assignment to package pins
    //---------------------------------------------------------------------
    // Pins IO0-IO13
    //
    // FTDI UART TX/RX are not connected to ck_io[0,1]
    // the way they are on Arduino boards.  We copy outgoing
    // data to both places, switch 3 (sw[3]) determines whether
    // input to UART comes from FTDI chip or gpio_16 (shield pin PD0)

    val iobuf_uart_txd = Module(new IOBUF())
    iobuf_uart_txd.io.I := dut.io.pins.gpio.pins(16).o.oval
    iobuf_uart_txd.io.T := ~dut.io.pins.gpio.pins(16).o.oe
    attach(iobuf_uart_txd.io.IO, uart_txd_in)

    // gpio(16) input is shared between FTDI TX pin and the Arduino shield pin using SW[3]
    dut.io.pins.gpio.pins(16).i.ival := iobuf_uart_txd.io.O & dut.io.pins.gpio.pins(16).o.ie

    IOBUF(uart_rxd_out, dut.io.pins.gpio.pins(17))

    // Header row 1: PB0-PB5
    IOBUF(btn_mid,  dut.io.pins.gpio.pins(0))  // PWM0(0)
    IOBUF(btn_left,  dut.io.pins.gpio.pins(1))  // PWM0(1)
    IOBUF(btn_right, dut.io.pins.gpio.pins(2))  // SPI CS(0) / PWM0(2)
    IOBUF(btn_down, dut.io.pins.gpio.pins(3))  // SPI MOSI  / PWM0(3)
    IOBUF(btn_up, dut.io.pins.gpio.pins(4))  // SPI MISO


    // Mirror outputs of GPIOs with PWM peripherals to RGB LEDs on Arty
    // assign RGB LED0 R,G,B inputs = PWM0(1,2,3) when iof_1 is active
    IOBUF(led0_r, dut.io.pins.gpio.pins(23))
    IOBUF(led0_g, dut.io.pins.gpio.pins(24))
    IOBUF(led0_b, dut.io.pins.gpio.pins(25))

    // Note that this is the one which is actually connected on the HiFive/Crazy88
    // Board. Same with RGB LED1 R,G,B inputs = PWM1(1,2,3) when iof_1 is active
    IOBUF(led1_r, dut.io.pins.gpio.pins(19))
    IOBUF(led1_g, dut.io.pins.gpio.pins(21))
    IOBUF(led1_b, dut.io.pins.gpio.pins(22))
  
  
    native_error := dut.io.native_error
    fifo_error := dut.io.fifo_error

    dut.io.native_do_check := dut.io.pins.gpio.pins(26).o.oval
    dut.io.fifo_deq := dut.io.pins.gpio.pins(27).o.oval
    dut.io.fifo_enq := dut.io.pins.gpio.pins(28).o.oval


    dram_init_done := dut.io.dram_init_done
    dram_init_error := dut.io.dram_init_error

    dram_intf <> dut.io.dram_intf

    ip_mmcm.io.ADDR.get :=  dut.io.drp.ADDR 
    ip_mmcm.io.CLK.get :=  dut.io.drp.CLK 
    ip_mmcm.io.EN.get :=  dut.io.drp.EN 
    ip_mmcm.io.DI.get :=  dut.io.drp.DI 
    ip_mmcm.io.WE.get :=  dut.io.drp.WE 
    dut.io.drp.RDY := ip_mmcm.io.RDY.get
    dut.io.drp.DO := ip_mmcm.io.DO.get 

    dut.io.dram_clk := clk_100
    dut.io.dram_rst := ck_rst//wNextReset

    val mdio_buf = Module(new IOBUF())
    mdio_buf.io.T := dut.io.mdio.MDIO.tri
    mdio_buf.io.I := dut.io.mdio.MDIO.out
    dut.io.mdio.MDIO.in := mdio_buf.io.O

    val phy_rst_buf = Module(new IOBUF())
    phy_rst_buf.io.T := ~dut.io.phy_rst
    phy_rst_buf.io.I := false.B
    
    attach(mdio, mdio_buf.io.IO)
    attach(phy_rst, phy_rst_buf.io.IO)

    mclk := dut.io.mdio.MDC
    //---------------------------------------------------------------------
    // Unconnected inputs
    //---------------------------------------------------------------------
  dut.io.reset_periph := reset_periph
   /* dut.io.pins.aon.erst_n.i.ival       := ~reset_periph
    dut.io.pins.aon.lfextclk.i.ival     := slow_clock
    dut.io.pins.aon.pmu.vddpaden.i.ival := 1.U*/
  }
}


abstract class Nexys4Shell(implicit val p: Parameters) extends RawModule {

  //-----------------------------------------------------------------------
  // Interface
  //-----------------------------------------------------------------------

  // Clock & Reset
  val CLK100MHZ    = IO(Input(Clock()))
  val proc_rst = IO(Input(Bool()))
  val clk_100 = Wire(Clock())
  clk_100 := IBUFG(CLK100MHZ)
  val ck_rst = Wire(Bool())
  ck_rst := ~proc_rst

  // Dedicated QSPI interface
  val qspi_cs      = IO(Analog(1.W))
  val qspi_sck     = IO(Analog(1.W))
  val qspi_dq      = IO(Vec(4, Analog(1.W)))

  // UART0
  val uart_rxd_out = IO(Analog(1.W))
  val uart_txd_in = IO(Analog(1.W))

  // JD (used for JTAG connection)
  val jd_1 = IO(Analog(1.W))
  val jd_2 = IO(Analog(1.W))
  val jd_3 = IO(Analog(1.W))
  val jd_4 = IO(Analog(1.W))
  val jd_10 = IO(Analog(1.W)) // MRCC


  val btn_mid = IO(Analog(1.W))
  val btn_left = IO(Analog(1.W))
  val btn_right = IO(Analog(1.W))
  val btn_down = IO(Analog(1.W))
  val btn_up = IO(Analog(1.W))


  val led0_r = IO(Analog(1.W))
  val led0_g = IO(Analog(1.W))
  val led0_b = IO(Analog(1.W))
  val led1_r = IO(Analog(1.W))
  val led1_g = IO(Analog(1.W))
  val led1_b = IO(Analog(1.W))

  val native_error = IO(Output(Bool()))
  val fifo_error = IO(Output(Bool()))

  val dram_init_done = IO(Output(Bool()))
  val dram_init_error = IO(Output(Bool()))
  // DRAM RGMII and Transceiver Data

      // DRAM
      val dram_intf = IO(new Bundle{
          // DRAM Interface

          val ddram_a = Output(UInt(13.W))
          val ddram_ba = Output(UInt(3.W))
          val ddram_ras_n = Output(Bool())
          val ddram_cas_n = Output(Bool())
          val ddram_we_n = Output(Bool())
          val ddram_cs_n = Output(UInt(1.W))
          val ddram_dm = Output(UInt(2.W))
          val ddram_dq = (Analog(16.W))
          val ddram_dqs_p = (Analog(2.W))
          val ddram_dqs_n = (Analog(2.W))
          val ddram_clk_p = Output(UInt(1.W))
          val ddram_clk_n = Output(UInt(1.W))
          val ddram_cke = Output(UInt(1.W))
          val ddram_odt = Output(UInt(1.W))
      })
     
  val mdio = IO(Analog(1.W))
  val mclk = IO(Output(Bool()))
  val phy_clk = IO(Output(Bool()))
  val phy_rst = IO(Analog(1.W))

  //-----------------------------------------------------------------------
  // Wire declrations
  //-----------------------------------------------------------------------

  // Note: these frequencies are approximate.
  val clock_8MHz     = Wire(Clock())
  val clock_32MHz    = Wire(Clock())
  val clock_65MHz    = Wire(Clock())
  val clock_125MHz    = Wire(Clock())
  val clock_250MHz    = Wire(Clock())

  val mmcm_locked    = Wire(Bool())

  val reset_core     = Wire(Bool())
  val reset_bus      = Wire(Bool())
  val reset_periph   = Wire(Bool())
  val reset_intcon_n = Wire(Bool())
  val reset_periph_n = Wire(Bool())

  val SRST_n         = Wire(Bool())

  val dut_jtag_TCK   = Wire(Clock())
  val dut_jtag_TMS   = Wire(Bool())
  val dut_jtag_TDI   = Wire(Bool())
  val dut_jtag_TDO   = Wire(Bool())
  val dut_jtag_reset = Wire(Bool())
  val dut_ndreset    = Wire(Bool())

  //val wReset = Wire(Bool())
	//val wNextReset = Wire(Bool())
  //-----------------------------------------------------------------------
  // Clock Generator
  //-----------------------------------------------------------------------
  // Mixed-mode clock generator

  val ip_mmcm = Module(new m_mmcm(3, ResetActiveLow = true, hasDRP = true))

  ip_mmcm.io.clk_in1 := clk_100
  clock_8MHz         := ip_mmcm.io.clk_out1.get  // 8.388 MHz = 32.768 kHz * 256
  clock_65MHz        := ip_mmcm.io.clk_out2.get  // 50 Mhz
  clock_32MHz        := ip_mmcm.io.clk_out3.get  // 50 Mhz
  ip_mmcm.io.resetn.get  := proc_rst
  mmcm_locked        := ip_mmcm.io.locked


  withClockAndReset(clock_32MHz, ck_rst){
    val clk_phy_out = RegInit(false.B)
    clk_phy_out := ~clk_phy_out
    phy_clk := clk_phy_out
  }
  //-----------------------------------------------------------------------
  // System Reset
  //-----------------------------------------------------------------------
  // processor system reset module

  val ip_reset_sys = Module(new reset_sys())

  ip_reset_sys.io.slowest_sync_clk := clock_8MHz
  ip_reset_sys.io.ext_reset_in     := SRST_n
  ip_reset_sys.io.aux_reset_in     := proc_rst
  ip_reset_sys.io.mb_debug_sys_rst := dut_ndreset
  ip_reset_sys.io.dcm_locked       := mmcm_locked

  reset_core                       := ip_reset_sys.io.mb_reset
  reset_bus                        := ip_reset_sys.io.bus_struct_reset
  reset_periph                     := ip_reset_sys.io.peripheral_reset
  reset_intcon_n                   := ip_reset_sys.io.interconnect_aresetn
  reset_periph_n                   := ip_reset_sys.io.peripheral_aresetn

  //-----------------------------------------------------------------------
  // SPI Flash
  //-----------------------------------------------------------------------

  def connectSPIFlash(dut: HasPeripherySPIFlashModuleImp): Unit = {
    val qspiParams = p(PeripherySPIFlashKey)
    if (!qspiParams.isEmpty) {
      val qspi_params = qspiParams(0)
      val qspi_pins = Wire(new SPIPins(() => {new BasePin()}, qspi_params))

      SPIPinsFromPort(qspi_pins,
        dut.qspi(0),
        dut.clock,
        dut.reset,
        syncStages = qspi_params.defaultSampleDel
      )

      IOBUF(qspi_sck, dut.qspi(0).sck)
      IOBUF(qspi_cs,  dut.qspi(0).cs(0))

      (qspi_dq zip qspi_pins.dq).foreach {
        case(a, b) => IOBUF(a,b)
      }
    }
  }

  //---------------------------------------------------------------------
  // Debug JTAG
  //---------------------------------------------------------------------

  def connectDebugJTAG(dut: HasPeripheryDebugModuleImp): SystemJTAGIO = {

    //-------------------------------------------------------------------
    // JTAG Reset
    //-------------------------------------------------------------------

    val jtag_power_on_reset = PowerOnResetFPGAOnly(clock_32MHz)

    dut_jtag_reset := jtag_power_on_reset

    //-------------------------------------------------------------------
    // JTAG IOBUFs
    //-------------------------------------------------------------------

    dut_jtag_TCK  := IBUFG(IOBUF(jd_10).asClock)

    dut_jtag_TMS  := IOBUF(jd_1)
    PULLUP(jd_1)

    dut_jtag_TDI  := IOBUF(jd_2)
    PULLUP(jd_2)

    IOBUF(jd_3, dut_jtag_TDO)

    SRST_n := IOBUF(jd_4)
    PULLUP(jd_4)

    //-------------------------------------------------------------------
    // JTAG PINS
    //-------------------------------------------------------------------

    val djtag     = dut.debug.systemjtag.get

    djtag.jtag.TCK := dut_jtag_TCK
    djtag.jtag.TMS := dut_jtag_TMS
    djtag.jtag.TDI := dut_jtag_TDI
    dut_jtag_TDO   := djtag.jtag.TDO.data

    djtag.mfr_id   := p(JtagDTMKey).idcodeManufId.U(11.W)

    djtag.reset    := dut_jtag_reset
    dut_ndreset    := dut.debug.ndreset

    djtag
  }

  //---------------------------------------------------------------------
  // UART
  //---------------------------------------------------------------------

  def connectUART(dut: HasPeripheryUARTModuleImp): Unit = {
    val uartParams = p(PeripheryUARTKey)
    if (!uartParams.isEmpty) {
      IOBUF(uart_rxd_out, dut.uart(0).txd)
      //IOBUF(uart_txd_in, dut.uart(0).txd)
      dut.uart(0).rxd := IOBUF(uart_txd_in)
    }
  }

}
