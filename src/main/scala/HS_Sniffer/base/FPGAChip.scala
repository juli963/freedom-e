// See LICENSE for license details.
package juli.HSSnifferv1.base

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

import sifive_copy.fpgashells.ip.xilinx._

//-------------------------------------------------------------------------
// HSSnifferv1FPGAChip
//-------------------------------------------------------------------------

class HSSnifferv1FPGAChip(implicit override val p: Parameters) extends HSSnifferv1Shell {

  //-----------------------------------------------------------------------
  // Clock divider
  //-----------------------------------------------------------------------
  
  // Divide clock by 256, used to generate 32.768 kHz clock for AON block
  withClockAndReset(clock_8MHz, ~mmcm_locked) {
    val clockToggleReg = RegInit(false.B)
    val (_, slowTick) = Counter(true.B, 256) 
    when (slowTick) {clockToggleReg := ~clockToggleReg}

    val countreg = RegInit(0.U(20.W))
    val outreg = RegInit(true.B)
    wReset := RegNext(outreg)
    when(countreg >= 1000000.U){
      //countreg := 0.U
      outreg := false.B
    }.otherwise{
      countreg := countreg + 1.U
    }
  }

  withClockAndReset(clock_32MHz, false.B){
  	wNextReset := RegNext(RegNext(wReset))
  }
  //-----------------------------------------------------------------------
  // DUT
  //-----------------------------------------------------------------------
  withClockAndReset(clock_32MHz, wNextReset) {
    val dut = Module(new HSSnifferv1Platform)

    
    //---------------------------------------------------------------------
    // GPIO Connections
    //---------------------------------------------------------------------

   


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

    dut.io.pins.jtag.TCK.i.ival := IBUFG(IOBUF(jtag_tck).asClock).asUInt

    IOBUF(jtag_tms, dut.io.pins.jtag.TMS)
    PULLUP(jtag_tms)

    IOBUF(jtag_tdi, dut.io.pins.jtag.TDI)
    PULLUP(jtag_tdi)

    IOBUF(jtag_tdo, dut.io.pins.jtag.TDO)

    // mimic putting a pullup on this line (part of reset vote)
    SRST_n := IOBUF(jtag_srst)
    PULLUP(jtag_srst)

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

		// UART Interface
    IOBUF(uart_rxd_out, dut.io.pins.gpio.pins(4))
    IOBUF(uart_txd_in, dut.io.pins.gpio.pins(5))

		// LED Pins 
    IOBUF(led0_r, dut.io.pins.gpio.pins(6))
    IOBUF(led0_g, dut.io.pins.gpio.pins(7))
    IOBUF(led0_b, dut.io.pins.gpio.pins(8))
    IOBUF(led1_r, dut.io.pins.gpio.pins(9))
    IOBUF(led1_g, dut.io.pins.gpio.pins(10))
    IOBUF(led1_b, dut.io.pins.gpio.pins(11))

		// I2C Pins
    IOBUF(eep_scl, dut.io.pins.gpio.pins(1))
    IOBUF(eep_sda, dut.io.pins.gpio.pins(0))
    IOBUF(clk_scl, dut.io.pins.gpio.pins(3))
    IOBUF(clk_sda, dut.io.pins.gpio.pins(2))

		// MUX/Load Switch IO's
    IOBUF(en_pswitch, dut.io.pins.gpio.pins(12))
    IOBUF(en_3v3, dut.io.pins.gpio.pins(13))
    IOBUF(en_5v, dut.io.pins.gpio.pins(14))
    
    IOBUF(dev_mux_oe, dut.io.pins.gpio.pins(15))
    IOBUF(dev_mux_sel, dut.io.pins.gpio.pins(16))
    
    IOBUF(host_mux_oe, dut.io.pins.gpio.pins(17))
    IOBUF(host_mux_sel, dut.io.pins.gpio.pins(18))

    IOBUF(clock_mux_oe, dut.io.pins.gpio.pins(19))
    IOBUF(clock_mux_sel, dut.io.pins.gpio.pins(20))
    
    IOBUF(dev_usb_oe, dut.io.pins.gpio.pins(21))
    IOBUF(host_usb_oe, dut.io.pins.gpio.pins(22))

	  
	  // CC Multiplexer
	  //dut.io.pins.gpio.pins(23)	// Device
	  dev_cc1_tx := Mux(dut.io.pins.gpio.pins(23).o.oval, dut.io.i2c_sda_tx(0), true.B)
	  dev_cc2_tx := Mux(dut.io.pins.gpio.pins(23).o.oval, dut.io.i2c_scl_tx(0), true.B)
	  dev_cc1_tx_en := Mux(dut.io.pins.gpio.pins(23).o.oval, dut.io.i2c_sda_tx_en(0), true.B)
	  dev_cc2_tx_en := Mux(dut.io.pins.gpio.pins(23).o.oval, dut.io.i2c_scl_tx_en(0), true.B)
	  dut.io.i2c_sda_rx(0) := Mux(dut.io.pins.gpio.pins(23).o.oval, dev_cc1_rx, true.B)
	  dut.io.i2c_scl_rx(0) := Mux(dut.io.pins.gpio.pins(23).o.oval, dev_cc2_rx, true.B)
	  
	  host_cc1_tx := Mux(dut.io.pins.gpio.pins(24).o.oval, dut.io.i2c_sda_tx(1), true.B)
	  host_cc2_tx := Mux(dut.io.pins.gpio.pins(24).o.oval, dut.io.i2c_scl_tx(1), true.B)
	  host_cc1_tx_en := Mux(dut.io.pins.gpio.pins(24).o.oval, dut.io.i2c_sda_tx_en(1), true.B)
	  host_cc2_tx_en := Mux(dut.io.pins.gpio.pins(24).o.oval, dut.io.i2c_scl_tx_en(1), true.B)
	  dut.io.i2c_sda_rx(1) := Mux(dut.io.pins.gpio.pins(24).o.oval, host_cc1_rx, true.B)
	  dut.io.i2c_scl_rx(1) := Mux(dut.io.pins.gpio.pins(24).o.oval, host_cc2_rx, true.B)
	  
    //---------------------------------------------------------------------
    // GTP RGMII DRAM
    //---------------------------------------------------------------------
    dut.io.clk_125 := clock_125MHz
    dut.io.clk_250 := clock_250MHz

    RGMII <> dut.io.RGMII
    PHY_nrst := dut.io.PHY_nrst

    dut.io.refclkp := refclkp
    dut.io.refclkn := refclkn

    txp := dut.io.txp
    txn := dut.io.txn

    dut.io.rxp := rxp
    dut.io.rxn := rxn

    dram_intf <> dut.io.dram_intf




    //---------------------------------------------------------------------
    // Unconnected inputs
    //---------------------------------------------------------------------
  }
}

