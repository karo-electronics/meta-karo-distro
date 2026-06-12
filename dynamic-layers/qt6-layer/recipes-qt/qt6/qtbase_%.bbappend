# when using userland graphic KHR/khrplatform.h is provided by userland but virtual/libgl is provided by mesa-gl where
# we explicitly delete KHR/khrplatform.h since its already coming from userland package
DEPENDS:append:stm32mpcommon = " ${@bb.utils.contains('PREFERRED_PROVIDER_virtual/egl','mesa', '', 'gcnano-userland', d)}"
