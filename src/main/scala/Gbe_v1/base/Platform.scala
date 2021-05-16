// See LICENSE for license details.
package juli.GBEv1.base

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
import sifive.blocks.devices.pwm._
import sifive.blocks.devices.spi._
import sifive.blocks.devices.uart._
import sifive.blocks.devices.i2c._
import sifive.blocks.devices.pinctrl._

import hni.blocks.wdt._

import Ethernet.Interface.Types._
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

class GBEv1PlatformIO(implicit val p: Parameters) extends Bundle {
  val pins = new Bundle {
    val jtag = new JTAGPins(() => PinGen(), false)
    val gpio = new GPIOPins(() => PinGen(), p(PeripheryGPIOKey)(0))
    val qspi = new SPIPins(() => PinGen(), p(PeripherySPIFlashKey)(0))
  }
  val jtag_reset = Bool(INPUT)
  val ndreset    = Bool(OUTPUT)

  val RGMII = new Ethernet.Interface.Types.RGMII_Interface()
  val PHY_nrst = Bool(OUTPUT)
  val rx_error = Bool(OUTPUT)
  val EthernetClock125 = Clock(INPUT)
  val EthernetClock250 = Clock(INPUT)
}

//-------------------------------------------------------------------------
// E300ArtyDevKitPlatform
//-------------------------------------------------------------------------

class GBEv1Platform(implicit val p: Parameters) extends Module {
  val sys = Module(LazyModule(new GBEv1System).module)
  val io = new GBEv1PlatformIO

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
  val sys_pwm  = sys.pwm
  val sys_i2c  = sys.i2c

  val uart_pins = p(PeripheryUARTKey).map { c => Wire(new UARTPins(() => PinGen()))}
  val pwm_pins  = p(PeripheryPWMKey).map  { c => Wire(new PWMPins(() => PinGen(), c))}
  //val spi_pins  = p(PeripherySPIKey).map  { c => Wire(new SPIPins(() => PinGen(), c))}
  val i2c_pins  = p(PeripheryI2CKey).map  { c => Wire(new I2CPins(() => PinGen()))}

  (uart_pins zip  sys_uart) map {case (p, r) => UARTPinsFromPort(p, r, clock = clock, reset = reset, syncStages = 0)}
  (pwm_pins  zip  sys_pwm)  map {case (p, r) => PWMPinsFromPort(p, r) }
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


  // I2C
  if (p(PeripheryI2CKey).length == 1) {
    BasePinToIOF(i2c_pins(0).sda, iof_0(0))
    BasePinToIOF(i2c_pins(0).scl, iof_0(1))
  }

  // UART0
  //BasePinToIOF(uart_pins(0).rxd, iof_0(16))
  BasePinToIOF(uart_pins(0).txd, iof_0(2))

  //PWM
  BasePinToIOF(pwm_pins(0).pwm(0), iof_0(3) )
  BasePinToIOF(pwm_pins(0).pwm(1), iof_0(4) )
  BasePinToIOF(pwm_pins(0).pwm(2), iof_0(5) )

  BasePinToIOF(pwm_pins(1).pwm(0), iof_0(6))
  BasePinToIOF(pwm_pins(1).pwm(1), iof_0(7))
  BasePinToIOF(pwm_pins(1).pwm(2), iof_0(8))


  io.RGMII <> sys.ethctrl_io.get(0).RGMII
  io.PHY_nrst := sys.ethctrl_io.get(0).PHY_nrst
  io.rx_error := sys.ethctrl_io.get(0).rx_error
  sys.ethctrl_io.get(0).EthernetClock125 := io.EthernetClock125
  sys.ethctrl_io.get(0).EthernetClock250 := io.EthernetClock250

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
