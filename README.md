qf_wbfpga_pio
=
#### QuickLogic EOS S3:Cortex-M4 to FPGA Fabric via WISHBONE bus Sample Code.
## I.Set up environment
there's two way to use this project. You may choose each one which more comfortable.
### i. Clone the QORC-SDK before clone this project 
```
git clone -b v1.10.0 https://github.com/QuickLogic-Corp/qorc-sdk.git
cd qorc-sdk/qf_apps
git clone https://github.com/panda5mt/qf_wbfpga_pio.git
cd ../
source envsetup.sh
cd qf_apps/qf_wbfpga_pio/
make clean -C GCC_Projects/
make -C GCC_Projects/
```
you can download *.bin compiled-binary which in <code>[qf_wbfpga_pio/GCC_Projects/output/qf_wbfpga_pio.bin](qf_wbfpga_pio/GCC_Projects/output/qf_wbfpga_pio.bin)</code>.

### ii. Use Dockerfile
Or download <code>[Dockerfile](Dockerfile)</code> and execute below command on your Dockerfile's directory.
```
$ docker build -t debian-qorc --build-arg CACHEBUST=$(date +%s) .
```
an image name like <code>debian-qorc</code>, you can set any other words you like.<br>

Execute below after built Docker image.
```
$ docker run -it debian-qorc 
```
then, build the Projects. execute on your bash terminal.
```
$ cd /qorc-sdk/qf_apps/qf_wbfpga_pio
$ ~/qlogic_build.sh
```
at 1st time, this command take you several minutes, because download and install all tools which QORC-SDK need.  
you can download *.bin compiled-binary which in <code>[qf_wbfpga_pio/GCC_Projects/output/qf_wbfpga_pio.bin](qf_wbfpga_pio/GCC_Projects/output/qf_wbfpga_pio.bin)</code>.