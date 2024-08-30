import smbus
import time
import math

# MPU9250 I2C address
MPU9250_ADDR = 0x68

# MPU9250 Register Map
WHO_AM_I_REG = 0x75
ACCEL_XOUT_H = 0x3B
GYRO_XOUT_H = 0x43
MAG_XOUT_L = 0x03
MAG_ADDR = 0x0C

# MPU9250 Configuration
bus = smbus.SMBus(1)  # Use I2C bus 1

# Helper function to read sensor data
def read_i2c_word(address, reg):
    high = bus.read_byte_data(address, reg)
    low = bus.read_byte_data(address, reg + 1)
    value = (high << 8) + low
    if value >= 0x8000:  # Convert to signed
        return -((65535 - value) + 1)
    else:
        return value

# Initialize MPU9250
def setup_mpu9250():
    # Wake up the MPU9250
    bus.write_byte_data(MPU9250_ADDR, 0x6B, 0x00)

    # Configure accelerometer and gyroscope
    bus.write_byte_data(MPU9250_ADDR, 0x1C, 0x00)  # +/- 2g (accelerometer)
    bus.write_byte_data(MPU9250_ADDR, 0x1B, 0x00)  # +/- 250 deg/s (gyroscope)

def read_accel_gyro():
    # Read accelerometer data
    aX = read_i2c_word(MPU9250_ADDR, ACCEL_XOUT_H)
    aY = read_i2c_word(MPU9250_ADDR, ACCEL_XOUT_H + 2)
    aZ = read_i2c_word(MPU9250_ADDR, ACCEL_XOUT_H + 4)
    aSqrt = math.sqrt(aX**2 + aY**2 + aZ**2)
    
    # Read gyroscope data
    gX = read_i2c_word(MPU9250_ADDR, GYRO_XOUT_H)
    gY = read_i2c_word(MPU9250_ADDR, GYRO_XOUT_H + 2)
    gZ = read_i2c_word(MPU9250_ADDR, GYRO_XOUT_H + 4)
    
    print(f"accelX: {aX} | accelY: {aY} | accelZ: {aZ} | accelSqrt: {aSqrt}")
    print(f"gyroX: {gX} | gyroY: {gY} | gyroZ: {gZ}")
    
    return aX, aY, aZ, aSqrt, gX, gY, gZ

def read_mag():
    # Enable the magnetometer and read data
    bus.write_byte_data(MPU9250_ADDR, 0x37, 0x02)  # Enable bypass mode for I2C
    bus.write_byte_data(MAG_ADDR, 0x0A, 0x01)      # Enable magnetometer
    
    mX = read_i2c_word(MAG_ADDR, MAG_XOUT_L)
    mY = read_i2c_word(MAG_ADDR, MAG_XOUT_L + 2)
    mZ = read_i2c_word(MAG_ADDR, MAG_XOUT_L + 4)
    mDirection = math.atan2(mY, mX) * (180 / math.pi)  # Horizontal direction in degrees
    
    print(f"magX: {mX} | magY: {mY} | magZ: {mZ} | horizontal direction: {mDirection}")
    
    return mX, mY, mZ, mDirection

def main():
    setup_mpu9250()
    time.sleep(1)
    
    while True:
        sensorId = bus.read_byte_data(MPU9250_ADDR, WHO_AM_I_REG)
        if sensorId == 0x71:  # MPU9250 ID
            print(f"sensorId: {sensorId}")
        else:
            print("Cannot read sensorId")
        
        read_accel_gyro()
        read_mag()
        
        print(f"at {int(time.time() * 1000)} ms\n")
        time.sleep(2)  # Wait for 2 seconds

if __name__ == "__main__":
    main()
