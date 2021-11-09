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
