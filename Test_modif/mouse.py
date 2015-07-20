import ctypes


mouse = ctypes.CDLL("/home/me/Documents/Delta Robot/Test_modif/spacenavig_python.so")
readEvent = mouse.readEvent
init = mouse.init
main = mouse.main
print(readEvent)
print(init)
print(main)
main()

