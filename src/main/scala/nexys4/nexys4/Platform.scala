// See LICENSE for license details.
package sifive.freedom.Nexys4
import chisel3._
//import Chisel._
import chisel3.core.{attach, Input, Output}
import chisel3.experimental.{Analog}

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
import sifive.blocks.devices.pwm._
import sifive.blocks.devices.spi._
import sifive.blocks.devices.uart._
import sifive.blocks.devices.i2c._
import sifive.blocks.devices.pinctrl._
import Memory.DRAM.{litedram_wrapper}
import Xilinx.{DRP_Bundle}
import sifive_copy.fpgashells.ip.xilinx.{BUFG}

// export RISCV=../../../../../../Firmware/RISC-V/freedom-e-sdk/work/build/riscv-gnu-toolchain/riscv64-unknown-elf/prefix 
// make -f Makefile.nexys4base verilog
// make -f Makefile.nexys4base romgen
// Important constraints:
// set_clock_groups -asynchronous -group [get_clocks main_clkout1] -group [get_clocks clk_out3_m_mmcm]
// set_false_path -to [get_pins -filter {REF_PIN_NAME == PRE} -of_objects [get_cells -hierarchical -filter {ars_ff1 == TRUE || ars_ff2 == TRUE}]] -quiet
// set_max_delay -from [get_pins -filter {REF_PIN_NAME == C} -of_objects [get_cells -hierarchical -filter {ars_ff1 == TRUE}]] -to [get_pins -filter {REF_PIN_NAME == D} -of_objects [get_cells -hierarchical -filter {ars_ff2 == TRUE}]] 2.000 -quiet


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
// Nexys4PlatformIO
//-------------------------------------------------------------------------

class Nexys4PlatformIO(implicit val p: Parameters) extends Bundle {
  val pins = new Bundle {
    val jtag = new JTAGPins(() => PinGen(), false)
    val gpio = new GPIOPins(() => PinGen(), p(PeripheryGPIOKey)(0))
    val qspi = new SPIPins(() => PinGen(), p(PeripherySPIFlashKey)(0))
  }

  val jtag_reset = Input(Bool())
  val ndreset    = Output(Bool())
  val reset_periph = Input(Bool())

  // DRAM
  val dram_clk = Input(Clock())    // 100MHz
  val dram_rst = Input(Bool())
  val dram_pll_locked = Output(Bool())
  val dram_init_done = Output(Bool())
  val dram_init_error = Output(Bool())

  val dram_intf = new Bundle{
      // DRAM Interface
      val ddram_a = Output(UInt(13.W))
      val ddram_ba = Output(UInt(3.W))
      val ddram_ras_n = Output(Bool())
      val ddram_cas_n = Output(Bool())
      val ddram_we_n = Output(Bool())
      val ddram_cs_n = Output(UInt(1.W))
      val ddram_dm = Output(UInt(2.W))
      val ddram_dq = (Analog(width=16.W))
      val ddram_dqs_p = (Analog(width=2.W))
      val ddram_dqs_n = (Analog(width=2.W))
      val ddram_clk_p = Output(UInt(1.W))
      val ddram_clk_n = Output(UInt(1.W))
      val ddram_cke = Output(UInt(1.W))
      val ddram_odt = Output(UInt(1.W))
     // val ddram_reset_n = Output(Bool())
  }
  val drp = Flipped(new DRP_Bundle(Address_Width = 5))
}

//-------------------------------------------------------------------------
// Nexys4Platform
//-------------------------------------------------------------------------

class Nexys4Platform(implicit val p: Parameters) extends Module {
  val sys = Module(LazyModule(new Nexys4System).module)
  val io = IO(new Nexys4PlatformIO)

  // This needs to be de-asserted synchronously to the coreClk.
  //val async_corerst = sys.aon.rsts.corerst
  // Add in debug-controlled reset.
  sys.reset := ResetCatchAndSync(clock, io.reset_periph, 20)

  //-----------------------------------------------------------------------
  // Check for unsupported rocket-chip connections
  //-----------------------------------------------------------------------

  require (p(NExtTopInterrupts) == 0, "No Top-level interrupts supported");


