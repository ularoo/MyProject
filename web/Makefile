.SUFFIXES : .c .o
OBJECTS = main.o login.o tree.o status.o alarmconfiguration.o fake_alarm.o rfconfiguration.o logs.o alarmlogs.o usermanagement.o cgiutil.o communications.o troubleshooting.o function_focus.o protocol.o system_download.o multipart.o crcmodel.o list.o reset.o reset_module.o waiting_page.o waiting_page_fpga_upgrade.o factory_default.o factory_default_select.o factory_default_rf.o factory_default_alarm.o dummy.o transfer_except.o upload_waiting.o summary_page.o wizard1.o wizard2.o debug.o install_wizard_select.o #logs_osc.o modem_activate.o
SRCS = main.c login.c status.c alarmconfiguration.c fake_alarm.c rfconfiguration.c logs.c alarmlogs.c usermanagement.c cgiutil.c communications.c troubleshooting.c function_focus.c protocol.c system_download.c multipart.c crcmodel.c list.c reset.c reset_module.c waiting_page.c waiting_page_fpga_upgrade.c factory_default.c factory_default_select.c factory_default_rf.c factory_default_alarm.c dummy.c transfer_except.c upload_waiting.c summary_page.c wizard1.c wizard2.c install_wizard_select.c #logs_osc.c modem_activate.c
CC=/opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux/bin/powerpc-e300c3-linux-gcc
#LFLAGS=-Wl,-elf2flt
STRIP = powerpc-e300c3-linux-strip
CFLAGS= -O2 -g -fomit-frame-pointer -fno-common -fno-builtin -Wall   -DEMBED -Dlinux -D__linux__ -Dunix -D__uClinux__ -DCOLLECT -DNEXT
TARGET = html.cgi
$(TARGET) : $(OBJECTS)
	$(CC) $(LFLAGS) -o $(TARGET) $(OBJECTS) #setjmp.o
	$(STRIP) $@
#	flthdr -s 65536 $(TARGET)
#	cp ./html.cgi ../romfs
	cp ./html.cgi /mnt/hgfs/temp/
	gzip -c ./html.cgi > /mnt/hgfs/temp/html.cgi.gz
#	cp ./html.cgi ../aws_app
#	cd ../romfs;./build_app; ./build_pkg
dep :
	$(CC) $(CFLAGS) -M *.c > .depend
clean :
	rm -rf $(OBJECTS) $(TARGET) core

include .depend
