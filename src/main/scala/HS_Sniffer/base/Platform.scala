// See LICENSE for license details.
package juli.HSSnifferv1.base

import Chisel._

import freechips.rocketchip.config._
import freechips.rocketchip.subsystem._
import freechips.rocketchip.devices.debug._
import freechips.rocketchip.devices.tilelink._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.util.ResetCatchAndSync
import freechips.rocketchip.system._

import sifive.blocks.devices.mockaon._
import sifive.blocks.devices.gpio._
import sifive.blocks.devices.jtag._
import sifive.blocks.devices.spi._
import sifive.blocks.devices.uart._
import sifive.blocks.devices.i2c._
import sifive.blocks.devices.pinctrl._

import device.hssniffer.{hssniffer}
import Ethernet.Protocol.ApplUDP.{UDP_Base_noCrossbar}
import chisel3.core.{ Input, Output}
import chisel3.experimental.{ Analog, withClockAndReset, withClock}


//-------------------------------------------------------------------------
// PinGen
//-------------------------------------------------------------------------

object PinGen {
  def apply(): BasePin =  {
    val pin = new BasePin()
    pin
  }
}

//-------------------------------------------------------------------------
// E300ArtyDevKitPlatformIO
//-------------------------------------------------------------------------

class HSSnifferv1PlatformIO(implicit val p: Parameters) extends Bundle {
  val pins = new Bundle {
    val jtag = new JTAGPins(() => PinGen(), false)
    val gpio = new GPIOPins(() => PinGen(), p(PeripheryGPIOKey)(0))
    val qspi = new SPIPins(() => PinGen(), p(PeripherySPIFlashKey)(0))
  }
  val jtag_reset = Bool(INPUT)
  val ndreset    = Bool(OUTPUT)
	val i2c_sda_tx	= Vec(2,Bool(OUTPUT))
	val i2c_sda_tx_en	= Vec(2,Bool(OUTPUT))
	val i2c_sda_rx	= Vec(2,Bool(INPUT))
	val i2c_scl_tx	= Vec(2,Bool(OUTPUT))
	val i2c_scl_tx_en	= Vec(2,Bool(OUTPUT))
	val i2c_scl_rx	= Vec(2,Bool(INPUT))

  // RGMII MAC
    val clk_125 = Clock(INPUT)
    val clk_250 = Clock(INPUT)

    val RGMII = new Ethernet.Interface.Types.RGMII_Interface()
    val PHY_nrst = (Bool(OUTPUT))

  // HSSniffer Interface

    val refclkp = (Vec(2, Clock(INPUT)))
    val refclkn = (Vec(2, Clock(INPUT)))

    val txp = (Vec(4, Bool(OUTPUT)))
    val txn = (Vec(4, Bool(OUTPUT)))
    val rxp = (Vec(4, Bool(INPUT)))
    val rxn = (Vec(4, Bool(INPUT)))

    // DRAM
    val dram_clk = (Clock(INPUT))    // 100MHz
    val dram_rst = (Bool(INPUT))
    val dram_pll_locked = (Bool(OUTPUT))
    val dram_init_done = (Bool(OUTPUT))
    val dram_init_error = (Bool(OUTPUT))

    val dram_intf = new Bundle{


        // DRAM Interface
        val ddram_a = Output(UInt(14.W))
        val ddram_ba = Output(UInt(3.W))
        val ddram_ras_n = (Bool(OUTPUT))
        val ddram_cas_n = (Bool(OUTPUT))
        val ddram_we_n = (Bool(OUTPUT))
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
    }


}

//-------------------------------------------------------------------------
// E300ArtyDevKitPlatform
//-------------------------------------------------------------------------

class HSSnifferv1Platform(implicit val p: Parameters) extends Module {
  val sys = Module(LazyModule(new HSSnifferv1System).module)
  val io = new HSSnifferv1PlatformIO

  // This needs to be de-asserted synchronously to the coreClk.
  //val async_corerst = sys.aon.rsts.corerst
  // Add in debug-controlled reset.
  val countreg = RegInit(0.U(8.W))
  val outreg = RegInit(true.B)
  when(countreg >= 200.U){
    //countreg := 0.U
    outreg := false.B
  }.otherwise{
    countreg := countreg + 1.U
  }
  sys.reset := ResetCatchAndSync(clock, outreg, 20)

  //-----------------------------------------------------------------------
  // Check for unsupported rocket-chip connections
  //-----------------------------------------------------------------------

  require (p(NExtTopInterrupts) == 0, "No Top-level interrupts supported");

  /* RGMII MAC Interface */
    val mod_udp = Module(new UDP_Base_noCrossbar(true, 2))

    val wReset = Wire(Bool())
    withClock(io.clk_125){
      val reg_reset = RegNext(RegNext(RegNext(sys.reset)))
          wReset := reg_reset
    }
    mod_udp.reset := wReset    

