import os

# toolchains options
ARCH='arm'
CPU='cortex-m4'
CROSS_TOOL='gcc'

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')

# cross_tool provides the cross compiler
# EXEC_PATH is the compiler execute path, for example, CodeSourcery, Keil MDK, IAR
if  CROSS_TOOL == 'gcc':
    PLATFORM 	= 'gcc'
    EXEC_PATH 	= r'E:/Program Files/CodeSourcery/Sourcery G++ Lite/bin'
elif CROSS_TOOL == 'keil':
    PLATFORM 	= 'armcc'
    EXEC_PATH 	= r'C:/Keil'
elif CROSS_TOOL == 'iar':
    print '================ERROR============================'
    print 'Not support iar yet!'
    print '================================================='
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
	EXEC_PATH = os.getenv('RTT_EXEC_PATH')

BUILD = 'debug'
STM32_TYPE = 'STM32F4XX'

# toolchains
PREFIX = 'arm-none-eabi-'
CC = PREFIX + 'gcc'
CXX = PREFIX + 'g++'
AS = PREFIX + 'gcc'
AR = PREFIX + 'ar'
LINK = PREFIX + 'gcc'
TARGET_EXT = 'axf'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'
STRIP = PREFIX + 'strip'

DEVICE = '  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections'
CFLAGS = DEVICE + ' -g -Wall -DSTM32F405VG -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -D__ASSEMBLY__ -D__FPU_USED -eentry'
AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp -Wa,-mimplicit-it=thumb '
LFLAGS = DEVICE + ' -lm -lgcc -lc' + ' -nostartfiles -Wl,--gc-sections,-Map=rtthread-stm32.map,-cref,-u,Reset_Handler -T stm32_rom.ld'

CPATH = ''
LPATH = ''

if BUILD == 'debug':
    CFLAGS += ' -O0 -gdwarf-2'
    AFLAGS += ' -gdwarf-2'
else:
    CFLAGS += ' -O2'

POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'

# module setting 
CXXFLAGS = ' -Woverloaded-virtual -fno-exceptions -fno-rtti '
M_CFLAGS = CFLAGS + ' -mlong-calls -fPIC '
M_CXXFLAGS = CXXFLAGS + ' -mlong-calls -fPIC'
M_LFLAGS = DEVICE + CXXFLAGS + ' -Wl,--gc-sections,-z,max-page-size=0x4' +\
                                    ' -shared -fPIC -nostartfiles -static-libgcc'
M_POST_ACTION = STRIP + ' -R .hash $TARGET\n' + SIZE + ' $TARGET \n'

