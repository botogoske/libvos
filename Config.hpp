/*
 * Copyright (C) 2009 kilabit.org
 * Author:
 *	- m.shulhan (ms@kilabit.org)
 */

#ifndef _LIBVOS_CONFIG_HPP
#define	_LIBVOS_CONFIG_HPP	1

#include "File.hpp"
#include "ConfigData.hpp"

namespace vos {

enum _cfg_ch {
	CFG_CH_COMMENT		= '#',
	CFG_CH_COMMENT2		= ';',
	CFG_CH_CONT		= '\\',
	CFG_CH_HEAD_OPEN	= '[',
	CFG_CH_HEAD_CLOSE	= ']',
	CFG_CH_KEY_SEP		= '='
};

enum _cfg_save_mode {
	CONFIG_SAVE_WITH_COMMENT = 0,
	CONFIG_SAVE_WOUT_COMMENT
};

#define	CONFIG_ROOT	"__CONFIG__"

/**
 * @class		: Config
 * @attr		:
 *	- _data		: list of config headers, keys, and values.
 * @desc		: module for reading config file in INI format.
 */
class Config : public File {
public:
	Config();
	~Config();

	void dump();
	int load(const char *ini);
	int save();
	int save_as(const char *ini, const int mode);

	const char* get(const char *head, const char *key,
			const char *dflt = NULL);

	inline const char* get_no_head(const char *key, const char *dflt = NULL)
	{
		return get(CONFIG_ROOT, key, dflt);
	}

	int get_number(const char *head, const char *key, const int dflt = 0);

	inline int get_number_no_head(const char *key, const int dflt = 0)
	{
		return get_number(CONFIG_ROOT, key, dflt);
	}

	int set(const char *head, const char *key, const char *value);
	void add(const char *head, const char *key, const char *value);

	ConfigData *_data;
private:
	Config(const Config&);
	void operator=(const Config&);

	int parsing();
};

} /* namespace::vos */

#endif
