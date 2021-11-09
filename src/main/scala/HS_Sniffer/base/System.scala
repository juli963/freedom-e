// See LICENSE for license details.
package juli.HSSnifferv1.base

import Chisel._

import freechips.rocketchip.config._
import freechips.rocketchip.subsystem._
import freechips.rocketchip.devices.debug._
import freechips.rocketchip.devices.tilelink._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.system._

import sifive.blocks.devices.gpio._
import sifive.blocks.devices.spi._
import sifive.blocks.devices.uart._
import sifive.blocks.devices.i2c._

//-------------------------------------------------------------------------
// E300ArtyDevKitSystem
//-------------------------------------------------------------------------

class HSSnifferv1System(implicit p: Parameters) extends RocketSubsystem
    with HasPeripheryMaskROMSlave
    with HasPeripheryDebug
    with HasPeripheryUART
    with HasPeripherySPIFlash
    with HasPeripheryGPIO
    with HasPeripheryI2C
   {
  override lazy val module = new HSSnifferv1SystemModule(this)
}

class HSSnifferv1SystemModule[+L <: HSSnifferv1System](_outer: L)
  extends RocketSubsystemModuleImp(_outer)
    with HasPeripheryDebugModuleImp
    with HasPeripheryUARTModuleImp
    with HasPeripheryGPIOModuleImp
    with HasPeripherySPIFlashModuleImp
    with HasPeripheryI2CModuleImp
   {
  // Reset vector is set to the location of the mask rom
  val maskROMParams = p(PeripheryMaskROMKey)
  global_reset_vector := maskROMParams(0).address.U
}
