allsublist = $(shell ls -l |awk '/^d/ {print $$NF}')
allmake = $(foreach n,$(allsublist),$(wildcard ./$(n)/Makefile))
sublist = $(patsubst ./%/Makefile,%,$(allmake))
all:$(sublist)

.PHONY:clean
clean:$(sublist)

.PHONY:static
static:$(sublist)

.PHONY:dynamic
dynamic:$(sublist)

.PHONY:install
install:$(sublist)

$(sublist):ECHO
#	看看相应Makefile文件有没有出现这个指令，方法是统计grep "指令:" 后统计返回的字节数/字符数是否为0
#	直接make有时候会报错(没有事先make static/dynamic/install等)，为防止卡断，if前加个"-"让它报错了也继续跑
	@-if [ `cat $@/Makefile | grep $(MAKECMDGOALS):|wc -c` -ne 0 ] ;then\
		make $(MAKECMDGOALS) -C $@ ;\
	else \
		echo no this command;\
	fi

ECHO:
	@echo start with $(sublist)
