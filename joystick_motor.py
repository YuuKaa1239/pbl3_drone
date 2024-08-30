from machine import Pin, ADC, PWM
from time import sleep

X_AXIS_PIN = 33
ESC_PIN = 16

# Setup ADC for reading the joystick
joystick = ADC(Pin(X_AXIS_PIN))
joystick.atten(ADC.ATTN_11DB)  # Configure for the full range (0-3.3V)
joystick.width(ADC.WIDTH_12BIT)  # Set 12-bit resolution (values from 0 to 4095)

# Setup PWM for controlling the ESC
esc = PWM(Pin(ESC_PIN), freq=50)  # 50Hz for standard servo
esc.duty(40)  # Neutral position (duty cycle between 0 and 1023)

# Allow the ESC to initialize
sleep(2)

def map_value(x, in_min, in_max, out_min, out_max):
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

while True:
    joystick_value = joystick.read()
    joystick_value = min(max(joystick_value, 2200), 4095)  # Constrain between 2200 and 4095
    motor_speed = map_value(joystick_value, 2200, 4095, 40, 102)  # Map to PWM duty cycle range
    esc.duty(motor_speed)
    print(motor_speed)
    sleep(0.1)

