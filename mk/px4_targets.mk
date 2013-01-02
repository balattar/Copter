# PX4 build is via external build system

ifneq ($(PX4_ROOT),)

px4:
	make -C $(PX4_ROOT) EXTERNAL_APPS=$(PWD)

px4-clean: clean
	make -C $(PX4_ROOT) EXTERNAL_APPS=$(PWD) clean

px4-upload:
	make -C $(PX4_ROOT) EXTERNAL_APPS=$(PWD) upload

else

px4:
	$(error ERROR: You need to add PX4_ROOT to your config.mk)

endif