  //-----------------------------------------------------------------------
  // DRAM Interface
  //-----------------------------------------------------------------------
  lazy val mod_dram = Module(new litedram_wrapper( ResourceName = "litedram_core_nexys4.v", Addrwidth = 13, Bytegroups = 2, Ranks = 1, 
    withAXI = false, withWishbone = false, withNative = true, withFifo = false, Native_DW = 64, Native_AW = 24
  ) )

    io.dram_intf.ddram_a      := mod_dram.io.ddram_a
		io.dram_intf.ddram_ba     := mod_dram.io.ddram_ba
		io.dram_intf.ddram_ras_n  := mod_dram.io.ddram_ras_n
		io.dram_intf.ddram_cas_n  := mod_dram.io.ddram_cas_n
		io.dram_intf.ddram_we_n   := mod_dram.io.ddram_we_n
		io.dram_intf.ddram_cs_n   := mod_dram.io.ddram_cs_n
		io.dram_intf.ddram_dm     := mod_dram.io.ddram_dm


    mod_dram.io.ddram_dq      <> io.dram_intf.ddram_dq
    mod_dram.io.ddram_dqs_p   <> io.dram_intf.ddram_dqs_p
    mod_dram.io.ddram_dqs_n   <> io.dram_intf.ddram_dqs_n

		
		io.dram_intf.ddram_clk_p  := mod_dram.io.ddram_clk_p
		io.dram_intf.ddram_clk_n  := mod_dram.io.ddram_clk_n
		io.dram_intf.ddram_cke    := mod_dram.io.ddram_cke
		io.dram_intf.ddram_odt    := mod_dram.io.ddram_odt
		//io.dram_intf.ddram_reset_n := mod_dram.io.ddram_reset_n
        
    mod_dram.io.clk := io.dram_clk
    mod_dram.io.rst := io.dram_rst

    mod_dram.io.wb_ctrl <> sys.LiteDRAM_Ctrl_io.get(0).wbus
    sys.LiteDRAM_Ctrl_io.get(0).dram_clock := mod_dram.io.user_clk.get
    //sys.LiteDRAM_Ctrl_io.get(0).dram_clock := io.dram_clk

    mod_dram.io.native.get.cmd_valid := false.B
    mod_dram.io.native.get.cmd_we := false.B
    mod_dram.io.native.get.cmd_addr := 0.U
    mod_dram.io.native.get.wdata_valid := false.B
    mod_dram.io.native.get.wdata_we := 0.U
    mod_dram.io.native.get.wdata_data := 0.U
    mod_dram.io.native.get.rdata_ready := false.B

    io.dram_pll_locked := mod_dram.io.pll_locked
    io.dram_init_done := mod_dram.io.init_done
    io.dram_init_error := mod_dram.io.init_error


    io.drp.ADDR := sys.drp_mem_io.get(0).ADDR 
    io.drp.CLK := sys.drp_mem_io.get(0).CLK 
    io.drp.EN := sys.drp_mem_io.get(0).EN
    io.drp.DI := sys.drp_mem_io.get(0).DI 
    io.drp.WE := sys.drp_mem_io.get(0).WE
    sys.drp_mem_io.get(0).RDY := io.drp.RDY
    sys.drp_mem_io.get(0).DO := io.drp.DO

  //-----------------------------------------------------------------------
  // Build GPIO Pin Mux
  //-----------------------------------------------------------------------
  // Pin Mux for UART, SPI, PWM
  // First convert the System outputs into "IOF" using the respective *GPIOPort
  // converters.

  val sys_uart = sys.uart
  val sys_pwm  = sys.pwm
  val sys_spi  = sys.spi
  val sys_i2c  = sys.i2c

  val uart_pins = p(PeripheryUARTKey).map { c => Wire(new UARTPins(() => PinGen()))}
  val pwm_pins  = p(PeripheryPWMKey).map  { c => Wire(new PWMPins(() => PinGen(), c))}
  val spi_pins  = p(PeripherySPIKey).map  { c => Wire(new SPIPins(() => PinGen(), c))}
  val i2c_pins  = p(PeripheryI2CKey).map  { c => Wire(new I2CPins(() => PinGen()))}

