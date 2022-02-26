// See LICENSE for license details.
package juli.HSSnifferv1.base

import freechips.rocketchip.config._
import freechips.rocketchip.subsystem._
import freechips.rocketchip.devices.debug._
import freechips.rocketchip.devices.tilelink._
import freechips.rocketchip.diplomacy.{DTSModel, DTSTimebase}
import freechips.rocketchip.system._
import freechips.rocketchip.tile._

import sifive.blocks.devices.mockaon._
import sifive.blocks.devices.gpio._
import sifive.blocks.devices.spi._
import sifive.blocks.devices.uart._
import sifive.blocks.devices.i2c._

import juli.blocks.devices.ESP_FIFO._
import juli.blocks.devices.LiteDRAM_Ctrl._
import juli.blocks.HSControl._

// Default HSSnifferv1 v1 Config
class DefaultHSSnifferv1Config extends Config (
  new WithNBreakpoints(2)        ++
  new WithNExtTopInterrupts(0)   ++
  new WithJtagDTM                ++
  new TinyConfig
)

// HSSnifferv1 v1 Dev Kit Peripherals
class HSSnifferv1Peripherals extends Config((site, here, up) => {
  case PeripheryGPIOKey => List(
    GPIOParams(address = 0x10012000, width = 25, includeIOF = true))
  case PeripherySPIFlashKey => List(
    SPIFlashParams(
      fAddress = 0x20000000,
      rAddress = 0x10014000,
      defaultSampleDel = 3))
  case PeripheryUARTKey => List(
    UARTParams(address = 0x10013000))
  case PeripheryI2CKey => List(
    I2CParams(address = 0x10016000),
    I2CParams(address = 0x10017000),
    I2CParams(address = 0x10018000),
    I2CParams(address = 0x10019000))
  case PeripheryMaskROMKey => List(
    MaskROMParams(address = 0x10000, name = "BootROM"))
  case ESP_FIFOListKey => Some(List(
    ESP_FIFOParams(reg_slave_0_address = 0x10020000)
    ))
  case HSControlListKey => Some(List(
    HSControlParams(reg_slave_0_address = 0x10080000)
    ))
  case LiteDRAM_CtrlListKey => Some(List(
    LiteDRAM_CtrlParams(mem_slave_0_address = 0x10040000, mem_slave_0_sizeBytes = 0x0000FFFF )
    ))
})

// HSSnifferv1 Peripherals
class HSSnifferv1Config extends Config(
  new HSSnifferv1Peripherals    ++
  new DefaultHSSnifferv1Config().alter((site,here,up) => {
    case DTSTimebase => BigInt(32768)
    case JtagDTMKey => new JtagDTMConfig (
      idcodeVersion = 2,
      idcodePartNum = 0x000,
      idcodeManufId = 0x489,
      debugIdleCycles = 5)
  })
)
