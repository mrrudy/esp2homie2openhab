Import('env')
from base64 import b64decode

#
# Dump build environment (for debug)
# print env.Dump()
#

env.Replace(UPLOADCMD='./.piolibdeps/Homie/scripts/ota_updater/ota_updater.py ' + b64decode(ARGUMENTS.get("CUSTOM_OPTION")) + ' "${BUILD_DIR}/${PROGNAME}.bin" ')
env.Replace(UPLOAD_PORT='3333')
# uncomment line below to see environment variables
#print "trying to make line:" + b64decode(ARGUMENTS.get("CUSTOM_OPTION")) + ' ' + map(str, BUILD_DIR) + '/${PROGNAME}.bin'
print "uploadline: ", env.get("UPLOADCMD")

# print ARGUMENTS

# .piolibdeps/Homie/scripts/ota_updater/ota_updater.py .pioenvs/esp01_1m/firmware.bin -l 192.168.1.25 -t "homie/" -i "5ccf7f9f306c"

#env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", uploadME)
