
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
OUTPUT_FILE = /var/www/cgi-bin/georget
#############################################################################################################################
all: clean proc delete-temp-file

proc: vipproc newsproc searchproc stockproc userproc

######################userproc###############################################
USERNAME = user_
USERPROC_PATH = /user
USER_OBJECT_PATH = ./obj$(NEWSPROC_PATH)
OTJERPROC_VERSION = /v1
userproc:$(USERPROC_PATH)/registeruser.fcgi $(USERPROC_PATH)/registerver.fcgi $(USERPROC_PATH)/loginuser.fcgi $(USERPROC_PATH)/loginver.fcgi  $(USERPROC_PATH)/smslogin.fcgi

$(USERPROC_PATH)/registeruser.fcgi: $(USER_OBJECT_PATH)/$(USERNAME)registeruser.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/registeruser.fcgi

$(USERPROC_PATH)/loginuser.fcgi: $(USER_OBJECT_PATH)/$(USERNAME)loginuser.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/loginuser.fcgi

$(USERPROC_PATH)/registerver.fcgi: $(USER_OBJECT_PATH)/$(USERNAME)registerver.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/registerver.fcgi

$(USERPROC_PATH)/loginver.fcgi: $(USER_OBJECT_PATH)/$(USERNAME)loginver.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/loginver.fcgi

$(USERPROC_PATH)/smslogin.fcgi: $(USER_OBJECT_PATH)/$(USERNAME)smslogin.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(USERPROC_PATH)$(OTJERPROC_VERSION)/smslogin.fcgi
	
### 编译
$(USER_OBJECT_PATH)/$(USERNAME)loginuser.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/loginuser/loginuser.cc

$(USER_OBJECT_PATH)/$(USERNAME)registeruser.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/registeruser/registeruser.cc

$(USER_OBJECT_PATH)/$(USERNAME)registerver.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/registerver/registerver.cc

$(USER_OBJECT_PATH)/$(USERNAME)loginver.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/loginver/loginver.cc

$(USER_OBJECT_PATH)/$(USERNAME)smslogin.o:
	$(COMPILE) .$(USERPROC_PATH)$(OTJERPROC_VERSION)/smslogin/smslogin.cc
	

######################stockproc###############################################
STOCKNAME = stock_
STOCKPROC_PATH = /stock
STOCK_OBJECT_PATH = ./obj$(STOCKPROC_PATH)
OTJERPROC_VERSION = /v1
stockproc:$(STOCKPROC_PATH)/stockhotdiagram.fcgi $(STOCKPROC_PATH)/stocklimitdata.fcgi

$(STOCKPROC_PATH)/stockhotdiagram.fcgi: $(STOCK_OBJECT_PATH)/$(STOCKNAME)stockhotdiagram.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(STOCKPROC_PATH)$(OTJERPROC_VERSION)/stockhotdiagram.fcgi

$(STOCKPROC_PATH)/stocklimitdata.fcgi: $(STOCK_OBJECT_PATH)/$(STOCKNAME)stocklimitdata.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(STOCKPROC_PATH)$(OTJERPROC_VERSION)/stocklimitdata.fcgi
	
### 编译

$(STOCK_OBJECT_PATH)/$(STOCKNAME)stockhotdiagram.o:
	$(COMPILE) .$(STOCKPROC_PATH)$(OTJERPROC_VERSION)/stockhotdiagram/stockhotdiagram.cc

$(STOCK_OBJECT_PATH)/$(STOCKNAME)stocklimitdata.o:
	$(COMPILE) .$(STOCKPROC_PATH)$(OTJERPROC_VERSION)/stocklimitdata/stocklimitdata.cc
	
	
######################searchproc###############################################
SEARCHNAME = search_
SEARCHPROC_PATH = /search
SEARCH_OBJECT_PATH = ./obj$(SEARCHPROC_PATH)
OTJERPROC_VERSION = /v1
searchproc:$(SEARCHPROC_PATH)/altersubscribe.fcgi $(SEARCHPROC_PATH)/hotsubscribe.fcgi $(SEARCHPROC_PATH)/searchstock.fcgi $(SEARCHPROC_PATH)/usersubscribe.fcgi

$(SEARCHPROC_PATH)/altersubscribe.fcgi: $(SEARCH_OBJECT_PATH)/$(SEARCHNAME)altersubscribe.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SEARCHPROC_PATH)$(OTJERPROC_VERSION)/altersubscribe.fcgi

$(SEARCHPROC_PATH)/hotsubscribe.fcgi: $(SEARCH_OBJECT_PATH)/$(SEARCHNAME)hotsubscribe.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SEARCHPROC_PATH)$(OTJERPROC_VERSION)/hotsubscribe.fcgi

