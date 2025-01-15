# Texas Instruments TAS2562 and TAS2564 Audio Amplifier Driver For Windows

## Notes

- This driver has not been completed yet and bugs may exist
- This driver is mainly applicable to Xiaomi devices that use TAS2562 or TAS2564, specifically Redmi Note 9 Series and POCO X3 series

## Important for people wanting to try it out on other devices

- Since all settings are hardcoded inside driver, **make sure you changed them according to your device settings**
- Default Mode set by the driver is 24Bit Bitwidth and 16Bit Slot Length

## Issues

- Sometimes it takes about 5 seconds for sound to appear(POCO X3 PRO)
- Sometimes speakers can produce very loud sound while switching between music tracks or videos

## Credits

- [Sunflower2333](https://github.com/sunflower2333) for his AudFilter driver from [tas2557_win](https://github.com/sunflower2333/tas2557_win)
- [Xiaomi](https://github.com/MiCode) for their downstream tas256x-codec driver
