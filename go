#! /bin/sh

# Compile, program and run eLua.

# Usage, from an elua source directory hereunder:
#	../go [options]
# options are a selection of:
usage="	avr32program	Use JTAG programmer instead of dfu bootloader
	batchisp3	Use batchisp3 programmer instead of dfu-programmer
	dfu		Use dfu-programmer (which is the default)
	noprog		Don't program the firmware to the board
	evk1100		Make image for EVK1100 board instead of Mizar32
	128/256/512	Build for 128KB 256KB or 512KB models (default 256)
	emblod		Prepare image to put on SD card for emBLOD bootloader
	lua,lualong	Compile eLua with floating point or integer variables
	lualonglong	Compile eLua with 64-bit integer variables
	picoc,picoclong Compile picoc with floating point or without
	picolisp	Compile picolisp
	tinyscheme	Compile tinyscheme
	simple		Use simple allocator (default for 128)
	multiple	Use multiple allocator (default for 256/512)
	newlib		Use newlib's allocator
        allocator=x	Synonym for one of the above three
	optram=n	Set memory optimization (default optram=2)
	rostrings	Enable read-only strings and functions
	avr32-tools	Use jsnyder's gcc-4.4
	x-tools		Use crosstool-ng's compiler
	as4e-ide	Use AVR32Studio's compiler
	scons		Use the scons build system
	build_elua	Use the Lua build system
	clean		Remove all object files from the source tree first
        build_date      Include the build date (as suffix) in output file
	-a [opts]	Build all supported combinations"

set -e

JFLAG="-j 8"

if [ "$1" = "-a" ]; then
	shift
	for target in lua lualong lualonglong \
		      picoc picoclong \
		      picolisp tinyscheme
	do
		echo -n "${target}: "
		if $0 $target noprog "$@" > errs 2>&1; then
			echo "OK"
		else
			echo "FAIL"
		fi
	done
	exit $?
fi

# $board, $cpu and $target are just the values to pass to scons' "board=X" args.
# The other optional scons flags' values are of the form "allocator=simple",
# so $allocator's value is either "" or, for example, "allocator=simple"
# This lets us say "scons $allocator" below, to let scons choose a default
# value if not set.

board=mizar32
flash=256
# cpu is automatically derived from flash size
lang=
target=
allocator=
programmer=dfu
bootloader=
media="$(echo /media/????-????)"
optram=
rostrings=
build_system=scons
build_date=false
gcc=
clean=false

# Process command-line options and override default settings
for arg
do
  case "$arg" in

  avr32program|batchisp3|dfu|noprog) programmer="$arg" ;;

  evk1100) board=ATEVK1100; flash=512 ;;

  emblod) bootloader="bootloader=emblod"
	  programmer=emblod ;;

  board=*)  eval $arg ;;

  lua) lang=elua; target=fp ;;
  lualong) lang=elua; target=long ;;
  lualonglong) lang=elua; target=longlong ;;
  picoc)  lang=picoc; target=fp ;;
  picoclong)  lang=picoc; target=long ;;
  picolisp)  lang=picolisp; target=regular ;;
  tinyscheme)  lang=tinyscheme; target=fp ;;

  128|256|512) flash=$arg ;;

  allocator=*)            allocator="$arg" ;;
  simple|multiple|newlib) allocator="allocator=$arg" ;;

  optram=[012]) optram="$arg" ;;
  rostrings)      rostrings="rostrings=1" ;;
  rostrings=[01]) rostrings="$arg" ;;

  gcc-4.3|gcc-4.4|avr32-tools|x-tools|as4e-ide) gcc=$arg ;;

  scons|build_elua) build_system=$arg ;;

  clean) clean=true ;;

  build_date) build_date=true ;;

  *) echo "What's \"$arg\"?  Options are:
$usage"
    exit 1
  esac
done

# Apply defaults
if [ ! "$lang" ]; then
  lang=elua
fi

if [ ! "$target" ]; then
  lang=elua
  case "$flash" in
  128)	   target=long ;;
  256|512) target=long ;;
  *)	echo "target undefined and flash size unrecognized" 1>&2
	exit 1 ;;
  esac
fi

# If optram is not set, 128 model defaults to the smallest code size
if [ ! "$optram" ]; then
  case "$flash" in
  128)	optram="optram=0" ;;
  esac
fi

# If rostrings is not set, 128 model defaults to the smallest code size
if [ ! "$rostrings" ]; then
  case "$flash" in
  128)	rostrings="rostrings=0" ;;
  esac
fi

# If allocator is not set, 128 model defaults to the smallest code size
if [ ! "$allocator" ]; then
  case "$flash" in
  128)	allocator="allocator=simple" ;;
  esac