$(SEARCHPROC_PATH)/searchstock.fcgi: $(SEARCH_OBJECT_PATH)/$(SEARCHNAME)searchstock.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SEARCHPROC_PATH)$(OTJERPROC_VERSION)/searchstock.fcgi

$(SEARCHPROC_PATH)/usersubscribe.fcgi: $(SEARCH_OBJECT_PATH)/$(SEARCHNAME)usersubscribe.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(SEARCHPROC_PATH)$(OTJERPROC_VERSION)/usersubscribe.fcgi	
	
### 编译

$(SEARCH_OBJECT_PATH)/$(SEARCHNAME)altersubscribe.o:
	$(COMPILE) .$(SEARCHPROC_PATH)$(OTJERPROC_VERSION)/altersubscribe/altersubscribe.cc

$(SEARCH_OBJECT_PATH)/$(SEARCHNAME)hotsubscribe.o:
	$(COMPILE) .$(SEARCHPROC_PATH)$(OTJERPROC_VERSION)/hotsubscribe/hotsubscribe.cc

$(SEARCH_OBJECT_PATH)/$(SEARCHNAME)searchstock.o:
	$(COMPILE) .$(SEARCHPROC_PATH)$(OTJERPROC_VERSION)/searchstock/searchstock.cc

$(SEARCH_OBJECT_PATH)/$(SEARCHNAME)usersubscribe.o:
	$(COMPILE) .$(SEARCHPROC_PATH)$(OTJERPROC_VERSION)/usersubscribe/usersubscribe.cc

	
######################newsproc###############################################
NEWSNAME = news_
NEWSPROC_PATH = /news
NEWS_OBJECT_PATH = ./obj$(NEWSPROC_PATH)
OTJERPROC_VERSION = /v1
newsproc:$(NEWSPROC_PATH)/branchnews.fcgi $(NEWSPROC_PATH)/countnews.fcgi $(NEWSPROC_PATH)/detailsnews.fcgi

$(NEWSPROC_PATH)/branchnews.fcgi: $(NEWS_OBJECT_PATH)/$(NEWSNAME)branchnews.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(NEWSPROC_PATH)$(OTJERPROC_VERSION)/branchnews.fcgi

$(NEWSPROC_PATH)/countnews.fcgi: $(NEWS_OBJECT_PATH)/$(NEWSNAME)countnews.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(NEWSPROC_PATH)$(OTJERPROC_VERSION)/countnews.fcgi

$(NEWSPROC_PATH)/detailsnews.fcgi: $(NEWS_OBJECT_PATH)/$(NEWSNAME)detailsnews.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(NEWSPROC_PATH)$(OTJERPROC_VERSION)/detailsnews.fcgi
	
### 编译

$(NEWS_OBJECT_PATH)/$(NEWSNAME)branchnews.o:
	$(COMPILE) .$(NEWSPROC_PATH)$(OTJERPROC_VERSION)/branchnews/branchnews.cc

$(NEWS_OBJECT_PATH)/$(NEWSNAME)countnews.o:
	$(COMPILE) .$(NEWSPROC_PATH)$(OTJERPROC_VERSION)/countnews/countnews.cc

$(NEWS_OBJECT_PATH)/$(NEWSNAME)detailsnews.o:
	$(COMPILE) .$(NEWSPROC_PATH)$(OTJERPROC_VERSION)/detailsnews/detailsnews.cc

######################vipproc###############################################
VIPNAME = vip_
VIPPROC_PATH = /vip
VIP_OBJECT_PATH = ./obj$(VIPPROC_PATH)
OTJERPROC_VERSION = /v1
vipproc:$(VIPPROC_PATH)/vipnews.fcgi $(VIPPROC_PATH)/hotvip.fcgi $(VIPPROC_PATH)/viparticle.fcgi $(VIPPROC_PATH)/subcribevip.fcgi $(VIPPROC_PATH)/setsub.fcgi $(VIPPROC_PATH)/newsdigest.fcgi

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
	
$(VIPPROC_PATH)/newsdigest.fcgi: $(VIP_OBJECT_PATH)/$(VIPNAME)newsdigest.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(VIPPROC_PATH)$(OTJERPROC_VERSION)/newsdigest.fcgi
	
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

$(VIP_OBJECT_PATH)/$(VIPNAME)newsdigest.o:
	$(COMPILE) .$(VIPPROC_PATH)$(OTJERPROC_VERSION)/newsdigest/newsdigest.cc



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
