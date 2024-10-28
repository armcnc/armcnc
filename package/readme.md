# ARMCNC PACKAGE

⚡ Develop and compile the related drivers and SDK ⚡

## Instructions

> This includes the drivers and SDKs required for the operation and daily use of armcnc and linuxcnc. You can customize and develop them further based on your needs. After completing the development, simply upload to Ubuntu, and all changes will be automatically handled when generating the deb package. You don’t need to worry about these adjustments.

### python

This is the Python SDK required for armcnc operation, which is used in the startup program of each machine.

### rdk_x5_gpio

This is the driver adapted for the RDK X5's 40 GPIO pins, used to control the machine's hardware modules.

```shell
# Compile and Build
sudo halcompile --install rdk_x5_gpio.c | grep Linking
```

```shell
# Example usage
loadrt [KINS]KINEMATICS
loadrt [EMCMOT]EMCMOT base_period_nsec=[EMCMOT]BASE_PERIOD servo_period_nsec=[EMCMOT]SERVO_PERIOD num_joints=[KINS]JOINTS
loadrt armcncio pwm_types="p,p,p,p,p,f" in_pins="19,20,21" out_pins="4,5,6"
```