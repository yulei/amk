/**
  ******************************************************************************
  * @file    usbh_hid_usage.c
  * @author  MCD Application Team
  * @brief   This file contain the USAGE page codes
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USDH_HID_USAGE_H
#define __USDH_HID_USAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */

/** @defgroup USBH_HID_USAGE
  * @brief This file is the Header file for usbh_hid_usage.c
  * @{
  */


/** @defgroup USBH_HID_USAGE_Exported_Types
  * @{
  */

/****************************************************/
/* HID 1.11 usage pages                             */
/****************************************************/

#define HID_USAGE_PAGE_UNDEFINED  (0x00)   /* Undefined */
/**** Top level pages */
#define HID_USAGE_PAGE_GEN_DES    (0x01)   /* Generic Desktop Controls*/
#define HID_USAGE_PAGE_SIM_CTR    (0x02)   /* Simulation Controls */
#define HID_USAGE_PAGE_VR_CTR     (0x03)   /* VR Controls */
#define HID_USAGE_PAGE_SPORT_CTR  (0x04)   /* Sport Controls */
#define HID_USAGE_PAGE_GAME_CTR   (0x05)   /* Game Controls */
#define HID_USAGE_PAGE_GEN_DEV    (0x06)   /* Generic Device Controls */
#define HID_USAGE_PAGE_KEYB       (0x07)   /* Keyboard/Keypad */
#define HID_USAGE_PAGE_LED        (0x08)   /* LEDs */
#define HID_USAGE_PAGE_BUTTON     (0x09)   /* Button */
#define HID_USAGE_PAGE_ORDINAL    (0x0A)   /* Ordinal */
#define HID_USAGE_PAGE_PHONE      (0x0B)   /* Telephony */
#define HID_USAGE_PAGE_CONSUMER   (0x0C)   /* Consumer */
#define HID_USAGE_PAGE_DIGITIZER  (0x0D)   /* Digitizer*/
/* 0E    Reserved */
#define HID_USAGE_PAGE_PID        (0x0F)   /* PID Page (force feedback and related devices) */
#define HID_USAGE_PAGE_UNICODE    (0x10)   /* Unicode */
/* 11-13 Reserved */
#define HID_USAGE_PAGE_ALNUM_DISP (0x14)   /* Alphanumeric Display */
/* 15-1f Reserved */
/**** END of top level pages */
/* 25-3f Reserved */
#define HID_USAGE_PAGE_MEDICAL    (0x40)   /* Medical Instruments */
/* 41-7F Reserved */
/*80-83 Monitor pages USB Device Class Definition for Monitor Devices
  84-87 Power pages USB Device Class Definition for Power Devices */
/* 88-8B Reserved */
#define HID_USAGE_PAGE_BARCODE    (0x8C)   /* Bar Code Scanner page */
#define HID_USAGE_PAGE_SCALE      (0x8D)   /* Scale page */
#define HID_USAGE_PAGE_MSR        (0x8E)   /* Magnetic Stripe Reading (MSR) Devices */
#define HID_USAGE_PAGE_POS        (0x8F)   /* Reserved Point of Sale pages */
#define HID_USAGE_PAGE_CAMERA_CTR (0x90)   /* Camera Control Page */
#define HID_USAGE_PAGE_ARCADE     (0x91)   /* Arcade Page */

