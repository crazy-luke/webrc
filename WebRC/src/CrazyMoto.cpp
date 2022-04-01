#include <Servo.h>
#include <Adafruit_NeoPixel.h>

class CrazyMoto
{
private:
    Servo handServo, wheelServo;
    Adafruit_NeoPixel strip;
    bool islight = false;
    int hornPin;

public:
    enum Act
    {
        Forward,
        Standby,
        Backward,
        Horn,
        LedSwitch,
        LedRGB,
    };

    CrazyMoto(uint8_t handlePin, uint8_t wheelPin, uint8_t ledPin, uint8_t hornPin)
    {
        strip.setPin(ledPin);
        strip.updateLength(1);
        strip.updateType(NEO_GRB + NEO_KHZ800);
        strip.begin();

        pinMode(hornPin, OUTPUT);
        digitalWrite(hornPin, LOW);
        this->hornPin = hornPin;

        handServo.attach(handlePin);
        handServo.write(90);
        wheelServo.attach(wheelPin);
        wheelServo.write(90);
    }

    void doAct(Act act)
    {
        switch (act)
        {
        case Act::Forward:
            wheelServo.write(0);
            break;

        case Act::Standby:
            wheelServo.write(90);
            break;

        case Act::Backward:
            wheelServo.write(180);
            break;

        case Act::Horn:
            tone(hornPin, 400, 300);
            break;

        case Act::LedSwitch:
            if (!islight)
                strip.setPixelColor(0, strip.Color(secureRandom(0, 255), secureRandom(0, 255), secureRandom(0, 255)));
            else
                strip.setPixelColor(0, strip.Color(0, 0, 0));
            strip.show();
            islight = !islight;
            break;

        case Act::LedRGB:
            strip.setPixelColor(0, strip.Color(255, 0, 0));
            strip.show();
            delay(500);
            strip.setPixelColor(0, strip.Color(0, 255, 0));
            strip.show();
            strip.setPixelColor(0, strip.Color(0, 0, 255));
            strip.show();
            delay(500);
            strip.setPixelColor(0, strip.Color(0, 0, 0));
            strip.show();
            break;

        default:
            break;
        }
    }

    void doHandle(int value)
    {
        handServo.write(value);
    }

    void doWheel(int value)
    {
        wheelServo.write(value);
    }
};
