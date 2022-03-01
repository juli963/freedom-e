// See LICENSE for license details.
package sifive.freedom.Nexys4

import Chisel._

import freechips.rocketchip.config._
import freechips.rocketchip.subsystem._
import freechips.rocketchip.devices.debug._
import freechips.rocketchip.devices.tilelink._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.system._

import sifive.blocks.devices.mockaon._
import sifive.blocks.devices.gpio._
import sifive.blocks.devices.pwm._
import sifive.blocks.devices.spi._
import sifive.blocks.devices.uart._
import sifive.blocks.devices.i2c._

import hni.blocks.wdt._

import juli.blocks.devices.LiteDRAM_Ctrl._
import juli.blocks.DRP_Mem._
//-------------------------------------------------------------------------
// Nexys4System
//-------------------------------------------------------------------------

class Nexys4System(implicit p: Parameters) extends RocketSubsystem
    with HasPeripheryMaskROMSlave
    with HasPeripheryDebug
   // with HasPeripheryMockAON
    with HasPeripheryUART
    with HasPeripherySPIFlash
    with HasPeripherySPI
    with HasPeripheryGPIO
    with HasPeripheryPWM
    with CanHavePeripheryDRP_MemList
    with CanHavePeripheryLiteDRAM_CtrlList
    with HasPeripheryI2C{
  override lazy val module = new Nexys4SystemModule(this)
}

class Nexys4SystemModule[+L <: Nexys4System](_outer: L)
  extends RocketSubsystemModuleImp(_outer)
    with HasPeripheryDebugModuleImp
    with HasPeripheryUARTModuleImp
    with HasPeripherySPIModuleImp
    with HasPeripheryGPIOModuleImp
    with HasPeripherySPIFlashModuleImp
    //with HasPeripheryMockAONModuleImp
    with HasPeripheryPWMModuleImp
    with CanHavePeripheryDRP_MemListModuleImp
    with CanHavePeripheryLiteDRAM_CtrlListModuleImp
    with HasPeripheryI2CModuleImp{
  // Reset vector is set to the location of the mask rom
  val maskROMParams = p(PeripheryMaskROMKey)
  global_reset_vector := maskROMParams(0).address.U
}