/****************************************************/
/* Usage definitions for the "Generic Desktop" page */
/****************************************************/
#define HID_USAGE_UNDEFINED     (0x00)   /* Undefined */
#define HID_USAGE_POINTER       (0x01)   /* Pointer (Physical Collection) */
#define HID_USAGE_MOUSE         (0x02)   /* Mouse (Application Collection) */
/* 03 Reserved */
#define HID_USAGE_JOYSTICK      (0x04)   /* Joystick (Application Collection) */
#define HID_USAGE_GAMEPAD       (0x05)   /* Game Pad (Application Collection) */
#define HID_USAGE_KBD           (0x06)   /* Keyboard (Application Collection) */
#define HID_USAGE_KEYPAD        (0x07)   /* Keypad (Application Collection) */
#define HID_USAGE_MAX_CTR       (0x08)   /* Multi-axis Controller (Application Collection) */
/* 09-2F Reserved */
#define HID_USAGE_X             (0x30)   /* X (Dynamic Value) */
#define HID_USAGE_Y             (0x31)   /* Y (Dynamic Value) */
#define HID_USAGE_Z             (0x32)   /* Z (Dynamic Value) */
#define HID_USAGE_RX            (0x33)   /* Rx (Dynamic Value) */
#define HID_USAGE_RY            (0x34)   /* Ry (Dynamic Value) */
#define HID_USAGE_RZ            (0x35)   /* Rz (Dynamic Value) */
#define HID_USAGE_SLIDER        (0x36)   /* Slider (Dynamic Value) */
#define HID_USAGE_DIAL          (0x37)   /* Dial (Dynamic Value) */
#define HID_USAGE_WHEEL         (0x38)   /* Wheel (Dynamic Value) */
#define HID_USAGE_HATSW         (0x39)   /* Hat switch (Dynamic Value) */
#define HID_USAGE_COUNTEDBUF    (0x3A)   /* Counted Buffer (Logical Collection) */
#define HID_USAGE_BYTECOUNT     (0x3B)   /* Byte Count (Dynamic Value) */
#define HID_USAGE_MOTIONWAKE    (0x3C)   /* Motion Wakeup (One Shot Control) */
#define HID_USAGE_START         (0x3D)   /* Start (On/Off Control) */
#define HID_USAGE_SELECT        (0x3E)   /* Select (On/Off Control) */
/* 3F Reserved */
#define HID_USAGE_VX            (0x40)   /* Vx (Dynamic Value) */
#define HID_USAGE_VY            (0x41)   /* Vy (Dynamic Value) */
#define HID_USAGE_VZ            (0x42)   /* Vz (Dynamic Value) */
#define HID_USAGE_VBRX          (0x43)   /* Vbrx (Dynamic Value) */
#define HID_USAGE_VBRY          (0x44)   /* Vbry (Dynamic Value) */
#define HID_USAGE_VBRZ          (0x45)   /* Vbrz (Dynamic Value) */
#define HID_USAGE_VNO           (0x46)   /* Vno (Dynamic Value) */
#define HID_USAGE_FEATNOTIF     (0x47)   /* Feature Notification (Dynamic Value),(Dynamic Flag) */
/* 48-7F Reserved */
#define HID_USAGE_SYSCTL        (0x80)   /* System Control (Application Collection) */
#define HID_USAGE_PWDOWN        (0x81)   /* System Power Down (One Shot Control) */
#define HID_USAGE_SLEEP         (0x82)   /* System Sleep (One Shot Control) */
#define HID_USAGE_WAKEUP        (0x83)   /* System Wake Up (One Shot Control)  */
#define HID_USAGE_CONTEXTM      (0x84)   /* System Context Menu (One Shot Control) */
#define HID_USAGE_MAINM         (0x85)   /* System Main Menu (One Shot Control) */
#define HID_USAGE_APPM          (0x86)   /* System App Menu (One Shot Control) */
#define HID_USAGE_MENUHELP      (0x87)   /* System Menu Help (One Shot Control) */
#define HID_USAGE_MENUEXIT      (0x88)   /* System Menu Exit (One Shot Control) */
#define HID_USAGE_MENUSELECT    (0x89)   /* System Menu Select (One Shot Control) */
#define HID_USAGE_SYSM_RIGHT    (0x8A)   /* System Menu Right (Re-Trigger Control) */
#define HID_USAGE_SYSM_LEFT     (0x8B)   /* System Menu Left (Re-Trigger Control) */
#define HID_USAGE_SYSM_UP       (0x8C)   /* System Menu Up (Re-Trigger Control) */
#define HID_USAGE_SYSM_DOWN     (0x8D)   /* System Menu Down (Re-Trigger Control) */
#define HID_USAGE_COLDRESET     (0x8E)   /* System Cold Restart (One Shot Control) */
#define HID_USAGE_WARMRESET     (0x8F)   /* System Warm Restart (One Shot Control) */
#define HID_USAGE_DUP           (0x90)   /* D-pad Up (On/Off Control) */
#define HID_USAGE_DDOWN         (0x91)   /* D-pad Down (On/Off Control) */
#define HID_USAGE_DRIGHT        (0x92)   /* D-pad Right (On/Off Control) */
#define HID_USAGE_DLEFT         (0x93)   /* D-pad Left (On/Off Control) */
/* 94-9F Reserved */
#define HID_USAGE_SYS_DOCK      (0xA0)   /* System Dock (One Shot Control) */
#define HID_USAGE_SYS_UNDOCK    (0xA1)   /* System Undock (One Shot Control) */
#define HID_USAGE_SYS_SETUP     (0xA2)   /* System Setup (One Shot Control) */
#define HID_USAGE_SYS_BREAK     (0xA3)   /* System Break (One Shot Control) */
#define HID_USAGE_SYS_DBGBRK    (0xA4)   /* System Debugger Break (One Shot Control) */
#define HID_USAGE_APP_BRK       (0xA5)   /* Application Break (One Shot Control) */
#define HID_USAGE_APP_DBGBRK    (0xA6)   /* Application Debugger Break (One Shot Control) */
#define HID_USAGE_SYS_SPKMUTE   (0xA7)   /* System Speaker Mute (One Shot Control) */
#define HID_USAGE_SYS_HIBERN    (0xA8)   /* System Hibernate (One Shot Control) */
/* A9-AF Reserved */
#define HID_USAGE_SYS_SIDPINV   (0xB0)   /* System Display Invert (One Shot Control) */
#define HID_USAGE_SYS_DISPINT   (0xB1)   /* System Display Internal (One Shot Control) */
#define HID_USAGE_SYS_DISPEXT   (0xB2)   /* System Display External (One Shot Control) */
#define HID_USAGE_SYS_DISPBOTH  (0xB3)   /* System Display Both (One Shot Control) */
#define HID_USAGE_SYS_DISPDUAL  (0xB4)   /* System Display Dual (One Shot Control) */
#define HID_USAGE_SYS_DISPTGLIE (0xB5)   /* System Display Toggle Int/Ext (One Shot Control) */
#define HID_USAGE_SYS_DISP_SWAP (0xB6)   /* System Display Swap Primary/Secondary (One Shot Control) */
#define HID_USAGE_SYS_DIPS_LCDA (0xB7)   /* System Display LCD Autoscale (One Shot Control) */
/* B8-FFFF Reserved */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USDH_HID_USAGE_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
