// See LICENSE for license details.
package sifive.freedom.Nexys4

import freechips.rocketchip.config._
import freechips.rocketchip.subsystem._
import freechips.rocketchip.devices.debug._
import freechips.rocketchip.devices.tilelink._
import freechips.rocketchip.diplomacy.{DTSModel, DTSTimebase}
import freechips.rocketchip.system._
import freechips.rocketchip.tile._

import sifive.blocks.devices.mockaon._
import sifive.blocks.devices.gpio._
import sifive.blocks.devices.pwm._
import sifive.blocks.devices.spi._
import sifive.blocks.devices.uart._
import sifive.blocks.devices.i2c._

import hni.blocks.wdt._
import hni.blocks.devices.watchdog._

import juli.blocks.devices.LiteDRAM_Ctrl._
import juli.blocks.DRP_Mem._

// Default FreedomEConfig
class DefaultFreedomEConfig extends Config (
  new WithNBreakpoints(2)        ++
  new WithNExtTopInterrupts(0)   ++
  new WithJtagDTM                ++
  new TinyConfig
)

// Freedom E300 Arty Dev Kit Peripherals
class Nexys4Peripherals extends Config((site, here, up) => {
  case PeripheryGPIOKey => List(
    GPIOParams(address = 0x10012000, width = 32, includeIOF = true),
    GPIOParams(address = 0x10022000, width = 32, includeIOF = false))
  case PeripheryPWMKey => List(
    PWMParams(address = 0x10015000, cmpWidth = 8),
    PWMParams(address = 0x10025000, cmpWidth = 16),
    PWMParams(address = 0x10035000, cmpWidth = 16))
  case PeripherySPIKey => List(
    SPIParams(csWidth = 4, rAddress = 0x10024000, defaultSampleDel = 3),
    SPIParams(csWidth = 1, rAddress = 0x10034000, defaultSampleDel = 3))
  case PeripherySPIFlashKey => List(
    SPIFlashParams(
      fAddress = 0x20000000,
      rAddress = 0x10014000,
      defaultSampleDel = 3))
  case PeripheryUARTKey => List(
    UARTParams(address = 0x10013000),
    UARTParams(address = 0x10023000))
  case PeripheryI2CKey => List(
    I2CParams(address = 0x10016000))
  case PeripheryMockAONKey =>
    MockAONParams(address = 0x10000000)
  case PeripheryMaskROMKey => List(
    MaskROMParams(address = 0x10000, name = "BootROM"))
  case DRP_MemListKey => Some(List(
    DRP_MemParams(mem_slave_0_address = 0x100A0000, drp_width = 5)
    ))
  case LiteDRAM_CtrlListKey => Some(List(
    LiteDRAM_CtrlParams(mem_slave_0_address = 0x10040000, mem_slave_0_sizeBytes = 0x0000FFFF )
    ))

})

// Freedom E300 Arty Dev Kit Peripherals
class Nexys4Config extends Config(
  new Nexys4Peripherals    ++
  new DefaultFreedomEConfig().alter((site,here,up) => {
    case DTSTimebase => BigInt(32768)
    case JtagDTMKey => new JtagDTMConfig (
      idcodeVersion = 2,
      idcodePartNum = 0x000,
      idcodeManufId = 0x489,
      debugIdleCycles = 5)
  })
)
