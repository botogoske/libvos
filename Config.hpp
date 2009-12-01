/**
 * Copyright (C) 2009 kilabit.org
 * Author:
 *	- m.shulhan (ms@kilabit.org)
 */

#ifndef _LIBVOS_CONFIG_HPP
#define	_LIBVOS_CONFIG_HPP	1

#include "File.hpp"
#include "ConfigData.hpp"

namespace vos {

#define	CFG_HDR			"__CONFIG__"
#define	CFG_CH_COMMENT		'#'
#define	CFG_CH_COMMENT2		';'
#define	CFH_CH_CONT		'\\'
#define	CFG_CH_HEAD_OPEN	'['
#define	CFG_CH_HEAD_CLOSE	']'
#define	CFG_CH_KEY_SEP		'='

enum _cfg_save_mode {
	CONFIG_SAVE_WITH_COMMENT = 0,
	CONFIG_SAVE_WOUT_COMMENT
};

class Config : public File {
public:
	Config();
	~Config();

	void dump();
	int load(const char *ini);
	int save();
	int save_as(const char *ini, const int mode);

	const char *get(const char *head, const char *key, const char *dflt);
	int get_number(const char *head, const char *key, const int dflt);

	int set(const char *head, const char *key, const char *value);
	void add(const char *head, const char *key, const char *value);

	int		_e_row;
	int		_e_col;
	ConfigData	*_data;
private:
	int parsing();
	DISALLOW_COPY_AND_ASSIGN(Config);
};

} /* namespace::vos */

#endif
