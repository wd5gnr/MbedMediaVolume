#ifndef __ANALOGINOVERSAMPLE_H
#define __ANALOGINOVERSAMPLE_H

// Simple class to read 16-bit counts from ADC and average N samples
// Up to you not to overflow 32-bits!

class AnalogInOversample : public AnalogIn
{
    protected:
    uint8_t N;  // # of samples
    public:
    // constructor assumes 16 samples, or set your own
    AnalogInOversample(PinName pin, uint8_t n=16,float vref=MBED_CONF_TARGET_DEFAULT_ADC_VREF)
     : AnalogIn(pin,vref) { N=n; } 
     // access N
    uint8_t get_N(void) { return N; }
    void set_N(uint8_t n) { N=n; }
    // Here's the meat of it
    unsigned short read_u16(void)
    {
        uint32_t samples=0;  // 32 bits for 16-bit samples
        for (int i=0;i<N;i++) samples+=AnalogIn::read_u16();
        return samples/N;
    }
};

#endif