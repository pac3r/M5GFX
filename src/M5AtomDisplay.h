#ifndef __M5GFX_M5ATOMDISPLAY__
#define __M5GFX_M5ATOMDISPLAY__

// If you want to use a set of functions to handle SD/SPIFFS/HTTP,
//  please include <SD.h>,<SPIFFS.h>,<HTTPClient.h> before <M5GFX.h>
// #include <SD.h>
// #include <SPIFFS.h>
// #include <HTTPClient.h>

#include <sdkconfig.h>
#include <esp_efuse.h>
#include <soc/efuse_reg.h>

#include "M5GFX.h"
#include "lgfx/v1/panel/Panel_M5HDMI.hpp"

#ifndef M5ATOMDISPLAY_LOGICAL_WIDTH
#define M5ATOMDISPLAY_LOGICAL_WIDTH 1280
#endif
#ifndef M5ATOMDISPLAY_LOGICAL_HEIGHT
#define M5ATOMDISPLAY_LOGICAL_HEIGHT 720
#endif
#ifndef M5ATOMDISPLAY_REFRESH_RATE
#define M5ATOMDISPLAY_REFRESH_RATE 0.0f
#endif
#ifndef M5ATOMDISPLAY_OUTPUT_WIDTH
#define M5ATOMDISPLAY_OUTPUT_WIDTH 0
#endif
#ifndef M5ATOMDISPLAY_OUTPUT_HEIGHT
#define M5ATOMDISPLAY_OUTPUT_HEIGHT 0
#endif
#ifndef M5ATOMDISPLAY_SCALE_W
#define M5ATOMDISPLAY_SCALE_W 0
#endif
#ifndef M5ATOMDISPLAY_SCALE_H
#define M5ATOMDISPLAY_SCALE_H 0
#endif

class M5AtomDisplay : public lgfx::LGFX_Device
{
  lgfx::Panel_M5HDMI _panel_instance;
  lgfx::Bus_SPI      _bus_instance;

public:

  m5gfx::board_t getBoard(void) const { return m5gfx::board_t::board_M5AtomDisplay; }

  M5AtomDisplay( uint16_t logical_width  = M5ATOMDISPLAY_LOGICAL_WIDTH
               , uint16_t logical_height = M5ATOMDISPLAY_LOGICAL_HEIGHT
               , float refresh_rate      = M5ATOMDISPLAY_REFRESH_RATE
               , uint16_t output_width   = M5ATOMDISPLAY_OUTPUT_WIDTH
               , uint16_t output_height  = M5ATOMDISPLAY_OUTPUT_HEIGHT
               , uint_fast8_t scale_w    = M5ATOMDISPLAY_SCALE_W
               , uint_fast8_t scale_h    = M5ATOMDISPLAY_SCALE_H
               )
  {
    static constexpr int i2c_port =  1;
    static constexpr int i2c_sda  = 25;
    static constexpr int i2c_scl  = 21;
    static constexpr int spi_cs   = 33;
    static constexpr int spi_mosi = 19;
    static constexpr int spi_miso = 22;

#if defined ( EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4 )
    int spi_sclk = (esp_efuse_get_pkg_ver() == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4)
                 ? 23  // for ATOM Lite / Matrix
                 : 5   // for ATOM PSRAM
                 ;
#else
    int spi_sclk = 5;
#endif

    {
      auto cfg = _bus_instance.config();
      cfg.freq_write = 80000000;
      cfg.freq_read  = 20000000;
      cfg.spi_host = VSPI_HOST;
      cfg.spi_mode = 3;
      cfg.dma_channel = 1;
      cfg.use_lock = true;
      cfg.pin_mosi = spi_mosi;
      cfg.pin_miso = spi_miso;
      cfg.pin_sclk = spi_sclk;
      cfg.spi_3wire = false;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config_transmitter();
      cfg.freq_read = 400000;
      cfg.freq_write = 400000;
      cfg.pin_scl = i2c_scl;
      cfg.pin_sda = i2c_sda;
      cfg.i2c_port = i2c_port;
      cfg.i2c_addr = 0x39;
      cfg.prefix_cmd = 0x00;
      cfg.prefix_data = 0x00;
      cfg.prefix_len = 0;
      _panel_instance.config_transmitter(cfg);
    }

    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs     = spi_cs;
      cfg.readable   = false;
      cfg.bus_shared = false;
      _panel_instance.config(cfg);

      m5gfx::Panel_M5HDMI::config_resolution_t cfg_reso;
      cfg_reso.logical_width  = logical_width;
      cfg_reso.logical_height = logical_height;
      cfg_reso.refresh_rate   = refresh_rate;
      cfg_reso.output_width   = output_width;
      cfg_reso.output_height  = output_height;
      cfg_reso.scale_w        = scale_w;
      cfg_reso.scale_h        = scale_h;
      _panel_instance.config_resolution(cfg_reso);
    }
    setPanel(&_panel_instance);
  }

  bool setResolution( uint16_t logical_width  = M5ATOMDISPLAY_LOGICAL_WIDTH
                    , uint16_t logical_height = M5ATOMDISPLAY_LOGICAL_HEIGHT
                    , float refresh_rate      = M5ATOMDISPLAY_REFRESH_RATE
                    , uint16_t output_width   = M5ATOMDISPLAY_OUTPUT_WIDTH
                    , uint16_t output_height  = M5ATOMDISPLAY_OUTPUT_HEIGHT
                    , uint_fast8_t scale_w    = M5ATOMDISPLAY_SCALE_W
                    , uint_fast8_t scale_h    = M5ATOMDISPLAY_SCALE_H
                    )
  {
    return _panel_instance.setResolution
      ( logical_width
      , logical_height
      , refresh_rate
      , output_width
      , output_height
      , scale_w
      , scale_h
      );
  }
};

#endif