  (uart_pins zip  sys_uart) map {case (p, r) => UARTPinsFromPort(p, r, clock = clock, reset = reset.toBool(), syncStages = 0)}
  (pwm_pins  zip  sys_pwm)  map {case (p, r) => PWMPinsFromPort(p, r) }
  (spi_pins  zip  sys_spi)  map {case (p, r) => SPIPinsFromPort(p, r, clock = clock, reset = reset.toBool(), syncStages = 0)}
  (i2c_pins  zip  sys_i2c)  map {case (p, r) => I2CPinsFromPort(p, r, clock = clock, reset = reset.toBool(), syncStages = 0)}

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

  // SPI1 (0 is the dedicated)
  BasePinToIOF(spi_pins(0).cs(0), iof_0(2))
  BasePinToIOF(spi_pins(0).dq(0), iof_0(3))
  BasePinToIOF(spi_pins(0).dq(1), iof_0(4))
  BasePinToIOF(spi_pins(0).sck,   iof_0(5))
  BasePinToIOF(spi_pins(0).dq(2), iof_0(6))
  BasePinToIOF(spi_pins(0).dq(3), iof_0(7))
  BasePinToIOF(spi_pins(0).cs(1), iof_0(8))
  BasePinToIOF(spi_pins(0).cs(2), iof_0(9))
  BasePinToIOF(spi_pins(0).cs(3), iof_0(10))

  // SPI2
  BasePinToIOF(spi_pins(1).cs(0), iof_0(26))
  BasePinToIOF(spi_pins(1).dq(0), iof_0(27))
  BasePinToIOF(spi_pins(1).dq(1), iof_0(28))
  BasePinToIOF(spi_pins(1).sck,   iof_0(29))
  BasePinToIOF(spi_pins(1).dq(2), iof_0(30))
  BasePinToIOF(spi_pins(1).dq(3), iof_0(31))

  // I2C
  if (p(PeripheryI2CKey).length == 1) {
    BasePinToIOF(i2c_pins(0).sda, iof_0(12))
    BasePinToIOF(i2c_pins(0).scl, iof_0(13))
  }

  // UART0
  BasePinToIOF(uart_pins(0).rxd, iof_0(16))
  BasePinToIOF(uart_pins(0).txd, iof_0(17))

  // UART1
  BasePinToIOF(uart_pins(1).rxd, iof_0(24))
  BasePinToIOF(uart_pins(1).txd, iof_0(25))

  //PWM
  BasePinToIOF(pwm_pins(0).pwm(0), iof_1(0) )
  BasePinToIOF(pwm_pins(0).pwm(1), iof_1(1) )
  BasePinToIOF(pwm_pins(0).pwm(2), iof_1(2) )
  BasePinToIOF(pwm_pins(0).pwm(3), iof_1(3) )

  BasePinToIOF(pwm_pins(1).pwm(1), iof_1(19))
  BasePinToIOF(pwm_pins(1).pwm(0), iof_1(20))
  BasePinToIOF(pwm_pins(1).pwm(2), iof_1(21))
  BasePinToIOF(pwm_pins(1).pwm(3), iof_1(22))

  BasePinToIOF(pwm_pins(2).pwm(0), iof_1(10))
  BasePinToIOF(pwm_pins(2).pwm(1), iof_1(11))
  BasePinToIOF(pwm_pins(2).pwm(2), iof_1(12))
  BasePinToIOF(pwm_pins(2).pwm(3), iof_1(13))

  //-----------------------------------------------------------------------
  // Drive actual Pads
  //-----------------------------------------------------------------------

  // Result of Pin Mux
  GPIOPinsFromPort(io.pins.gpio, sys.gpio(0))

  // Dedicated SPI Pads
  SPIPinsFromPort(io.pins.qspi, sys.qspi(0), clock = sys.clock, reset = sys.reset.toBool(), syncStages = 3)

  // JTAG Debug Interface
  val sjtag = sys.debug.systemjtag.get
  JTAGPinsFromPort(io.pins.jtag, sjtag.jtag)
  sjtag.reset := io.jtag_reset
  sjtag.mfr_id := p(JtagDTMKey).idcodeManufId.U(11.W)

  io.ndreset := sys.debug.ndreset

}
