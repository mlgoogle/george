
###############################################################################
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -D__DEBUG_CORE__ -DFCGI_STD  #-DCHECK_HEADER-DTEST
RCPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD #-DTEST
## for release
##CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

LIB_DIR = ../../library
BASE_DIR = ./../pub
PLUGIN_DIR = ../plugins
## 头文件
INCLUDE_PATHS = -I$(LIB_DIR)/mylib/public/\
		-I$(LIB_DIR)/mylib/base\
		-I/usr/local/lib\
		-I$(BASE_DIR)/\
		-I$(PLUGIN_DIR)\
		-I$(LIB_DIR)/third_party/breakpad/src

## 库
LINK_PATHS =  -L./
LINK_LIBRARYS = -lrt -lpthread -lfcgi++ \


# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@
RCOMPILE = $(COMPILER) $(RCPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)
LINKALL = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $?  $(LINK_LIBRARYS)

POBJ =  mig_log.o core_interface.o http_formate.o fcgimodule.o logic_comm.o
OUTPUT_FILE = /var/www/cgi-bin/george
#############################################################################################################################
all: clean proc delete-temp-file

proc: vipproc

######################vipproc###############################################
VIPNAME = vip_
VIPPROC_PATH = /vip
VIP_OBJECT_PATH = ./obj$(VIPPROC_PATH)
OTJERPROC_VERSION = /v1
vipproc:$(VIPPROC_PATH)/vipnews.fcgi $(VIPPROC_PATH)/hotvip.fcgi $(VIPPROC_PATH)/viparticle.fcgi $(VIPPROC_PATH)/subcribevip.fcgi $(VIPPROC_PATH)/setsub.fcgi

$(VIPPROC_PATH)/vipnews.fcgi: $(VIP_OBJECT_PATH)/$(VIPNAME)vipnews.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(VIPPROC_PATH)$(OTJERPROC_VERSION)/vipnews.fcgi

$(VIPPROC_PATH)/hotvip.fcgi: $(VIP_OBJECT_PATH)/$(VIPNAME)hotvip.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(VIPPROC_PATH)$(OTJERPROC_VERSION)/hotvip.fcgi

$(VIPPROC_PATH)/viparticle.fcgi: $(VIP_OBJECT_PATH)/$(VIPNAME)viparticle.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(VIPPROC_PATH)$(OTJERPROC_VERSION)/viparticle.fcgi
	
$(VIPPROC_PATH)/subcribevip.fcgi: $(VIP_OBJECT_PATH)/$(VIPNAME)subcribevip.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(VIPPROC_PATH)$(OTJERPROC_VERSION)/subcribevip.fcgi
	
$(VIPPROC_PATH)/setsub.fcgi: $(VIP_OBJECT_PATH)/$(VIPNAME)setsub.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(VIPPROC_PATH)$(OTJERPROC_VERSION)/setsub.fcgi
	
### 编译

$(VIP_OBJECT_PATH)/$(VIPNAME)vipnews.o:
	$(COMPILE) .$(VIPPROC_PATH)$(OTJERPROC_VERSION)/vipnews/vipnews.cc

$(VIP_OBJECT_PATH)/$(VIPNAME)hotvip.o:
	$(COMPILE) .$(VIPPROC_PATH)$(OTJERPROC_VERSION)/hotvip/hotvip.cc
	
$(VIP_OBJECT_PATH)/$(VIPNAME)viparticle.o:
	$(COMPILE) .$(VIPPROC_PATH)$(OTJERPROC_VERSION)/viparticle/viparticle.cc
	
$(VIP_OBJECT_PATH)/$(VIPNAME)subcribevip.o:
	$(COMPILE) .$(VIPPROC_PATH)$(OTJERPROC_VERSION)/subcribevip/subcribevip.cc
	
$(VIP_OBJECT_PATH)/$(VIPNAME)setsub.o:
	$(COMPILE) .$(VIPPROC_PATH)$(OTJERPROC_VERSION)/setsub/setsub.cc



clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)./*.fcgi

delete-temp-file:
	-rm -f *.o

############public###############################

logic_comm.o:
	$(COMPILE) $(LIB_DIR)/mylib/base/logic/logic_comm.cc
	
mig_log.o:
	$(COMPILE) $(LIB_DIR)/mylib/public/log/mig_log.cc

core_interface.o:
	$(COMPILE) $(LIB_DIR)/mylib/public/comm/core_interface.cc
	
http_formate.o:
	$(COMPILE) $(LIB_DIR)/mylib/public/http/http_formate.cc

fcgimodule.o:
	$(COMPILE) $(BASE_DIR)/fcgimodule/fcgimodule.cc
