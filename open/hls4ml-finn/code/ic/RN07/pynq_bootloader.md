# Boot from Flash
0. Under the `<project>_standalone` project, in `submitter_implemented.h`, make sure to set `#define EE_CFG_ENERGY_MODE` to `1`. This enables the configuation changes required to run energy benchmarks
1. In the SDK, select  `<project>_bootloader` in _Project Explorer_
2. Click: `Xilinx >> Create Boot Image`
3. Click: `Add`

<img src="img/01.png" width=600/>

4. Select: `<project>_standalone`

<img src="img/02.png" width=600/>

5. Click: `Create Image`
6. Click: `Xilinx >> Program Flash`
7. Click: `Image File:` - `Browse`

<img src="img/03.png" width=400/>

8. Select: `BOOT.bin` (check the full path at the top of the following image)

<img src="img/04.png" width=600/>

9. Click: `Ok`
10. Select: `<project>_fsbl.elf`

<img src="img/05.png" width=600/>

11. Click: `Ok`
12. **Make sure that the jumper on the board is on QSPI**
3. Click: `Program`
