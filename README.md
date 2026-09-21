# ESP32 Mini Drift Car

A small 2WD-style mini drift car controlled by a PS3 Bluetooth controller.

## Features

- ESP32-WROOM-32
- PS3 Bluetooth controller
- L298N dual H-bridge motor driver
- Brushed DC rear traction motor
- N20 DC front steering motor
- Normal mode and Drift mode
- Green LED = Normal mode
- Red LED = Drift mode
- Right joystick X-axis = steering
- R2 = forward
- L2 = reverse
- Motors stop when the PS3 controller disconnects
- Inverted steering direction

> Current repository version uses the L298N setup that was tested successfully.
> A future hardware revision can replace the L298N with a more efficient driver such as TB6612FNG or a higher-current driver if the motor requires it.

## Hardware

- ESP32-WROOM-32 development board
- L298N motor driver
- PS3 controller
- Brushed DC motor for rear traction
- N20 DC motor for steering
- Green LED
- Red LED
- 220–330 ohm resistors, 2x
- Battery suitable for the motors
- ON/OFF switch
- Chassis, wheels, steering mechanism and mechanical linkage

## Pinout

### L298N

| ESP32 | L298N | Function |
|---|---|---|
| GPIO 25 | ENA | Rear motor PWM |
| GPIO 26 | IN1 | Rear motor direction |
| GPIO 27 | IN2 | Rear motor direction |
| GPIO 32 | ENB | Steering motor PWM |
| GPIO 33 | IN3 | Steering motor direction |
| GPIO 14 | IN4 | Steering motor direction |
| GND | GND | Common ground |

### LEDs

| ESP32 | Component |
|---|---|
| GPIO 4 | 220–330 ohm resistor -> green LED -> GND |
| GPIO 5 | 220–330 ohm resistor -> red LED -> GND |

### Motors

- L298N OUT1/OUT2 -> rear traction motor
- L298N OUT3/OUT4 -> N20 steering motor

### Power

For the tested arrangement:

- Battery positive -> L298N 12V/VMS
- Battery negative -> L298N GND
- ESP32 GND -> L298N GND
- ESP32 may be powered separately from a 5 V USB source during testing.

If using the L298N 5 V regulator to power the ESP32, verify the module's 5 V output with a multimeter before connecting it to the ESP32 5V/VIN pin.

**Never connect the motor battery directly to the ESP32 3V3 pin.**

## Switch

Place the main ON/OFF switch in series with the battery positive:

```text
Battery + -> ON/OFF switch -> L298N VMS/12V
Battery - --------------------> L298N GND
```

Keep the ESP32 and L298N grounds common.

## Controller

The ESP32 Bluetooth MAC used by the current firmware is:

`3C:71:BF:C5:D0:5C`

The firmware calls:

```cpp
Ps3.begin("3C:71:BF:C5:D0:5C");
```

Pair the PS3 controller with the ESP32 using a compatible PS3 pairing workflow before driving.

## Controls

| PS3 control | Function |
|---|---|
| X | Normal mode |
| Triangle | Drift mode |
| R2 | Forward |
| L2 | Reverse |
| Right stick left | Steer right |
| Right stick right | Steer left |

### Modes

Normal mode:
- Green LED ON
- Red LED OFF
- Rear motor maximum command = 180/255

Drift mode:
- Red LED ON
- Green LED OFF
- Rear motor maximum command = 255/255 (100%)

## Software

The project uses the `Ps3Controller` Arduino library.

The ESP32 board package must be installed in Arduino IDE and a compatible `Ps3Controller` library must be available.

### Upload

1. Select the correct ESP32 board.
2. Select the ESP32 COM port.
3. Connect the ESP32 by USB.
4. Upload `src/mini_drift_car.ino`.
5. If the board requires it, hold BOOT during the `Connecting...` phase.

## Safety

- Test with the wheels off the ground first.
- Do not hold the N20 steering motor against a mechanical stop for long periods.
- Use a battery appropriate for the motor voltage and current.
- A motor that spins freely but cannot move the car can indicate insufficient battery current, excessive drivetrain friction, or voltage loss in the driver.
- The L298N has significant voltage drop and can be inefficient for high-current traction motors.
- Add a fuse and a physical emergency power switch for a more robust build.

## Upgrade path

If the car is strong with the wheels lifted but weak on the ground, investigate:

1. Battery voltage under load.
2. Battery current capability.
3. Motor stall/current rating.
4. Mechanical friction and wheel grip.
5. L298N voltage drop.

For a future revision:
- TB6612FNG can control two smaller DC motors.
- BTS7960 is a high-current single-motor driver and is better suited to a high-current traction motor than a TB6612FNG, while a second driver can handle the N20 steering motor.