abstract class HSSnifferv1Shell(implicit val p: Parameters) extends RawModule {

  //-----------------------------------------------------------------------
  // Interface
  //-----------------------------------------------------------------------

  // Clock & Reset
  val CLK100MHZ    = IO(Input(Clock()))

  // RGB LEDs, 3 pins each
  val led0_r       = IO(Analog(1.W))
  val led0_g       = IO(Analog(1.W))
  val led0_b       = IO(Analog(1.W))

  val led1_r       = IO(Analog(1.W))
  val led1_g       = IO(Analog(1.W))
  val led1_b       = IO(Analog(1.W))

	// I2C Interface
  val eep_sda       = IO(Analog(1.W))
  val eep_scl       = IO(Analog(1.W))
  
  val clk_sda       = IO(Analog(1.W))
  val clk_scl       = IO(Analog(1.W))

	// USB C CC Interface
	val dev_cc1_tx 	= IO(Output(Bool()))
	val dev_cc1_tx_en= IO(Output(Bool()))
	val dev_cc1_rx	= IO(Input(Bool()))
	val dev_cc2_tx 	= IO(Output(Bool()))
	val dev_cc2_tx_en= IO(Output(Bool()))
	val dev_cc2_rx	= IO(Input(Bool()))
	
	val host_cc1_tx 	= IO(Output(Bool()))
	val host_cc1_tx_en= IO(Output(Bool()))
	val host_cc1_rx	= IO(Input(Bool()))
	val host_cc2_tx 	= IO(Output(Bool()))
	val host_cc2_tx_en= IO(Output(Bool()))
	val host_cc2_rx	= IO(Input(Bool()))
	// MUX/Load Switch IO's
  val en_pswitch    = IO(Analog(1.W))
  val en_3v3        = IO(Analog(1.W))
  val en_5v         = IO(Analog(1.W))
  
  val dev_mux_oe    = IO(Analog(1.W))
  val dev_mux_sel   = IO(Analog(1.W))
  
  val host_mux_oe   = IO(Analog(1.W))
  val host_mux_sel  = IO(Analog(1.W))
  
  val clock_mux_oe   = IO(Analog(1.W))
  val clock_mux_sel  = IO(Analog(1.W))
  
