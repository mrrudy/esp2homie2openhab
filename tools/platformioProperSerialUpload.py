Import('env')
from base64 import b64decode

#
# Dump build environment (for debug)
# print env.Dump()
#

env.Replace(UPLOADCMD='esptool.py ' + '--port "${UPLOAD_PORT}" ' + b64decode(ARGUMENTS.get("CUSTOM_OPTION")) + ' "${BUILD_DIR}/${PROGNAME}.bin" ')
#env.Replace(UPLOAD_PORT='3333')
# uncomment line below to see environment variables
#print "trying to make line:" + b64decode(ARGUMENTS.get("CUSTOM_OPTION")) + ' ' + map(str, BUILD_DIR) + '/${PROGNAME}.bin'
print "uploadline: ", env.get("UPLOADCMD")

# print ARGUMENTS

#env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", uploadME)

#esptool.py --port /dev/ttyUSB0 write_flash --flash_mode dout --verify 0x00000 .pioenvs/esp01_1m/firmware.bin
