// See LICENSE for license details.
package juli.GBEv1.base

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
import sifive.blocks.devices.pwm._

//import sifive.fpgashells.shell.xilinx.artyshell.{ArtyShell}
import sifive.fpgashells.ip.xilinx.{IBUFG, IOBUF, PULLUP, reset_sys, PowerOnResetFPGAOnly}

import sifive_copy.fpgashells.ip.xilinx._

import Ethernet.Interface.Types._
//-------------------------------------------------------------------------
// GBEv1FPGAChip
//-------------------------------------------------------------------------

class GBEv1FPGAChip(implicit override val p: Parameters) extends GBEv1Shell {

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
    wReset := outreg
    when(countreg >= 1000000.U){
      //countreg := 0.U
      outreg := false.B
    }.otherwise{
      countreg := countreg + 1.U
    }
  }

  
  val RGMII  = IO(new RGMII_Interface())
  val PHY_nrst = IO(Bool(OUTPUT))
  //-----------------------------------------------------------------------
  // DUT
  //-----------------------------------------------------------------------
  withClockAndReset(clock_32MHz, wReset) {
    val dut = Module(new GBEv1Platform)

    
    //---------------------------------------------------------------------
    //
    //---------------------------------------------------------------------
    /*val RGMII  = IO(new Ethernet.Interface.Types.RGMII_Interface())
    val PHY_nrst = IO(Output(Bool()))
    val EthernetClock125 = IO(Input(Clock()))
    val EthernetClock250 = IO(Input(Clock()))*/

    

    RGMII <> dut.io.RGMII
    PHY_nrst := dut.io.PHY_nrst
    dut.io.EthernetClock125 := clock_125MHz
    dut.io.EthernetClock250 := clock_250MHz


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

    dut.io.pins.jtag.TCK.i.ival := IBUFG(IOBUF(jd_2).asClock).asUInt

    IOBUF(jd_5, dut.io.pins.jtag.TMS)
    PULLUP(jd_5)

    IOBUF(jd_4, dut.io.pins.jtag.TDI)
    PULLUP(jd_4)

    IOBUF(jd_0, dut.io.pins.jtag.TDO)

    // mimic putting a pullup on this line (part of reset vote)
    SRST_n := IOBUF(jd_6)
    PULLUP(jd_6)

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

    IOBUF(uart_rxd_out, dut.io.pins.gpio.pins(2))


    // Mirror outputs of GPIOs with PWM peripherals to RGB LEDs on Arty
    // assign RGB LED0 R,G,B inputs = PWM0(1,2,3) when iof_1 is active
    IOBUF(led0_r, dut.io.pins.gpio.pins(3))
    IOBUF(led0_g, dut.io.pins.gpio.pins(4))
    IOBUF(led0_b, dut.io.pins.gpio.pins(5))

    // Note that this is the one which is actually connected on the HiFive/Crazy88
    // Board. Same with RGB LED1 R,G,B inputs = PWM1(1,2,3) when iof_1 is active
    IOBUF(led1_r, dut.io.pins.gpio.pins(6))
    IOBUF(led1_g, dut.io.pins.gpio.pins(7))
    IOBUF(led1_b, dut.io.pins.gpio.pins(8))



    IOBUF(i2c_scl, dut.io.pins.gpio.pins(1))
    IOBUF(i2c_sda, dut.io.pins.gpio.pins(0))

    //---------------------------------------------------------------------
    // Unconnected inputs
    //---------------------------------------------------------------------
  }
}

abstract class GBEv1Shell(implicit val p: Parameters) extends RawModule {

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

  val i2c_sda       = IO(Analog(1.W))
  val i2c_scl       = IO(Analog(1.W))

  // Dedicated QSPI interface
  val qspi_cs      = IO(Analog(1.W))
  val qspi_sck     = IO(Analog(1.W))
  val qspi_dq      = IO(Vec(4, Analog(1.W)))

  // UART0
  val uart_rxd_out = IO(Analog(1.W))

  // JD (used for JTAG connection)
  val jd_0         = IO(Analog(1.W))  // TDO
  val jd_1         = IO(Analog(1.W))  // TRST_n
  val jd_2         = IO(Analog(1.W))  // TCK
  val jd_4         = IO(Analog(1.W))  // TDI
  val jd_5         = IO(Analog(1.W))  // TMS
  val jd_6         = IO(Analog(1.W))  // SRST_n


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

    dut_jtag_TCK  := IBUFG(IOBUF(jd_2).asClock)

    dut_jtag_TMS  := IOBUF(jd_5)
    PULLUP(jd_5)

    dut_jtag_TDI  := IOBUF(jd_4)
    PULLUP(jd_4)

    IOBUF(jd_0, dut_jtag_TDO)

    SRST_n := IOBUF(jd_6)
    PULLUP(jd_6)

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
      dut.uart(0).rxd := false.B
    }
  }

}