fi

case "$gcc" in
avr32-tools|x-tools|as4e-ide)
	case "$gcc" in
	avr32-tools) BIN=$HOME/avr32-tools/bin ;;
	x-tools)     BIN=$HOME/x-tools/avr32-unknown-none/bin ;;
	as4e-ide)    BIN=/usr/local/as4e-ide/plugins/com.atmel.avr.toolchains.linux.x86_3.0.0.201009140852/os/linux/x86/bin ;;
	esac
        if [ ! -x $BIN/avr32-gcc ]; then
	  echo "$BIN/avr32-gcc not found" 1>&2
          exit 1
        fi
	PATH="$BIN:$PATH"
	export PATH
	# Check we got it right...
	if [ "`which avr32-gcc`" != "$BIN/avr32-gcc" ]; then
		echo "We're getting the wrong toolchain" 1>&2
		exit 1
	fi
	;;
"")
	: # Uses the default avr32-gcc
	;;
*)
	echo "BUG in $0: gcc=$gcc not recognized" 1>&2
	exit 1
	;;
esac

# Values derived from the above
part=at32uc3a0${flash}
cpu=$(echo $part | tr a-z A-Z)

if $clean; then
    scons board=$board lang=$lang target=$target -c
    rm -rf .build	# build_lua.lua object dir
    rm -f *.elf *.hex *.bin config.log
fi

# Compile it

case "$build_system" in
scons)
    echo scons $JFLAG \
	board=$board cpu=$cpu lang=$lang target=$target $bootloader $allocator \
	$optram $rostrings build_date=$build_date
    scons $JFLAG \
	board=$board cpu=$cpu lang=$lang target=$target $bootloader $allocator \
	$optram $rostrings build_date=$build_date || exit 1
    ;;
build_elua)
    echo lua build_elua.lua \
	board=$board cpu=$cpu target=$target $bootloader $allocator \
	$optram $rostrings build_date=$build_date
    lua build_elua.lua \
	board=$board cpu=$cpu target=$target $bootloader $allocator \
	$optram $rostrings build_date=$build_date || exit 1
    ;;
*)
    echo "Unrecognized build system '$build_system'"
    exit 1
esac

# $firmware: basename of output filenames, different for elua and Alcor6L
# raman: The date now is an optional parameter.
date=`date +%Y%m%d`
case "$(basename $(pwd))" in
elua*)	firmware=elua_${target}_${part} ;;
*)	case "$lang" in
	elua|picoc|picolisp|tinyscheme)
        if [ ! "$build_date" ]; then
	    firmware=Alcor6L_${lang}_${target}_${part}_${date}
        else
	    firmware=Alcor6L_${lang}_${target}_${part}
        fi ;;
	*)   echo "Unrecognised lang \"$lang\"" ; exit 1 ;;
	esac ;;
esac

# Make hex and bin versions for distribution

echo Making .hex and .bin versions...
avr32-objcopy -O ihex $firmware.elf $firmware.hex
avr32-objcopy -O binary $firmware.elf $firmware.bin

# Program it to the chip

case "$programmer" in
noprog)
  ;;
avr32program)
  avr32program program -cxtal -e -v -f internal@0x80000000,${flash}kB $firmware.elf
  avr32program reset -r
  ;;
batchisp3)
  echo "=== If erasure fails, wait 10 seconds and press I ==="
  batchisp3 -hardware usb -device $part -operation \
	erase f memory flash blankcheck \
	loadbuffer $PWD/$firmware.elf \
	program verify start reset 0
  ;;
dfu)

  # Make sure device is present. Quit if not.
  sudo dfu-programmer $part get || exit 1

  echo dfu-programmer $part erase
  sudo dfu-programmer $part erase || {
	sleep 1
	until sudo dfu-programmer $part get > /dev/null
	do
		echo "Erase failed. Waiting..."
		sleep 1
	done
  }

  echo dfu-programmer $part flash $firmware.hex
  sudo dfu-programmer $part flash $firmware.hex
  false && {
    echo "Verifying..."
    avr32-objcopy -O binary $firmware.elf $firmware.bin
    size="`ls -l $firmware.bin | awk '{print $5}'`"	# size of elua
    echo $size
    # Drop 8k boot loader from dump, and compare only up to the size of elua
    sudo dfu-programmer $part dump | \
	tail -c +8193 | head -c "$size" | \
	cmp -l - $firmware.bin
  }
  echo dfu-programmer $part start
  sudo dfu-programmer $part start
  ;;
emblod)
  avr32-objcopy -O binary $firmware.elf $firmware.bin
  cp $firmware.bin "$media"/autorun.bin
  umount "$media"
  ;;
esac
