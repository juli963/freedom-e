// See LICENSE for license details.
package juli.GBEv1.base

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

import juli.blocks.devices.tdpmem._
import juli.blocks.devices.ethctrl._

// Default GBE v1 Config
class DefaultGBEv1Config extends Config (
  new WithNBreakpoints(2)        ++
  new WithNExtTopInterrupts(0)   ++
  new WithJtagDTM                ++
  new TinyConfig
)

// GBE v1 Dev Kit Peripherals
class GBEv1Peripherals extends Config((site, here, up) => {
  case PeripheryGPIOKey => List(
    GPIOParams(address = 0x10012000, width = 9, includeIOF = true))
  case PeripheryPWMKey => List(
    PWMParams(address = 0x10015000, cmpWidth = 16, ncmp = 3),
    PWMParams(address = 0x10025000, cmpWidth = 16, ncmp = 3))
  case PeripherySPIFlashKey => List(
    SPIFlashParams(
      fAddress = 0x20000000,
      rAddress = 0x10014000,
      defaultSampleDel = 3))
  case PeripheryUARTKey => List(
    UARTParams(address = 0x10013000))
  case PeripheryI2CKey => List(
    I2CParams(address = 0x10016000))
  case PeripheryMaskROMKey => List(
    MaskROMParams(address = 0x10000, name = "BootROM"))
  case TDPMemListKey => Some(
                    List(
                      TDPMemParams(address = 0x8000, sizeBytes = 40)
                    ))
})

// GBEv1 Peripherals
class GBEv1Config extends Config(
  new GBEv1Peripherals    ++
  new DefaultGBEv1Config().alter((site,here,up) => {
    case DTSTimebase => BigInt(32768)
    case JtagDTMKey => new JtagDTMConfig (
      idcodeVersion = 2,
      idcodePartNum = 0x000,
      idcodeManufId = 0x489,
      debugIdleCycles = 5)
  })
)
