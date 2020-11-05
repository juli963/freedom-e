// See LICENSE for license details.
package juli.GBEv1.base

import Chisel._

import freechips.rocketchip.config._
import freechips.rocketchip.subsystem._
import freechips.rocketchip.devices.debug._
import freechips.rocketchip.devices.tilelink._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.system._

import sifive.blocks.devices.gpio._
import sifive.blocks.devices.pwm._
import sifive.blocks.devices.spi._
import sifive.blocks.devices.uart._
import sifive.blocks.devices.i2c._

import juli.blocks.devices.tdpmem._
import juli.blocks.devices.ethctrl._

//-------------------------------------------------------------------------
// E300ArtyDevKitSystem
//-------------------------------------------------------------------------

class GBEv1System(implicit p: Parameters) extends RocketSubsystem
    with HasPeripheryMaskROMSlave
    with HasPeripheryDebug
    with HasPeripheryUART
    with HasPeripherySPIFlash
    with HasPeripheryGPIO
    with HasPeripheryPWM
    with HasPeripheryI2C
    with CanHavePeripheryTDPMemList
    with CanHavePeripheryETHCtrlList
   {
  override lazy val module = new GBEv1SystemModule(this)
}

class GBEv1SystemModule[+L <: GBEv1System](_outer: L)
  extends RocketSubsystemModuleImp(_outer)
    with HasPeripheryDebugModuleImp
    with HasPeripheryUARTModuleImp
    with HasPeripheryGPIOModuleImp
    with HasPeripherySPIFlashModuleImp
    with HasPeripheryPWMModuleImp
    with HasPeripheryI2CModuleImp
    with CanHavePeripheryTDPMemListModuleImp 
    with CanHavePeripheryETHCtrlListModuleImp 
   {
  // Reset vector is set to the location of the mask rom
  val maskROMParams = p(PeripheryMaskROMKey)
  global_reset_vector := maskROMParams(0).address.U
}