    mod_udp.io.clk_125 := io.clk_125
    mod_udp.io.clk_250 := io.clk_250

    io.RGMII <> mod_udp.io.RGMII
    io.PHY_nrst := mod_udp.io.PHY_nrst
    mod_udp.io.PHY_Stats.get.Speed := 2.U   // GBit 
    mod_udp.io.Params.MAC := "h111213141516".U
    mod_udp.io.Params.IP := "hC0A802D4".U


  /* HSSniffer Module */
    val mod_hssniffer = Module(new hssniffer(num_channels = 4, num_refclk = 2, PipeWidth = 32,  num_triggers = 2, num_protocols = 1, DRAM_DW = 128) )
    
    //mod_hssniffer.reset := ResetCatchAndSync(clock, outreg, 20)

    mod_hssniffer.io.refclkp := io.refclkp
    mod_hssniffer.io.refclkn := io.refclkn

    io.txp := mod_hssniffer.io.txp 
    io.txn := mod_hssniffer.io.txn 
    mod_hssniffer.io.rxp := io.rxp
    mod_hssniffer.io.rxn := io.rxn

    //mod_hssniffer.io.DRP <> io.dram_intf
    mod_hssniffer.io.DRP.ADDR := sys.drp_mem_io.get(0).ADDR 
    mod_hssniffer.io.DRP.CLK := sys.drp_mem_io.get(0).CLK 
    mod_hssniffer.io.DRP.EN := sys.drp_mem_io.get(0).EN
    mod_hssniffer.io.DRP.DI := sys.drp_mem_io.get(0).DI 
    mod_hssniffer.io.DRP.WE := sys.drp_mem_io.get(0).WE
    sys.drp_mem_io.get(0).RDY := mod_hssniffer.io.DRP.RDY
    sys.drp_mem_io.get(0).DO := mod_hssniffer.io.DRP.DO

    //mod_hssniffer.io.DRP <> sys.drp_mem_io.get(0)

    //sys.HSControl_io.get(0).gtp_regs <> mod_hssniffer.io.gtp_regs
    mod_hssniffer.io.gtp_regs.gtp_sel := sys.HSControl_io.get(0).gtp_regs.gtp_sel

  //  sys.HSControl_io.get(0).gtp_regs.pll_ctrl(0).REFCLKLOST := mod_hssniffer.io.gtp_regs.pll_ctrl(0).REFCLKLOST
  //  sys.HSControl_io.get(0).gtp_regs.pll_ctrl(0).LOCK := mod_hssniffer.io.gtp_regs.pll_ctrl(0).LOCK
  //  mod_hssniffer.io.gtp_regs.pll_ctrl(0).REFCLKSEL := sys.HSControl_io.get(0).gtp_regs.pll_ctrl(0).REFCLKSEL

  //  sys.HSControl_io.get(0).gtp_regs.pll_ctrl(1).REFCLKLOST := mod_hssniffer.io.gtp_regs.pll_ctrl(1).REFCLKLOST
  //  sys.HSControl_io.get(0).gtp_regs.pll_ctrl(1).LOCK := mod_hssniffer.io.gtp_regs.pll_ctrl(1).LOCK
  //  mod_hssniffer.io.gtp_regs.pll_ctrl(1).REFCLKSEL := sys.HSControl_io.get(0).gtp_regs.pll_ctrl(1).REFCLKSEL

  //  sys.HSControl_io.get(0).gtp_regs.plls_ctrl(0).REFCLKLOST := false.B
  //  sys.HSControl_io.get(0).gtp_regs.plls_ctrl(0).LOCK := false.B
  //  mod_hssniffer.io.gtp_regs.pll_ctrl(0).REFCLKSEL := 0.U

  //  sys.HSControl_io.get(0).gtp_regs.plls_ctrl(1).REFCLKLOST := false.B
  //  sys.HSControl_io.get(0).gtp_regs.plls_ctrl(1).LOCK := false.B
  //  mod_hssniffer.io.gtp_regs.pll_ctrl(1).REFCLKSEL := 0.U


    mod_hssniffer.io.gtp_regs.rx_selclk := sys.HSControl_io.get(0).gtp_regs.rx_selclk
    mod_hssniffer.io.gtp_regs.tx_selclk := sys.HSControl_io.get(0).gtp_regs.tx_selclk
	  sys.HSControl_io.get(0).gtp_regs.gtp_running := mod_hssniffer.io.gtp_regs.gtp_running
    mod_hssniffer.io.gtp_regs.protocol_sel := sys.HSControl_io.get(0).gtp_regs.protocol_sel

    sys.HSControl_io.get(0).c8b10b_regs <> mod_hssniffer.io.c8b10b_regs
    sys.HSControl_io.get(0).gtp_clock := clock

    io.dram_intf <> mod_hssniffer.io.dram_intf
        
    mod_hssniffer.io.dram_clk := io.dram_clk
    mod_hssniffer.io.dram_rst := io.dram_rst

