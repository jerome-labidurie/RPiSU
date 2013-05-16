#include <avr/io.h>
#ifndef F_CPU
#warning "F_CPU war noch nicht definiert, wird nun mit 1000000 definiert"
#define F_CPU 1000000UL
#endif
#include <util/delay.h>

void sleep(uint16_t ms)
{
    for(; ms > 0; ms--) _delay_ms(1);
}

inline uint8_t debounce(volatile uint8_t *port, uint8_t pin)
{
    if(!(*port & (1<<pin))) // Check low level for button pressed
    {
        sleep(100); // Give user some time to release the button
        if(*port & (1<<pin)) // If button released there is a high level
        {
            sleep(250); // Wait a while for new button check
            return 1;
        }
    }
    return 0;
}

void init(void)
{
    DDRB &= ~((1<<DDB0) | (1<<DDB1) | (1<<DDB2)); // Set pin 1 and 2 to input (1: Power, 2: Reset, 0: Raspberry Pi)
    PORTB |= (1<<PB1); // enable Pullup
    PORTB |= (1<<PB2); // enable Pullup
    DDRB |= (1<<DDB3); // Set pin 3 to output (3: Relais)
}

void powerOn(void)
{
    PORTB |= (1<<PB3); // Set Relais ON
}

void powerOff(void)
{
    PORTB &= ~(1<<PB3); // Set Relais OFF
}

int waitForOn(void)
{
    while(1)
    { // Wait for button pressed
        if(debounce(&PINB, PB1)) return 1;
    }
    return 0;
}

int waitForOffRst(void)
{
    while(1)
    { // Wait for button pressed
        if(debounce(&PINB, PB1)) return 1; // On/Off button pressed
        if(debounce(&PINB, PB2)) { // Reset button pressed
            powerOff();
            return 0;
        }
    }
    return 0;
}

void waitForRPiOn(void)
{
    while(!(PINB & (1<<PINB0)))
    { // Wait for Raspberry Pi
        ;
    }
}

void waitForRPiOff(void)
{
    while(PINB & (1<<PINB0))
    { // Wait for Raspberry Pi
        ;
    }
}

void sendShutdown()
{
    DDRB |= (1<<DDB0); // Set pin 0 to output (0: Raspberry Pi)
    PORTB |= (1<<PB0); // HIGH
    sleep(1000); // Give RPi a 1 second HIGH pulse
    PORTB &= ~(1<<PB0); // LOW
    DDRB &= ~(1<<DDB0); // Set pin 0 to input (0: Raspberry Pi)
    sleep(2000); // Wait for RPi
}

int main(void)
{
    while(1)
    {
        init(); // Init Ports
        waitForOn(); // Wait until user pressed On/Off button
        powerOn(); // RPi power on
        waitForRPiOn(); // Wait for RPi ready signal
        if(!waitForOffRst()) continue; // If button On/Off pressed, shutdown. If button Reset pressed, reset.
        // This code only execute if user pressed On/Off button.
        sendShutdown(); // Send shutdown signal
        waitForRPiOff(); // Wait until RPi shutting down
        sleep(5000); // Wait for safe shutdown
        powerOff(); // RPi power off
    }
    return 0;
}