  val dev_usb_oe   = IO(Analog(1.W))
  val host_usb_oe  = IO(Analog(1.W))

  // Dedicated QSPI interface
  val qspi_cs      = IO(Analog(1.W))
  val qspi_sck     = IO(Analog(1.W))
  val qspi_dq      = IO(Vec(4, Analog(1.W)))

  // UART0
  val uart_rxd_out = IO(Analog(1.W))
  val uart_txd_in = IO(Analog(1.W))

  // JD (used for JTAG connection)
  val jtag_tdo         = IO(Analog(1.W))  // TDO
  //val jd_1         = IO(Analog(1.W))  // TRST_n
  val jtag_tck         = IO(Analog(1.W))  // TCK
  val jtag_tdi         = IO(Analog(1.W))  // TDI
  val jtag_tms         = IO(Analog(1.W))  // TMS
  val jtag_srst         = IO(Analog(1.W))  // SRST_n


  // DRAM RGMII and Transceiver Data
    // RGMII MAC
      val RGMII = IO(new Ethernet.Interface.Types.RGMII_Interface())
      val PHY_nrst = IO(Output(Bool()))

    // HSSniffer Interface

      val refclkp = IO(Input(Vec(2, Clock())))
      val refclkn = IO(Input(Vec(2, Clock())))

      val txp = IO(Output(Vec(4, Bool())))
      val txn = IO(Output(Vec(4, Bool())))
      val rxp = IO(Input(Vec(4, Bool())))
      val rxn = IO(Input(Vec(4, Bool())))

      // DRAM
      val dram_intf = IO(new Bundle{
          val clk = Input(Clock())    // 100MHz
          val rst = Input(Bool())
          val pll_locked = Output(Bool())
          val init_done = Output(Bool())
          val init_error = Output(Bool())

          // DRAM Interface
          val ddram_a = Output(UInt(14.W))
          val ddram_ba = Output(UInt(3.W))
          val ddram_ras_n = Output(Bool())
          val ddram_cas_n = Output(Bool())
          val ddram_we_n = Output(Bool())
          val ddram_cs_n = Output(UInt(1.W))
          val ddram_dm = Output(UInt(2.W))
          val ddram_dq = Analog((2*8).W)
          val ddram_dqs_p = Analog(2.W)
          val ddram_dqs_n = Analog(2.W)
          val ddram_clk_p = Output(UInt(1.W))
          val ddram_clk_n = Output(UInt(1.W))
          val ddram_cke = Output(UInt(1.W))
          val ddram_odt = Output(UInt(1.W))
          val ddram_reset_n = Output(Bool())
      })


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

  val wReset = Wire(Bool())
	val wNextReset = Wire(Bool())
  //-----------------------------------------------------------------------
  // Clock Generator
  //-----------------------------------------------------------------------
  // Mixed-mode clock generator

  val ip_mmcm = Module(new m_mmcm(5, ResetActiveLow = true))

  ip_mmcm.io.clk_in1 := CLK100MHZ
  clock_8MHz         := ip_mmcm.io.clk_out1.get  // 8.388 MHz = 32.768 kHz * 256
  clock_65MHz        := ip_mmcm.io.clk_out2.get  // 65 Mhz
  clock_32MHz        := ip_mmcm.io.clk_out3.get  // 65/2 Mhz
  clock_125MHz        := ip_mmcm.io.clk_out4.get  // 65/2 Mhz
  clock_250MHz        := ip_mmcm.io.clk_out5.get  // 65/2 Mhz
  ip_mmcm.io.resetn.get  := true.B
  mmcm_locked        := ip_mmcm.io.locked

  //-----------------------------------------------------------------------
  // System Reset
  //-----------------------------------------------------------------------
  // processor system reset module

  val ip_reset_sys = Module(new reset_sys())

  ip_reset_sys.io.slowest_sync_clk := clock_8MHz
  ip_reset_sys.io.ext_reset_in     := SRST_n
  ip_reset_sys.io.aux_reset_in     := ~wReset
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

    dut_jtag_TCK  := IBUFG(IOBUF(jtag_tck).asClock)

    dut_jtag_TMS  := IOBUF(jtag_tms)
    PULLUP(jtag_tms)

    dut_jtag_TDI  := IOBUF(jtag_tdi)
    PULLUP(jtag_tdi)

    IOBUF(jtag_tdo, dut_jtag_TDO)

    SRST_n := IOBUF(jtag_srst)
    PULLUP(jtag_srst)

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
      IOBUF(uart_txd_in, dut.uart(0).txd)
    }
  }

}