    io.dram_pll_locked := mod_hssniffer.io.dram_pll_locked
    io.dram_init_done := mod_hssniffer.io.dram_init_done
    io.dram_init_error := mod_hssniffer.io.dram_init_error

    
    mod_udp.io.Params.MAC := "h111213141517".U
    mod_udp.io.Params.IP := "hC0A802D5".U

    mod_udp.io.udp_slaves(0) <> mod_hssniffer.io.UDPBus
    mod_hssniffer.io.clkUDP := io.clk_125

  /* ESP FIFO Interface */
    mod_udp.io.udp_slaves(1) <> sys.ESP_FIFO_io.get(0).UDPBus
	  sys.ESP_FIFO_io.get(0).clkUDP := io.clk_125

  /* Wishbone Interface */
    mod_hssniffer.io.dram_wb_ctrl <> sys.LiteDRAM_Ctrl_io.get(0).wbus
    sys.LiteDRAM_Ctrl_io.get(0).dram_clock := mod_hssniffer.io.dram_user_clk//io.dram_clk

  //-----------------------------------------------------------------------
  // Build GPIO Pin Mux
  //-----------------------------------------------------------------------
  // Pin Mux for UART, SPI, PWM
  // First convert the System outputs into "IOF" using the respective *GPIOPort
  // converters.

  val sys_uart = sys.uart
  val sys_i2c  = sys.i2c

  val uart_pins = p(PeripheryUARTKey).map { c => Wire(new UARTPins(() => PinGen()))}
  val i2c_pins  = p(PeripheryI2CKey).map  { c => Wire(new I2CPins(() => PinGen()))}

  (uart_pins zip  sys_uart) map {case (p, r) => UARTPinsFromPort(p, r, clock = clock, reset = reset, syncStages = 0)}

  (i2c_pins  zip  sys_i2c)  map {case (p, r) => I2CPinsFromPort(p, r, clock = clock, reset = reset, syncStages = 0)}

  //-----------------------------------------------------------------------
  // Default Pin connections before attaching pinmux

  for (iof_0 <- sys.gpio(0).iof_0.get) {
    iof_0.default()
  }

  for (iof_1 <- sys.gpio(0).iof_1.get) {
    iof_1.default()
  }

  //-----------------------------------------------------------------------

  val iof_0 = sys.gpio(0).iof_0.get
  val iof_1 = sys.gpio(0).iof_1.get


	BasePinToIOF(i2c_pins(0).sda, iof_0(0))
	BasePinToIOF(i2c_pins(0).scl, iof_0(1))
	BasePinToIOF(i2c_pins(1).sda, iof_0(2))
	BasePinToIOF(i2c_pins(1).scl, iof_0(3))

	
	io.i2c_sda_tx(0) := i2c_pins(0).sda.o.oval
	io.i2c_sda_tx(1) := i2c_pins(1).sda.o.oval
	io.i2c_scl_tx(0) := i2c_pins(0).scl.o.oval
	io.i2c_scl_tx(1) := i2c_pins(1).scl.o.oval
	
	io.i2c_sda_tx(0) := i2c_pins(0).sda.o.oe
	io.i2c_sda_tx(1) := i2c_pins(1).sda.o.oe
	io.i2c_scl_tx(0) := i2c_pins(0).scl.o.oe
	io.i2c_scl_tx(1) := i2c_pins(1).scl.o.oe
	
	i2c_pins(0).sda.i.ival := io.i2c_sda_rx(0)
	i2c_pins(1).sda.i.ival := io.i2c_sda_rx(1)
	i2c_pins(0).scl.i.ival := io.i2c_scl_rx(0)
	i2c_pins(1).scl.i.ival := io.i2c_scl_rx(1)
  // I2C
  /*if (p(PeripheryI2CKey).length == 1) {
    BasePinToIOF(i2c_pins(0).sda, iof_0(0))
    BasePinToIOF(i2c_pins(0).scl, iof_0(1))
  }*/

  // UART0
  BasePinToIOF(uart_pins(0).rxd, iof_0(5))
  BasePinToIOF(uart_pins(0).txd, iof_0(4))


  //-----------------------------------------------------------------------
  // Drive actual Pads
  //-----------------------------------------------------------------------

  // Result of Pin Mux
  GPIOPinsFromPort(io.pins.gpio, sys.gpio(0))

  // Dedicated SPI Pads
  SPIPinsFromPort(io.pins.qspi, sys.qspi(0), clock = sys.clock, reset = sys.reset, syncStages = 3)

  // JTAG Debug Interface
  val sjtag = sys.debug.systemjtag.get
  JTAGPinsFromPort(io.pins.jtag, sjtag.jtag)
  sjtag.reset := io.jtag_reset
  sjtag.mfr_id := p(JtagDTMKey).idcodeManufId.U(11.W)

  io.ndreset := sys.debug.ndreset

}
