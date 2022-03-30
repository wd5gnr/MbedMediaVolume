// Pot Volume control for PC -- Williams

#include "mbed.h"
#include "USBKeyboard.h"
#include "AnalogInOversample.h"

// We will use one thread to read the pot and another to send keyboard commands
Thread readknobThread;
Thread keyboardThread;



USBKeyboard kbd;  // interface to keyboard
DigitalIn keyin(USER_BUTTON, PullUp);  // button on Blackpill
AnalogInOversample adc(A1);   // read pot

// This is the mail message sent between threads
#define VUP 1
#define VDN 0
#define VMUTE 2

typedef struct {
    int voldir;  // 0= down 1=up, 2=mute toggle
} post_t;

// And our shared mailbox
Mail<post_t,32> mail_box;

// Since we use a pot and not an encoder, we have to determine how much difference in rotation
// is an event. This is in counts so 6554 is about 10%
#define THRESHOLD 200

// Helper function to send mail messages from readknobthread to keyboardthread
void send_kbd(int event)
{
    post_t *mail=mail_box.try_alloc();
    if (!mail) return; // oh oh...
    mail->voldir=event;
    mail_box.put(mail);
}

// This thread will read the pot and issue commands to the keyboard thread
// The button on the blackpill will do two things. One it will toggle the mute. 
// Second, it will reset the idea of the baseline for the pot
// So if you are all the way CCW, for example, and you want to turn the volume down
// you are stuck. So mute, adjust the volume to about the center, and unmute.
// Now you can go up and down from that point. Sure -- should use an encoder, of course.
void vol_thread()
{

    int key;  // State of the button (0=pressed)
    // The pot value is in v and the last reading is in last.
    // The delta is what we compare against THRESHOLD
    // The keystate keeps track of the button push so we don't send mute over and over
    uint32_t v, last, delta, keystate=0;
    last=adc.read_u16();   // get baseline

    while (1)
    {
    key=keyin;   // read button
    if (keystate==1 && key==1)   // if button was down and now is up, complete cycle
    {
        keystate=0;
    }
    if (key==0 && keystate==0)   // if button was up and is now down, start a mute cycle
    {
        keystate=1;   // remember
        send_kbd(VMUTE);
        last=adc.read_u16();  // rebaseline the volume control
    }
    v=adc.read_u16();   // read pot
    delta=v>last?v-last:last-v;  // find difference
    if (delta>THRESHOLD)
    {
        send_kbd(v>last?VUP:VDN);
    }     
    last=v;  // remember position
    ThisThread::sleep_for(50ms);  // sleepy -- no need to slam the ADC and also debounce the switch
    }
}

// This thread pulls from the mailbox and sends keys to the PC
void pc_thread()
{
    while (1)
    {
        post_t *mail;
        mail=mail_box.try_get();   // get mail
        if (mail)
        {
            switch (mail->voldir)  // work out commands
            {
                case VDN:
                kbd.media_control(KEY_VOLUME_DOWN);
                break;

                case VUP:
                kbd.media_control(KEY_VOLUME_UP);
                break;

                case VMUTE:
                kbd.media_control(KEY_MUTE);
                break;
            }
            mail_box.free(mail);  // Note: we free mailbox message
        }
        else
            ThisThread::sleep_for(50ms);

    }
}




// Main thread
int main()
{
    readknobThread.start(vol_thread);  // launch threads
    keyboardThread.start(pc_thread);
    readknobThread.join();

}
