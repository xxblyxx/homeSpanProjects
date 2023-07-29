/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2022 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/
 
// HomeSpan Addressable RGB LED Examples.  Demonstrates use of:
//
//  * HomeSpan Pixel Class that provides for control of single-wire addressable RGB and RGBW LEDs, such as the WS2812 and SK6812
//  * HomeSpan Dot Class that provides for control of two-wire addressable RGB LEDs, such as the APA102 and SK9822
//
// IMPORTANT:  YOU LIKELY WILL NEED TO CHANGE THE PIN NUMBERS BELOW TO MATCH YOUR SPECIFIC ESP32/S2/C3 BOARD
//

#if defined(CONFIG_IDF_TARGET_ESP32)

  #define NEOPIXEL_RGBW_PIN      26

#elif defined(CONFIG_IDF_TARGET_ESP32S2)

  #define NEOPIXEL_RGBW_PIN      26

#elif defined(CONFIG_IDF_TARGET_ESP32C3)

  #define NEOPIXEL_RGBW_PIN      26

  #define DEVICE_SUFFIX          "-C3"

#endif
 
#include "HomeSpan.h"
#include "extras/Pixel.h"                       // include the HomeSpan Pixel class


///////////////////////////////

struct NeoPixel_RGBW : Service::LightBulb {      // Addressable single-wire RGBW LED Strand (e.g. NeoPixel)
 
  Characteristic::On power{0,true};
  Characteristic::Hue H{0,true};
  Characteristic::Saturation S{100,true};
  Characteristic::Brightness V{100,true};
  //Characteristic::ColorTemperature T{140,true};
  Pixel *pixel;
  uint8_t nPixels;
  
  NeoPixel_RGBW(uint8_t pin, uint8_t nPixels) : Service::LightBulb(){

    V.setRange(5,100,1);                      // sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%
    pixel=new Pixel(pin,true);                // creates Pixel RGBW LED (second parameter set to true for RGBW) on specified pin
    this->nPixels=nPixels;                    // save number of Pixels in this LED Strand
    update();                                 // manually call update() to set pixel with restored initial values
  }

  boolean update() override {

    int p=power.getNewVal();
    
    //float v=V.getNewVal<float>();       // range = [0,100]
    //float t=T.getNewVal<float>();       // range = [140,500] (140=coldest, 500=warmest)
    //float hue=240-(t-140)/3;            // add in a splash of color between blue and green to simulated change of color temperature
    // Pixel::Color color;              // if static HSV method is used (below), there is no need to first create a Color object
    //pixel->set(pixel->HSV(hue, 100, v*p, v*p),nPixels);       // sets all nPixels to the same HSV color (note use of static method pixel->HSV, instead of defining and setting Pixel::Color)
    
    pixel->set(pixel->HSV(H.getNewVal<float>(),S.getNewVal<float>(),V.getNewVal<float>()*p),nPixels); //multiply V(brightness) against power, so if power = 0, no brightness
     
    return(true);  
  }
};



///////////////////////////////

void setup() {
  
  Serial.begin(115200);
 
  homeSpan.begin(Category::Lighting,"Vinyl LED");    
  homeSpan.setPairingCode("11112222");                 
      
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Name("Vinyl LED");
      new Characteristic::Manufacturer("HomeSpan");
      new Characteristic::SerialNumber("123-BLY");
      new Characteristic::Model("NeoPixel 15 RGBW LEDs");
      new Characteristic::FirmwareRevision("1.0");
      new Characteristic::Identify();

  new Service::HAPProtocolInformation();
    new Characteristic::Version("1.1.0");

  new NeoPixel_RGBW(NEOPIXEL_RGBW_PIN,15);

}

///////////////////////////////

void loop() {
  homeSpan.poll();
}

///////////////////////////////
