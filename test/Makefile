##
## Copyright 2009-2017 M. Shulhan (ms@kilabit.info). All rights reserved.
## Use of this source code is governed by a BSD-style license that can be found
## in the LICENSE file.
##

SRC_D	=.
BLD_D	=.
LIBVOS_D=..

include $(LIBVOS_D)/Makefile

LIBVOS_OPTS     +=NO_DEFAULT_LIBS

TEST_OBJS=	$(BLD_D)/test.oo		\
		$(LIBVOS_BLD_D)/libvos.oo	\
		$(LIBVOS_BLD_D)/Object.oo	\
		$(LIBVOS_BLD_D)/Error.oo	\
		$(LIBVOS_BLD_D)/Locker.oo	\
		$(LIBVOS_BLD_D)/Buffer.oo	\
		$(LIBVOS_BLD_D)/FmtParser.oo	\
		$(LIBVOS_BLD_D)/File.oo		\
		$(LIBVOS_BLD_D)/Dlogger.oo	\
		$(LIBVOS_BLD_D)/Test.oo

Object_OBJS=	$(TEST_OBJS)

Error_OBJS=	$(TEST_OBJS)

BNode_OBJS=	$(TEST_OBJS) \
		$(LIBVOS_BLD_D)/BNode.oo

Buffer_OBJS=	$(TEST_OBJS)

FmtParser_OBJS=	$(TEST_OBJS)

File_OBJS=	$(TEST_OBJS)

List_OBJS=	$(BNode_OBJS)				\
		$(LIBVOS_BLD_D)/List.oo

Dlogger_OBJS=	$(TEST_OBJS)

ListBuffer_OBJS=					\
			$(List_OBJS)			\
			$(LIBVOS_BLD_D)/List.oo		\
			$(LIBVOS_BLD_D)/ListBuffer.oo

Config_OBJS=	$(File_OBJS) \
		$(ListBuffer_OBJS) \
		$(LIBVOS_BLD_D)/ConfigData.oo \
		$(LIBVOS_BLD_D)/Config.oo

DSVRecordMD_OBJS=	$(List_OBJS)			\
			$(File_OBJS)			\
			$(LIBVOS_BLD_D)/DSVRecordMD.oo

SSVReader_OBJS=		$(ListBuffer_OBJS)		\
			$(LIBVOS_BLD_D)/File.oo		\
			$(LIBVOS_BLD_D)/DSVRecord.oo	\
			$(LIBVOS_BLD_D)/Rowset.oo	\
			$(LIBVOS_BLD_D)/SSVReader.oo


SockAddr_OBJS=	$(TEST_OBJS)			\
		$(LIBVOS_BLD_D)/SockAddr.oo

ListSockAddr_OBJS=	$(SockAddr_OBJS) \
			$(ListBuffer_OBJS) \
			$(LIBVOS_BLD_D)/ListSockAddr.oo

DNSQuery_OBJS=		$(SSVReader_OBJS)		\
			$(LIBVOS_BLD_D)/DNS_rr.oo	\
			$(LIBVOS_BLD_D)/DNSQuery.oo	\
			$(LIBVOS_BLD_D)/DNSRecordType.oo

host_to_dnsquery_OBJS=	$(SSVReader_OBJS)	\
			$(DNSQuery_OBJS)

Rowset_OBJS=	$(List_OBJS)			\
		$(LIBVOS_BLD_D)/Rowset.oo

Resolver_OBJS=	\
		$(ListSockAddr_OBJS)		\
		$(DNSQuery_OBJS)		\
		$(LIBVOS_BLD_D)/ListBuffer.oo	\
		$(LIBVOS_BLD_D)/Socket.oo	\
		$(LIBVOS_BLD_D)/Resolver.oo

Locker_OBJS=	$(TEST_OBJS)			\
		$(BLD_D)/TestLocker.oo

File_OBJS=	$(TEST_OBJS)			\
		$(LIBVOS_BLD_D)/File.oo

Socket_OBJS=	$(File_OBJS)			\
		$(LIBVOS_BLD_D)/SockAddr.oo	\
		$(LIBVOS_BLD_D)/Socket.oo

SockServer_OBJS=$(Socket_OBJS)			\
		$(LIBVOS_BLD_D)/SockServer.oo

FTPD_OBJS=	$(List_OBJS)			\
		$(SockServer_OBJS)		\
		$(LIBVOS_BLD_D)/Dir.oo		\
		$(LIBVOS_BLD_D)/DirNode.oo	\
		$(LIBVOS_BLD_D)/FTP_cmd.oo	\
		$(LIBVOS_BLD_D)/FTPD_user.oo	\
		$(LIBVOS_BLD_D)/FTPD_cmd.oo	\
		$(LIBVOS_BLD_D)/FTPD_client.oo	\
		$(LIBVOS_BLD_D)/FTPD.oo

TreeNode_OBJS=	$(BNode_OBJS)			\
		$(LIBVOS_BLD_D)/TreeNode.oo

RBT_OBJS=	$(TreeNode_OBJS)		\
		$(Locker_OBJS)			\
		$(LIBVOS_BLD_D)/RBT.oo

Thread_OBJS=	$(Locker_OBJS)			\
		$(LIBVOS_BLD_D)/Thread.oo

User_OBJS=	$(TEST_OBJS)			\
		$(LIBVOS_BLD_D)/User.oo

Dir_OBJS=	$(TEST_OBJS)			\
		$(LIBVOS_BLD_D)/DirNode.oo	\
		$(LIBVOS_BLD_D)/Dir.oo

TARGET=	$(BLD_D)/Object.test		\
	$(BLD_D)/Error.test		\
	$(BLD_D)/Buffer.test		\
	$(BLD_D)/FmtParser.test		\
	$(BLD_D)/File.test		\
	$(BLD_D)/BNode.test		\
	$(BLD_D)/List.test		\
	$(BLD_D)/ListBuffer.test	\
	$(BLD_D)/Config.test		\
	$(BLD_D)/Dlogger.test		\
	$(BLD_D)/SSVReader.test		\
	$(BLD_D)/SockAddr.test		\
	$(BLD_D)/ListSockAddr.test	\
	$(BLD_D)/host_to_dnsquery.test	\
	$(BLD_D)/Resolver.test		\
	$(BLD_D)/Rowset.test		\
	$(BLD_D)/Locker.test		\
	$(BLD_D)/FTPD.test		\
	$(BLD_D)/DSVRecordMD.test	\
	$(BLD_D)/RBT.test		\
	$(BLD_D)/Thread.test		\
	$(BLD_D)/Dir.test

.PHONY: all clean

all: libvos-debug

clean: libvos-clean
	@$(call do_rm,$(TARGET))

$(BLD_D)/%.oo: $(SRC_D)/%.cc $(SRC_D)/%.hh
	@$(do_compile)

$(BLD_D)/%.test.oo: $(SRC_D)/%.test.cc
	@$(do_compile)

$(BLD_D)/%.test: $(BLD_D)/%.test.oo $$(%_OBJS)
	@$(do_build)
	@echo "[T] $@"
	$(BLD_D)/$@ || exit

$(BLD_D)/User.test: $(BLD_D)/User.test.oo $(User_OBJS)
	@$(do_build)
	@echo "[T] $@"
	-sudo $(BLD_D)/$@


## vi: ts=8 sw=8 tw=78:
